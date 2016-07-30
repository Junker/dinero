/*
 * Copyright (C) Dmitry Kosenkov <junker@front.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include <stdarg.h>
#include <sql-parser/gda-sql-parser.h>

#include "db.h"
#include "sutil.h"


static void _get_vargs_params(GdaStatement *stmt, GdaSet *params, va_list vargs);
static void sql_dump(GdaStatement *stmt, GdaSet *params);


gboolean db_connect(gchar * dsn) 
{
	GError *error = NULL;
	
	connection = gda_connection_open_from_string("SQLite", dsn, NULL, GDA_CONNECTION_OPTIONS_NONE, &error);
	if (!connection && error) 
	{
		g_error(error->message);
		return FALSE;
	}

	parser = gda_connection_create_parser(connection);
	
	const gchar *sqlite_ver = g_value_get_string(db_get_value("SELECT sqlite_version()"));
	gint sqlite_major_ver,
	     sqlite_minor_ver,
	     sqlite_patch_ver;

	sscanf(sqlite_ver, "%d.%d.%d", &sqlite_major_ver, &sqlite_minor_ver, &sqlite_patch_ver);

	if (sqlite_major_ver < 3 || 
	    (sqlite_major_ver == 3 && sqlite_minor_ver < 6) ||
	    (sqlite_major_ver == 3 && sqlite_minor_ver == 6 && sqlite_patch_ver < 19)
	   )
	{
		g_error("Sqlite version >= 3.6.19 needed");
	}   
	
	db_exec_select_sql ("PRAGMA foreign_keys = ON");

	g_debug("Connected to DSN: %s", dsn);
	
	return TRUE;
}

void db_disconnect()
{
	gda_connection_close(connection);
}

 
GdaSet* db_exec_sql(const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaSet *inserted_row;
	GError *error = NULL;

	stmt = gda_sql_parser_parse_string(parser, sql, NULL, &error);
	if (error) 
		g_error(error->message);
	
	gda_statement_get_parameters(stmt, &params, NULL);

	va_list vargs;
	va_start(vargs, sql);
	_get_vargs_params(stmt, params, vargs);
	va_end (vargs);
	
	sql_dump(stmt, params);

	gda_connection_statement_execute_non_select(connection, stmt, params, &inserted_row, &error);
	if (error)
	{
		str_replace_character(error->message, "%", "_");
		g_error(error->message);
	}

	if (params) g_object_unref(params);
	g_object_unref(stmt);

	return inserted_row;
}

GdaDataModel* db_exec_select_sql(const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaDataModel *model;
	GError *error = NULL;
	
	stmt = gda_sql_parser_parse_string (parser, sql, NULL, &error);
	if (error) 
		g_error(error->message);

	gda_statement_get_parameters(stmt, &params, NULL);

	va_list vargs;
	va_start(vargs, sql);
	_get_vargs_params(stmt, params, vargs);
	va_end (vargs);

	sql_dump(stmt, params);
	
	model = gda_connection_statement_execute_select (connection, stmt, params, &error);
	if (error)
	{
		str_replace_character(error->message, "%", "_");
		g_error(error->message);
	}		

	if (params) g_object_unref(params);
	g_object_unref(stmt);
	
	return model;
}

const GValue* db_get_value(const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaDataModel *model;
	GError *error = NULL;
	
	stmt = gda_sql_parser_parse_string(parser, sql, NULL, &error);
	if (error) 
		g_error(error->message);
	
	gda_statement_get_parameters (stmt, &params, NULL);


	va_list vargs;
	va_start(vargs, sql);
	_get_vargs_params(stmt, params, vargs);
	va_end (vargs);
	
	sql_dump(stmt, params);

	model = gda_connection_statement_execute_select (connection, stmt, params, &error);
	if (error)
	{
		str_replace_character(error->message, "%", "_");
		g_error(error->message);
	}

	if (gda_data_model_get_n_rows(model) <= 0) return NULL;
	
	const GValue *value = gda_data_model_get_value_at(model, 0, 0, &error);
	if (error) 
		g_error(error->message);

	GValue *result = g_new0(GValue, 1);
	g_value_init(result, G_VALUE_TYPE(value));
	g_value_copy(value, result);
	
	if (params) 
		g_object_unref(params);
	
	g_object_unref(stmt);
	g_object_unref(model);
	
	return result;
}

static void sql_dump(GdaStatement *stmt, GdaSet *params)
{
	GString *result;
	gchar *sql;

	if (g_getenv("GDA_SQL_DEBUG"))
		return;

	sql = gda_statement_to_sql(stmt, params, NULL);
	str_replace_character(sql, "%", "_");
	
	result = g_string_new("SQL: ");
	g_string_append(result, sql);
	
	if (params)
	{
		g_string_append(result, " [ ");
		
		int i;
		for (i = 0; i<1000; i++)
		{
			GdaHolder *holder = gda_set_get_nth_holder(params, i);

			if (!holder) break;

			if (i != 0)
				g_string_append(result, ", ");
			
			const gchar *name = gda_holder_get_id(holder);
			gchar *value = gda_holder_get_value_str(holder, NULL);

			g_string_append_printf(result, "%s => %s", name, value);
		}

		g_string_append(result, " ]");
	}

	g_debug(result->str);
}



static void _get_vargs_params(GdaStatement *stmt, GdaSet *params, va_list vargs)
{
	if (params)
	{
		for (;;)
		{
			const gchar *var_name = va_arg(vargs, gchar*);
			const GValue *var_value = va_arg(vargs, GValue*);

			if (!var_name) break;

			GdaHolder *holder = gda_set_get_holder(params, var_name);

			if (!holder) continue;
			
			gda_holder_set_value (holder, var_value, NULL);	
		}
	}
}



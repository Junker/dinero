/*
 * Copyright (C) Dmitry Kosenkov 2009 <junker@front.ru>
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
 
#include <glib/gi18n.h>
#include <stdarg.h>
#include <sql-parser/gda-sql-parser.h>

#include "db.h"
#include "actions.h"
#include "sutil.h"



gboolean db_connect (gchar * dsn) 
{
	GError *error = NULL;
	
	connection = gda_connection_open_from_string("SQLite", dsn, NULL, GDA_CONNECTION_OPTIONS_NONE, &error);
	if (!connection && error) 
	{
		show_error_dialog (_("DB connect failed"), error->message, NULL);
		return FALSE;
	}

	parser = gda_connection_create_parser(connection);
	
	gchar *sqlite_ver = g_value_get_string(db_get_value ("SELECT sqlite_version()", NULL));
	gint sqlite_major_ver,
	     sqlite_minor_ver,
	     sqlite_patch_ver;

	sscanf(sqlite_ver, "%d.%d.%d", &sqlite_major_ver, &sqlite_minor_ver, &sqlite_patch_ver);

	if (sqlite_major_ver < 3 || 
	    (sqlite_major_ver == 3 && sqlite_minor_ver < 6) ||
	    (sqlite_major_ver == 3 && sqlite_minor_ver == 6 && sqlite_patch_ver < 19)
	   )
	{
		show_error_dialog ("Sqlite version", "Sqlite version >= 3.6.19 needed" , NULL);
		exit(1);
	}   
	
	db_exec_select_sql ("PRAGMA foreign_keys = ON", NULL);

	return TRUE;
}

 
GdaSet* db_exec_sql (const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaSet *inserted_row;
	GError *error = NULL;

	stmt = gda_sql_parser_parse_string (parser, sql, NULL, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);
	gda_statement_get_parameters (stmt, &params, NULL);

	if (params) 
	{
		va_list ap;
		va_start(ap, sql);
		for (;;)
		{
			gchar *var_name = va_arg(ap, gchar*);
			GValue *var_value = va_arg(ap, GValue*);

			if (!var_name) break;

			GdaHolder *holder = gda_set_get_holder(params, var_name);

			if (!holder) break;
			
			//g_debug("VARNAME:%s VARVALUE:%s", var_name, gda_value_stringify(var_value));
			gda_holder_set_value (holder, var_value, NULL);
		}
		va_end (ap);
	}
	
	//g_debug(gda_statement_to_sql(stmt, params, NULL));
	gda_connection_statement_execute_non_select (connection, stmt, params, &inserted_row, &error);
	if (error) show_error_dialog (_("Query failed"), error->message, NULL);

	if (params) g_object_unref(params);
	g_object_unref(stmt);

	return inserted_row;
}

GdaDataModel* db_exec_select_sql (const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaDataModel *model;
	GError *error = NULL;

	//g_debug("SELECT SQL: %s", sql);
	
	stmt = gda_sql_parser_parse_string (parser, sql, NULL, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);

	gda_statement_get_parameters (stmt, &params, NULL);

	if (params)
	{
		va_list ap;
		va_start(ap, sql);
		for (;;)
		{
			const gchar *var_name = va_arg(ap, gchar*);
			const GValue *var_value = va_arg(ap, GValue*);

			if (!var_name) break;

			GdaHolder *holder = gda_set_get_holder(params, var_name);

			if (!holder) break;

			//g_debug("VARNAME:%s VARVALUE:%s", var_name, gda_value_stringify(var_value));
			gda_holder_set_value (holder, var_value, NULL);
		}
		va_end (ap);	
	}
	
	//g_debug(gda_statement_to_sql(stmt, params, NULL));
	model = gda_connection_statement_execute_select (connection, stmt, params, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);

	if (params) g_object_unref(params);
	g_object_unref(stmt);
	
	return model;
}

const GValue* db_get_value (const gchar *sql, ...)
{
	GdaStatement *stmt;
	GdaSet *params;
	GdaDataModel *model;
	GError *error = NULL;
	
	stmt = gda_sql_parser_parse_string (parser, sql, NULL, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);
	gda_statement_get_parameters (stmt, &params, NULL);


	va_list ap;
	va_start(ap, sql);
	for (;;)
	{
		gchar *var_name = va_arg(ap, gchar*);
		GValue *var_value = va_arg(ap, GValue*);

		if (!var_name) break;
		
		//g_debug("VARNAME:%s VARVALUE:%s", var_name, gda_value_stringify(var_value));
		gda_holder_set_value (gda_set_get_holder(params, var_name), var_value, NULL);
	}
	va_end (ap);
	
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);

	model = gda_connection_statement_execute_select (connection, stmt, params, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);

	if (gda_data_model_get_n_rows(model) <= 0) return NULL;
	
	const GValue *value = gda_data_model_get_value_at(model, 0, 0, &error);
	if (error) show_error_dialog (_("Query failed"), error->message , NULL);

	GValue *result = g_new0 (GValue, 1);
	g_value_init(result, G_VALUE_TYPE(value));
	g_value_copy(value, result);
	
	if (params) g_object_unref(params);
	g_object_unref(stmt);
	g_object_unref(model);
	
	return result;
}





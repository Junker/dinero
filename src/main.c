
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Dmitry Kosenkov 2009 <junker@front.ru>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libgda/libgda.h>

#include "main.h"
#include "mainform.h"
#include "db.h"



#define DB_FILE_NAME "dinero.sqlite"

static void create_lookup_models();
static void check_debtcredit_remain();

static void init_periodicity_model();

void destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
}


int main (int argc, char *argv[])
{
	GtkWidget *window;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init(&argc, &argv);

	setlocale(LC_NUMERIC, "POSIX");

	home_path = g_build_filename(g_get_home_dir(), ".config", PACKAGE_NAME, NULL);

	gchar dsn[1000];
	//Connect
	gchar *dbfilename = g_build_filename(home_path, DB_FILE_NAME, NULL);

	if (g_file_test(dbfilename, G_FILE_TEST_EXISTS))
		g_sprintf(dsn, "DB_DIR=%s;DB_NAME=%s", home_path, DB_FILE_NAME);
	else
	{
		g_mkdir_with_parents(home_path, 0755);
	
		gchar *source_path = g_build_filename(PACKAGE_DATA_DIR, PACKAGE_NAME, DB_FILE_NAME, NULL);

		if (!g_file_test(source_path, G_FILE_TEST_EXISTS))
		{
			g_print("%s File not found", source_path);
			exit(-1);
		}
		    
		gchar *dest_path   = dbfilename;
		GFile *source = g_file_new_for_path (source_path);
		GFile *dest   = g_file_new_for_path (dest_path);

		GError *error = NULL;
		g_file_copy (source, dest, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);

		g_sprintf(dsn, "DB_DIR=%s;DB_NAME=%s", home_path, DB_FILE_NAME);

		g_free(source_path);
	}

	gdaui_init();

	db_connect (dsn);

	create_lookup_models();

	
	window = create_main_window ();	
//	gtk_widget_show_all (window);

	check_debtcredit_remain();

	show_plan_payment_window ();

	
	gtk_main ();

	
	return 0;
}




void check_debtcredit_remain()
{
	GValue *date = ex_value_new_int (get_current_date());

	GdaDataModel *model = db_exec_select_sql ("SELECT id FROM debtcredit b \
	                                             WHERE b.remind=1 AND b.remind_date<##date::gint",
	                                          "date", date,
	                                          NULL);

	
	int i;
	for(i=0; i < gda_data_model_get_n_rows(model); i++) 
	{
		GValue *id = gda_data_model_get_value_at(model, 0, i, NULL);

		show_remind_debt_window (id);
		                                    
	}

	g_free(date);
	g_object_unref(model);
}



void create_lookup_models() 
{
	refresh_account_model();
	refresh_currency_model();
	refresh_category_model();
	refresh_in_category_model();
	refresh_out_category_model();
	refresh_subcategory_model();
	refresh_unit_model();
	refresh_person_model();
	init_periodicity_model();
}

void refresh_account_model()
{
	if (G_IS_OBJECT(account_model)) g_object_unref(account_model);
	account_model = db_exec_select_sql ("SELECT id,name FROM account", NULL);
}

void refresh_currency_model()
{
	if (G_IS_OBJECT(currency_model)) g_object_unref(currency_model);
	currency_model = db_exec_select_sql ("SELECT id,name FROM currency", NULL);
}

void refresh_category_model()
{
	if (G_IS_OBJECT(in_category_model)) g_object_unref(category_model);
	category_model = db_exec_select_sql ("SELECT id,name FROM category", NULL);
}

void refresh_in_category_model()
{
	if (G_IS_OBJECT(in_category_model)) g_object_unref(in_category_model);
	in_category_model = db_exec_select_sql ("SELECT id,name FROM category WHERE type=2", NULL);
}

void refresh_out_category_model()
{
	if (G_IS_OBJECT(out_category_model)) g_object_unref(out_category_model);
	out_category_model = db_exec_select_sql ("SELECT id,name FROM category WHERE type=1", NULL);
}

void refresh_subcategory_model()
{
	if (G_IS_OBJECT(subcategory_model)) g_object_unref(subcategory_model);
	subcategory_model = db_exec_select_sql ("SELECT id,name FROM subcategory", NULL);
}

void refresh_unit_model()
{
	if (G_IS_OBJECT(unit_model)) g_object_unref(unit_model);
	unit_model = db_exec_select_sql ("SELECT id,name FROM unit", NULL);
}

void refresh_person_model()
{
	if (G_IS_OBJECT(person_model)) g_object_unref(person_model);
	person_model = db_exec_select_sql ("SELECT id,name FROM person", NULL);
}

void init_periodicity_model()
{
	GValue *once = ex_value_new_string (_("Once"));
	GValue *day = ex_value_new_string (_("Daily"));
	GValue *month = ex_value_new_string (_("Monthly"));
	GValue *quarter = ex_value_new_string (_("Quarterly"));
	GValue *year = ex_value_new_string (_("Yearly"));
	
	db_exec_sql("CREATE TEMPORARY TABLE tmp_periodicity(id INTEGER, name TEXT)", NULL);

	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(0, ##name::string)", "name", once, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(1, ##name::string)", "name", day, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(2, ##name::string)", "name", month, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(3, ##name::string)", "name", quarter, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(4, ##name::string)", "name", year, NULL);

	periodicity_model = db_exec_select_sql ("SELECT id,name FROM tmp_periodicity", NULL);
	            
	g_free(once);
	g_free(day);
	g_free(month);
	g_free(quarter);
	g_free(year);

	            
}
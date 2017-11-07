
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Dmitry Kosenkov <junker@front.ru>
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

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <libgda/libgda.h>

#include "main.h"
#include "common.h"
#include "mainform.h"
#include "db.h"
#include "plan.h"
#include "sutil.h"
#include "actions.h"
#include "reminddebt.h"


void critical_log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);

static void check_debtcredit_remain();
static void init_periodicity_model();
static void app_activate(GtkApplication* app, gpointer user_data);

int main(int argc, char *argv[])
{
	GtkApplication *app;
	int status;

	//init gettext
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	textdomain (PACKAGE);

	//init log handler
//	g_log_set_handler(NULL, G_LOG_LEVEL_CRITICAL, critical_log_handler, NULL);
	
	app = gtk_application_new("org.junker.dinero", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

static void app_activate(GtkApplication* app, gpointer user_data)
{
	GList *list;
	list = gtk_application_get_windows(app);

	if (list)
	{
		gtk_window_present(GTK_WINDOW(list->data));

		return;
	}

	//locale for float numbers (for SQL)
	setlocale(LC_NUMERIC, "POSIX");

	open_main_db();

	gdaui_init();

	create_lookup_models();

	main_window = create_main_window();
	gtk_window_set_application(GTK_WINDOW(main_window), app);
	gtk_widget_show(main_window);

	check_debtcredit_remain();

	show_plan_payment_window();

	gtk_main();
}


void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
}


void critical_log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
	g_log_default_handler(log_domain, log_level, message, user_data);
	show_error_dialog(_("Query failed"), message, NULL);
}

void open_main_db()
{
	home_path = g_build_filename(g_get_home_dir(), ".config", PACKAGE_NAME, NULL);

	gchar dsn[1000];


	gchar *dbfilename = g_build_filename(home_path, DB_FILE_NAME, NULL);

	if (g_file_test(dbfilename, G_FILE_TEST_EXISTS))
		g_sprintf(dsn, "DB_DIR=%s;DB_NAME=%s", home_path, DB_FILE_NAME);
	else
	{
		g_mkdir_with_parents(home_path, 0755);
	
		gchar *source_path = g_build_filename(PACKAGE_DATA_DIR, DB_FILE_NAME, NULL);

		if (!g_file_test(source_path, G_FILE_TEST_EXISTS))
		{
			g_error("%s File not found", source_path);
		}
		    
		gchar *dest_path = dbfilename;
		GFile *source = g_file_new_for_path (source_path);
		GFile *dest   = g_file_new_for_path (dest_path);

		GError *error = NULL;
		g_file_copy (source, dest, G_FILE_COPY_NONE, NULL, NULL, NULL, &error);
		if (error)
			g_error(error->message);

		g_sprintf(dsn, "DB_DIR=%s;DB_NAME=%s", home_path, DB_FILE_NAME);

		g_free(source_path);
	}

	db_connect(dsn);
}

void disconnect_db()
{

}

static void check_debtcredit_remain()
{
	GValue *date = ex_value_new_int (get_current_date());

	GdaDataModel *model = db_exec_select_sql ("SELECT id FROM debtcredit b \
	                                             WHERE b.remind=1 AND b.remind_date<##date::gint",
	                                          "date", date,
	                                          NULL);

	
	int i;
	for(i=0; i < gda_data_model_get_n_rows(model); i++) 
	{
		const GValue *id = gda_data_model_get_value_at(model, 0, i, NULL);

		show_remind_debt_window(id);
		                                    
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



static void init_periodicity_model()
{
	GValue *once = ex_value_new_string (_("Once"));
	GValue *day = ex_value_new_string (_("Daily"));
	GValue *month = ex_value_new_string (_("Monthly"));
	GValue *quarter = ex_value_new_string (_("Quarterly"));
	GValue *year = ex_value_new_string (_("Yearly"));
	
	db_exec_sql("CREATE TEMPORARY TABLE tmp_periodicity(id INTEGER, name TEXT)");

	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(0, ##name::string)", "name", once, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(1, ##name::string)", "name", day, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(2, ##name::string)", "name", month, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(3, ##name::string)", "name", quarter, NULL);
	db_exec_sql ("INSERT INTO tmp_periodicity(id,name) VALUES(4, ##name::string)", "name", year, NULL);

	periodicity_model = db_exec_select_sql ("SELECT id,name FROM tmp_periodicity");
	            
	g_free(once);
	g_free(day);
	g_free(month);
	g_free(quarter);
	g_free(year);

	            
}
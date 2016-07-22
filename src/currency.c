/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * currency.c
 * Copyright (C) Dmitry Kosenkov 2009 <junker@front.ru>
 * 
 * callbacks.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * callbacks.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h> 
#include <libgda-ui/libgda-ui.h>
#include <string.h> 
#include <glib/gprintf.h> 
#include <glib/gi18n-lib.h>

#include "currency.h"
#include "db.h"
#include "actions.h"
#include "common.h"

#include "ex-grid.h"
#include "sutil.h"
 
static  ExGrid *grid;
static 	GtkWidget *window;

static void fill_grid(void);
static void show_editcurrency_window (OperType oper);

static enum {
	ID_COL,
	NAME_COL,
	SHORT_NAME_COL,
	MAIN_COL
};


void show_currency_window (void)
{

	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "currency.glade");
	gtk_builder_connect_signals (gbuilder, NULL);

	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *scrolledwindow_grid = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid"));

	grid = create_grid (scrolledwindow_grid);

	fill_grid();
	
	gtk_widget_show(window);
}

void fill_grid(void)
{
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id, name, short_name, is_main FROM currency ORDER BY name",NULL);
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), ID_COL, FALSE);

	ex_grid_column_set_title (grid, NAME_COL, _("Name"));
	ex_grid_column_set_title (grid, SHORT_NAME_COL, _("Short name"));
	ex_grid_column_set_title (grid, MAIN_COL, _("Main"));

	
	g_object_unref(db_model);
}


void show_editcurrency_window (OperType oper)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editcurrency.glade");
	gtk_builder_connect_signals (gbuilder, NULL);

	GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object (gbuilder, "dialog"));
	GtkEntry *entry_name = GTK_ENTRY(gtk_builder_get_object (gbuilder, "entry_name"));
	GtkEntry *entry_short_name = GTK_ENTRY(gtk_builder_get_object (gbuilder, "entry_short_name"));
	GtkCheckButton *checkbutton_main = GTK_CHECK_BUTTON(gtk_builder_get_object (gbuilder, "checkbutton_main"));

	if (oper == OPER_UPDATE) 
	{
		const gchar *name       = g_value_get_string (ex_grid_get_selected_row_value (grid, NAME_COL));
		const gchar *short_name = g_value_get_string (ex_grid_get_selected_row_value (grid, SHORT_NAME_COL));
		const gboolean main     = g_value_get_boolean (ex_grid_get_selected_row_value (grid, MAIN_COL));
		
		gtk_entry_set_text (entry_name, name);
		gtk_entry_set_text (entry_short_name, short_name);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(checkbutton_main), main);
	}

	                             
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK) 
	{
		GValue *id = ex_grid_get_selected_row_value (grid, ID_COL);

		GValue *name       = ex_value_new_string (gtk_entry_get_text (entry_name));
		GValue *short_name = ex_value_new_string (gtk_entry_get_text (entry_short_name));
		GValue *is_main    = ex_value_new_int (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkbutton_main)));


		gchar sql[500];
		if (oper == OPER_INSERT)
		{
			strcpy(sql, "INSERT INTO currency(name,short_name,is_main) VALUES(##name::string,##short_name::string,##is_main::gint)");
		}
		else if (oper == OPER_UPDATE)
		{
			strcpy(sql, "UPDATE currency SET name=##name::string, short_name=##short_name::string, is_main=##is_main::gint \
			               WHERE id=##id::gint");

		}

		db_exec_sql(sql,
		            "id", id,
                    "name", name,
		            "short_name", short_name,
		            "is_main", is_main,
		            NULL
		           );
	

		fill_grid();
		refresh_currency_model();

		g_free(name);
		g_free(short_name);
		g_free(is_main);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

}


void on_currency_button_add_clicked(GtkButton *button, gpointer user_data) 
{	
	show_editcurrency_window (OPER_INSERT);
}

void on_currency_button_edit_clicked(GtkButton *button,gpointer user_data) 
{	
	show_editcurrency_window (OPER_UPDATE);
}

void on_currency_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you remove the currency, all operations associated with this currency will be removed. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;

	GValue *id = ex_grid_get_selected_row_value(grid, 0);	
	
	db_exec_sql("DELETE FROM currency WHERE id=##id::gint", "id", id, NULL);
	
	fill_grid();
	refresh_currency_model();
}



	

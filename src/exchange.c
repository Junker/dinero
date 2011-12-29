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

#include <config.h> 
#include <libgda-ui/libgda-ui.h>
#include <string.h> 
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "main.h"
#include "exchange.h"
#include "mainform.h"
#include "db.h"
#include "actions.h"

static enum  {
	ID_COL,
	TIME_T_COL,
	DATE_COL,
	ACCOUNT_ID_COL,
	OUT_AMOUNT_COL,
	OUT_CURRENCY_ID_COL,
	IN_AMOUNT_COL,
	IN_CURRENCY_ID_COL,
	DESCR_COL
};


static ExGrid *grid;
static 	GtkWidget *window;
static GtkButton *button_del;

static void fill_grid(void);
static void on_grid_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_button_del_clicked (GtkButton *button,gpointer user_data);

void show_exchange_window (void)
{

	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "gridform.glade");
	gtk_builder_connect_signals (gbuilder,NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *scrolledwindow_grid = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid"));
	button_del = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_del"));

	GtkWidget *button_add = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_add"));
	GtkWidget *button_edit = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_edit"));

	gtk_widget_set_visible (button_add, FALSE);
	gtk_widget_set_visible (button_edit, FALSE);
	
	grid = create_grid (scrolledwindow_grid);

	g_signal_connect (G_OBJECT(grid), "selection-changed", G_CALLBACK (on_grid_selection_changed), NULL);
	g_signal_connect (G_OBJECT(button_del), "clicked", G_CALLBACK (on_button_del_clicked), NULL);


	fill_grid();
	
	gtk_widget_show(window);
}

void fill_grid(void)
{
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT a.id, b.date, strftime('%d.%m.%Y',date(b.date)), b.account_id, b.amount, b.currency_id, c.amount, c.currency_id,b.description \
	                                               FROM exchange a, operation b, operation c WHERE a.expenditure_id=b.id AND a.income_id=c.id \
	                                               ORDER BY b.date DESC",NULL);
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid),db_model);

	ex_grid_lookup_field(grid, ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field(grid, OUT_CURRENCY_ID_COL, currency_model, 0);
	refresh_currency_model(); //Dirty hack
	ex_grid_lookup_field(grid, IN_CURRENCY_ID_COL, currency_model, 0);

	ex_grid_column_set_title(grid, DATE_COL,_("Date"));
	ex_grid_column_set_title(grid, ACCOUNT_ID_COL,_("Account"));
	ex_grid_column_set_title(grid, OUT_AMOUNT_COL,_("Amount"));
	ex_grid_column_set_title(grid, OUT_CURRENCY_ID_COL,_("From currency"));
	ex_grid_column_set_title(grid, IN_AMOUNT_COL,_("Amount"));
	ex_grid_column_set_title(grid, IN_CURRENCY_ID_COL,_("To Currency"));
	ex_grid_column_set_title(grid, DESCR_COL,_("Description"));

	ex_grid_column_set_format(grid, OUT_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, IN_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), TIME_T_COL, FALSE);	

	gtk_widget_set_sensitive(GTK_WIDGET(button_del),FALSE);

	g_object_unref(db_model);
}


void on_grid_selection_changed (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_del),TRUE);
}

void on_button_del_clicked (GtkButton *button,gpointer user_data)
{
	gint response = show_warning_dialog(_("Are you sure you want to delete?"), GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid,0);	
		
	gchar *sql = "DELETE FROM exchange WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);

	fill_grid();
	fill_grid_account_full();
	fill_grid_account_short();

}
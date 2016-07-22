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

#include "config.h"

#include <string.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "common.h"
#include "editaccount.h"
#include "actions.h"
#include "mainform.h"
#include "ex-grid.h"
#include "sutil.h"
#include "db.h"

static GtkWidget *dialog,
                 *button_edit_currency;
static GtkEntry *entry_name;
static GtkTextView *textview_description;
static ExGrid *grid_currency;

static void fill_grid_currency ();
static void set_fields_values();
static void on_button_ok_clicked(GtkButton *button, OperType oper);
static void on_button_edit_currency_clicked(GtkButton *button, gpointer data);
static void on_grid_currency_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);



static enum {
		ID_COL,
		CURRENCY_ID_COL,
		AMOUNT_COL,
		ACTIVE_COL,
};

gint show_editaccount_window (OperType oper)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editaccount.ui");
	gtk_builder_connect_signals (gbuilder,NULL);

	GtkContainer *scrolledwindow_grid = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid"));
	
	dialog = GTK_WIDGET(gtk_builder_get_object (gbuilder, "dialog"));
	
	entry_name           = GTK_ENTRY(gtk_builder_get_object (gbuilder, "entry_name"));
	textview_description = GTK_TEXT_VIEW(gtk_builder_get_object (gbuilder, "textview_description"));

	GtkWidget *button_ok            = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_ok"));
	GtkWidget *button_cancel        = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_cancel"));
	GtkWidget *label_grid           = GTK_WIDGET(gtk_builder_get_object (gbuilder, "label_grid"));

	button_edit_currency = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_edit_currency"));

	g_signal_connect (G_OBJECT(button_ok), "clicked", G_CALLBACK (on_button_ok_clicked), oper);
	g_signal_connect (G_OBJECT(button_edit_currency), "clicked", G_CALLBACK (on_button_edit_currency_clicked), NULL);
	

	if (oper == OPER_UPDATE) 
	{	
		grid_currency = create_grid(scrolledwindow_grid);
		g_signal_connect (G_OBJECT(grid_currency), "selection_changed", G_CALLBACK (on_grid_currency_selection_changed), NULL);

		set_fields_values();
		fill_grid_currency();

		gtk_widget_set_sensitive(button_edit_currency, FALSE);

	}
	else 
	{
		gtk_widget_hide(GTK_WIDGET(scrolledwindow_grid));
		gtk_widget_hide(GTK_WIDGET(button_edit_currency));
		gtk_widget_hide(GTK_WIDGET(label_grid));
	}
	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	return response;

}	


static void set_fields_values()
{
	gchar *name        = g_value_get_string(ex_grid_get_selected_row_value(grid_account_short, ACS_NAME_COL));
	gchar *description = g_value_get_string(ex_grid_get_selected_row_value(grid_account_short, ACS_DESCR_COL));

	gtk_entry_set_text(entry_name,name);
	ex_textview_set_text(textview_description,description);

}
	
static void fill_grid_currency ()
{
		
	GValue *account = ex_grid_get_selected_row_value(grid_account_short, ACS_ID_COL);	
	
	gchar *sql = "SELECT b.id, a.id, cast(coalesce(b.amount,0) as real), b.active_currency \
	              FROM currency a \
	              LEFT JOIN account_startup b ON a.id=b.currency_id AND b.account_id=##account::gint::NULL \
	              ORDER BY a.name";
	
	GdaDataModel *db_model = db_exec_select_sql (sql, "account", account, NULL);

	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_currency), db_model);

	ex_grid_lookup_field (grid_currency, CURRENCY_ID_COL, currency_model, 0);

	ex_grid_column_set_format(grid_currency, AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid_currency, ACTIVE_COL, EX_GRID_COL_FORMAT_BOOL);

	ex_grid_column_set_title(grid_currency, CURRENCY_ID_COL, _("Name"));
	ex_grid_column_set_title(grid_currency, AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid_currency, ACTIVE_COL, _("Active"));

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_currency), ID_COL, FALSE);

	g_object_unref(db_model);
	
}

void on_grid_currency_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(button_edit_currency, TRUE);
}

void on_button_edit_currency_clicked(GtkButton *button, gpointer data)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editaccountcurrency.ui");
	gtk_builder_connect_signals (gbuilder,NULL);

	GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object (gbuilder, "dialog"));
	GtkSpinButton *spinbutton_amount = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_amount"));
	GtkCheckButton *checkbutton_active = GTK_WIDGET(gtk_builder_get_object (gbuilder, "checkbutton_active"));

	gboolean active = g_value_get_boolean(ex_grid_get_selected_row_value (grid_currency, ACTIVE_COL));
	gdouble amount = g_value_get_double(ex_grid_get_selected_row_value(grid_currency, AMOUNT_COL));
	gtk_toggle_button_set_active(checkbutton_active, active);
	gtk_spin_button_set_value(spinbutton_amount, amount);

	                             
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK) 
	{
		GValue *amount = ex_value_new_double(gtk_spin_button_get_value(spinbutton_amount));
		GValue *active = ex_value_new_int(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkbutton_active)));
		GValue *currency = ex_grid_get_selected_row_value (grid_currency, CURRENCY_ID_COL);
		GValue *id = ex_grid_get_selected_row_value (grid_currency, ID_COL);
		GValue *account = ex_grid_get_selected_row_value(grid_account_short, ACS_ID_COL);		
	
		db_exec_sql("INSERT OR REPLACE INTO account_startup(id, account_id, currency_id, amount, active_currency) \
		               VALUES(##id::gint::NULL, ##account::gint, ##currency::gint, ##amount::gdouble, ##active::gint)",
			        "id", id,
			        "account", account,
			        "currency", currency,
			        "amount", amount,
		            "active", active,
			        NULL
			       );	

		fill_grid_currency();

		g_free(amount);
		g_free(active);
		
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_button_ok_clicked(GtkButton *button, OperType oper)
{

	GValue *name        = ex_value_new_string(gtk_entry_get_text(entry_name));
	GValue *description = ex_value_new_string(ex_textview_get_text(textview_description));	
	
	if (oper == OPER_INSERT)
	{
		GValue *main_currency = db_get_value ("SELECT id FROM currency WHERE is_main=1", NULL);
//		if (!main_currency)
//			show_error_dialog ("Currency
			
		GdaSet *row = db_exec_sql("INSERT INTO account(name,description) VALUES(##name::string, ##description::string)", 
		                          "name", name,
		                          "description", description,
		                          NULL);
		
		GValue *id = gda_set_get_holder_value(row, "+0");

		db_exec_sql("INSERT INTO account_startup(account_id, currency_id, active_currency) VALUES(##account::gint, ##currency::gint, 1)", 
		            "account", id,
		            "currency", main_currency,
		            NULL);
	}

	if (oper == OPER_UPDATE)
	{	
		GValue *id = ex_grid_get_selected_row_value(grid_account_short,ACS_ID_COL);	

		db_exec_sql("UPDATE account SET name=##name::string, description=##description::string WHERE id=##id::gint", 
			        "name", name,
		            "description", description,
		            "id", id,
			        NULL);
	
	}

	g_free(name);
	g_free(description);
	
}
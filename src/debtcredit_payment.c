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
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "common.h"
#include "debtcredit_payment.h"
#include "mainform.h"
#include "actions.h"
#include "db.h"

#include "gtkdateentry.h"

static enum  {
	ID_COL,
	TIME_T_COL,
	DATE_COL,
	AMOUNT_COL,
	CURRENCY_ID_COL,
	DESCR_COL
};


static ExGrid *grid;
static GtkWidget *window;
static GtkButton *button_del,
                 *button_edit,
                 *button_add;

static OperType grid_oper = NULL;


static void fill_grid();
static void on_grid_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_button_del_clicked (GtkButton *button,gpointer user_data);
static void on_button_addedit_clicked (GtkButton *button, OperType oper);

void show_debtcredit_payment_window (OperType oper)
{

	grid_oper = oper;

	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "gridform.glade");
	gtk_builder_connect_signals (gbuilder,NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *scrolledwindow_grid = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid"));

	
	button_del  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_del"));
	button_edit = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_edit"));
	button_add  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_add"));

	grid = create_grid (scrolledwindow_grid);

	g_signal_connect (G_OBJECT(grid), "selection_changed", G_CALLBACK (on_grid_selection_changed), NULL);
	g_signal_connect (G_OBJECT(button_del), "clicked", G_CALLBACK (on_button_del_clicked), NULL);
	g_signal_connect (G_OBJECT(button_edit), "clicked", G_CALLBACK (on_button_addedit_clicked), OPER_UPDATE);
	g_signal_connect (G_OBJECT(button_add), "clicked", G_CALLBACK (on_button_addedit_clicked), OPER_INSERT);

	
	fill_grid();
	
	gtk_widget_show(window);
}



void fill_grid()
{

	ExGrid *source_grid = grid_oper == OPER_DEBT_PAYMENT ? grid_debt : grid_credit;
	GValue *id = ex_grid_get_selected_row_value (source_grid, DEB_ID_COL);

	GdaDataModel *db_model = NULL;

	if (grid_oper == OPER_DEBT_PAYMENT)
	{
		db_model = db_exec_select_sql ("SELECT a.id, b.date, strftime('%d.%m.%Y',date(b.date+1721425)), b.amount, b.currency_id, b.description \
		                                FROM debtcredit_payment a, operation b \
		                                WHERE a.id=b.id AND a.debtcredit_id=##id::gint AND b.amount > 0\
		                                ORDER BY b.date DESC",
		                                "id", id, 
		                               NULL);
	}
	else if (grid_oper == OPER_CREDIT_PAYMENT)
	{
		db_model = db_exec_select_sql ("SELECT a.id, b.date, strftime('%d.%m.%Y',date(b.date+1721425)), b.amount*-1, b.currency_id, b.description \
		                                FROM debtcredit_payment a, operation b \
		                                WHERE a.id=b.id AND a.debtcredit_id=##id::gint AND b.amount < 0\
		                                ORDER BY b.date DESC",
		                                "id", id, 
		                               NULL);		
	}
	
	if (!db_model) return;


	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field(grid, CURRENCY_ID_COL, currency_model, 0);

	ex_grid_column_set_title(grid, DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, DESCR_COL, _("Description"));

	
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), TIME_T_COL, FALSE);

	ex_grid_column_set_format(grid, AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	gtk_widget_set_sensitive(GTK_WIDGET(button_del), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_edit), FALSE);

	g_object_unref(db_model);
}


void on_button_addedit_clicked (GtkButton *button, OperType oper)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editdebtcredit_payment.glade");
	gtk_builder_connect_signals (gbuilder,NULL);

	GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object (gbuilder, "dialog"));
	GtkSpinButton *spinbutton_amount = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_amount"));
	GtkContainer *eventbox_dateedit = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit"));
	GtkTextView *textview_description = GTK_TEXT_VIEW(gtk_builder_get_object (gbuilder, "textview_description"));

	GtkDateEntry *dateedit = create_dateentry(eventbox_dateedit);

	if (oper == OPER_UPDATE)
    {
		gdouble amount     = g_value_get_double(ex_grid_get_selected_row_value(grid, AMOUNT_COL));
		gint date          = g_value_get_int(ex_grid_get_selected_row_value (grid, TIME_T_COL));
		gchar *description = g_value_get_string(ex_grid_get_selected_row_value (grid, DESCR_COL));

		gtk_spin_button_set_value(spinbutton_amount, amount);
		gtk_date_entry_set_date(dateedit, date);
		ex_textview_set_text(textview_description, description);
		
	}
	                             
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK) 
	{
		ExGrid *source_grid = grid_oper == OPER_DEBT_PAYMENT ? grid_debt : grid_credit;
		
		GValue *debtcredit = ex_grid_get_selected_row_value (source_grid, DEB_ID_COL);
		GValue *account    = ex_grid_get_selected_row_value (source_grid, DEB_ACCOUNT_ID_COL);
		GValue *currency   = ex_grid_get_selected_row_value (source_grid, DEB_CURRENCY_ID_COL);
		
		GValue *date        = ex_value_new_int(gtk_date_entry_get_date(dateedit));
//		GValue *amount      = ex_value_new_double(gtk_spin_button_get_value(spinbutton_amount));
		GValue *description = ex_value_new_string(ex_textview_get_text(textview_description));

		GValue *amount      = grid_oper == OPER_DEBT_PAYMENT ? ex_value_new_double(gtk_spin_button_get_value(spinbutton_amount)) :
		                                                       ex_value_new_double(gtk_spin_button_get_value(spinbutton_amount)*-1);
		
		if (oper == OPER_INSERT)
		{
			GdaSet *row = db_exec_sql("INSERT INTO operation(date, account_id,currency_id, amount,description) \
			                             VALUES(##date::gint, ##account::gint, ##currency::gint, ##amount::gdouble, ##descr::string)",
			                           "date", date,                  
			                           "account", account,
			                           "currency", currency,
			                           "amount", amount,
			                           "descr", description,
			                           NULL
			                          );	

			GValue *id = gda_set_get_holder_value(row, "+0");
			
			db_exec_sql("INSERT INTO debtcredit_payment(id, debtcredit_id) \
			           VALUES(##id::gint, ##debtcredit::gint)",
			        "id", id,
			        "debtcredit", debtcredit,
			        NULL
			       );	

		}
		else if (oper == OPER_UPDATE)
		{
			GValue *id = ex_grid_get_selected_row_value (grid, ID_COL);

			GdaSet *row = db_exec_sql("UPDATE operation SET date=##date::gint, amount=##amount::gdouble, description=##descr::string \
			                            WHERE id=##id::gint",
			                          "date", date,    
			                          "amount", amount,
			                          "descr", description,
			                          "id", id,
			                          NULL
			                         );				
		}
			

		
		fill_grid(oper);
		fill_grid_account_full();
		fill_grid_account_short();

		g_free(date);
		g_free(description);
		g_free(amount);

	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

}

void on_grid_selection_changed (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_edit), TRUE);	
}

void on_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("Are you sure you want to delete?"), GTK_WINDOW(window));

	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid, 0);	
	
	gchar *sql = "DELETE FROM operation WHERE id=##id::gint"; 
	db_exec_sql(sql, "id", id, NULL);

	fill_grid();
	fill_grid_account_full();
	fill_grid_account_short();

}
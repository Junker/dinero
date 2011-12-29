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
#include <string.h>
#include <time.h>
#include <glib/gprintf.h>
#include <libgda-ui/libgda-ui.h>
#include <glib/gi18n-lib.h>

#include "main.h"
#include "actions.h"
#include "db.h"
#include "currency.h"
#include "category.h"
#include "mainform.h"
#include "editexpin.h"
#include "editdebt.h"
#include "editaccount.h"
#include "edittransfer.h"
#include "transfer.h"
#include "editexchange.h"
#include "exchange.h"
#include "account_datedetailed.h"
#include "report.h"
#include "debtcredit_payment.h"
#include "editplan.h"

#include "ex-grid.h"
#include "sutil.h"

#include "gtkdateentry.h"

static GtkWidget *window;

static GtkDateEntry  *dateedit_filter_account_from,
                     *dateedit_filter_account_to,
                     *dateedit_filter_expend_from,
                     *dateedit_filter_expend_to,
                     *dateedit_filter_income_from,
                     *dateedit_filter_income_to,
                     *dateedit_filter_plan_expend_from,
                     *dateedit_filter_plan_expend_to,
                     *dateedit_filter_plan_income_from,
                     *dateedit_filter_plan_income_to;

static GdauiCombo *combo_filter_account_account,
                  *combo_filter_expend_account,
                  *combo_filter_expend_category,
                  *combo_filter_expend_subcategory,
                  *combo_filter_income_account,
                  *combo_filter_income_category,
                  *combo_filter_income_subcategory,
                  *combo_filter_plan_expend_account,
                  *combo_filter_plan_expend_category,
                  *combo_filter_plan_expend_subcategory,
                  *combo_filter_plan_income_account,
                  *combo_filter_plan_income_category,
                  *combo_filter_plan_income_subcategory;

static GtkExpander *expander_filter_account;

static gboolean account_filter,
                expend_filter,
                income_filter,
                plan_income_filter,
                plan_expend_filter;

static GtkButton *button_account_add,
                 *button_account_edit,
                 *button_account_del,
                 *button_account_transfer,
                 *button_expenditure_add,
                 *button_expenditure_edit,
                 *button_expenditure_del,
                 *button_income_add,
                 *button_income_edit,
                 *button_income_del,
                 *button_debt_add,
                 *button_debt_edit,
                 *button_debt_del,
                 *button_debt_payments,
                 *button_credit_add,
                 *button_credit_edit,
                 *button_credit_del,
                 *button_credit_payments,
                 *button_account_datedetailed,
                 *button_plan_expenditure_add,
                 *button_plan_expenditure_edit,
                 *button_plan_expenditure_del,
                 *button_plan_expenditure_exec,
                 *button_plan_income_add,
				 *button_plan_income_edit,
                 *button_plan_income_del;
                 *button_plan_income_exec;




static void on_combo_filter_expend_category_changed (GtkComboBox *widget, gpointer user_data);
static void on_combo_filter_income_category_changed (GtkComboBox *widget, gpointer user_data);
static void on_combo_filter_plan_expend_category_changed (GtkComboBox *widget, gpointer user_data);
static void on_combo_filter_plan_income_category_changed (GtkComboBox *widget, gpointer user_data);
static void on_grid_account_short_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_expenditure_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_income_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_debt_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_credit_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_account_full_selection_changed (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_plan_expenditure_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_plan_income_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);

void on_button_account_datedetailed_clicked(GtkButton *button, gpointer user_data);
void on_account_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_expenditure_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_income_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_debt_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_credit_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_debt_button_pay_clicked (GtkButton *button, gpointer user_data);
void on_credit_button_pay_clicked (GtkButton *button, gpointer user_data);
void on_plan_expenditure_button_edit_clicked (GtkButton *button, gpointer user_data);
void on_plan_income_button_edit_clicked (GtkButton *button, gpointer user_data);

GtkWidget* create_main_window (void)
{
	GtkBuilder *gbuilder = gtk_builder_new();

	ex_builder_load_file(gbuilder, "mainform.glade");
	
	gtk_builder_connect_signals (gbuilder, NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	//Grids containers
	GtkContainer *scrolledwindow_account_short    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_account_short"));
	GtkContainer *scrolledwindow_account_full     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_account_full"));
	GtkContainer *scrolledwindow_expenditure      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_expenditure"));
	GtkContainer *scrolledwindow_income           = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_income"));
	GtkContainer *scrolledwindow_debt             = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_debt"));
	GtkContainer *scrolledwindow_credit           = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_credit"));
	GtkContainer *scrolledwindow_plan_expenditure = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_plan_expenditure"));
	GtkContainer *scrolledwindow_plan_income      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_plan_income"));
	

	//Combo containers
	GtkContainer *eventbox_combo_filter_account_account         = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_account_account"));
	GtkContainer *eventbox_combo_filter_expend_account          = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_expend_account"));
	GtkContainer *eventbox_combo_filter_expend_category         = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_expend_category"));
	GtkContainer *eventbox_combo_filter_expend_subcategory      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_expend_subcategory"));
	GtkContainer *eventbox_combo_filter_income_account          = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_income_account"));
	GtkContainer *eventbox_combo_filter_income_category         = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_income_category"));
	GtkContainer *eventbox_combo_filter_income_subcategory      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_income_subcategory"));
	GtkContainer *eventbox_combo_filter_plan_expend_account     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_expend_account"));
	GtkContainer *eventbox_combo_filter_plan_expend_category    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_expend_category"));
	GtkContainer *eventbox_combo_filter_plan_expend_subcategory = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_expend_subcategory"));
	GtkContainer *eventbox_combo_filter_plan_income_account     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_income_account"));
	GtkContainer *eventbox_combo_filter_plan_income_category    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_income_category"));
	GtkContainer *eventbox_combo_filter_plan_income_subcategory = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_filter_plan_income_subcategory"));

	//dateedit containers
	GtkContainer *eventbox_dateedit_filter_account_from      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_account_from"));
	GtkContainer *eventbox_dateedit_filter_account_to        = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_account_to"));
	GtkContainer *eventbox_dateedit_filter_expend_from       = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_expend_from"));
	GtkContainer *eventbox_dateedit_filter_expend_to         = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_expend_to"));
	GtkContainer *eventbox_dateedit_filter_income_from       = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_income_from"));
	GtkContainer *eventbox_dateedit_filter_income_to         = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_income_to"));
	GtkContainer *eventbox_dateedit_filter_plan_expend_from  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_plan_expend_from"));
	GtkContainer *eventbox_dateedit_filter_plan_expend_to    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_plan_expend_to"));
	GtkContainer *eventbox_dateedit_filter_plan_income_from  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_plan_income_from"));
	GtkContainer *eventbox_dateedit_filter_plan_income_to    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_filter_plan_income_to"));

	
	expander_filter_account = GTK_EXPANDER(gtk_builder_get_object (gbuilder, "expander_filter_account"));

	//Buttons
	button_account_add          = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_account_add"));
	button_account_edit         = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_account_edit"));
	button_account_del          = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_account_del"));
	button_account_transfer     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_account_transfer"));
	button_expenditure_add      = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expenditure_add"));
	button_expenditure_edit     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expenditure_edit"));
	button_expenditure_del      = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expenditure_del"));
	button_income_add           = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_add"));
	button_income_edit          = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_edit"));
	button_income_del           = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_del"));
	button_debt_add             = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debt_add"));
	button_debt_edit            = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debt_edit"));
	button_debt_del             = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debt_del"));
	button_debt_payments        = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debt_payments"));
	button_credit_add           = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_credit_add"));
	button_credit_edit          = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_credit_edit"));
	button_credit_del           = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_credit_del"));
	button_credit_payments      = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_credit_payments"));
	button_account_datedetailed = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_account_datedetailed"));
	button_plan_expenditure_add  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_expenditure_add"));
	button_plan_expenditure_edit = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_expenditure_edit"));
	button_plan_expenditure_del  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_expenditure_del"));
	button_plan_expenditure_exec = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_expenditure_exec"));
	button_plan_income_add       = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_income_add"));
	button_plan_income_edit      = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_income_edit"));
	button_plan_income_del       = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_income_del"));
	button_plan_income_exec      = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_plan_income_exec"));
	
	

	//Create Combo controls
	combo_filter_account_account         = create_combo(eventbox_combo_filter_account_account);
	combo_filter_expend_account          = create_combo(eventbox_combo_filter_expend_account);
	combo_filter_expend_category         = create_combo(eventbox_combo_filter_expend_category);
	combo_filter_expend_subcategory      = create_combo(eventbox_combo_filter_expend_subcategory);
	combo_filter_income_account          = create_combo(eventbox_combo_filter_income_account);
	combo_filter_income_category         = create_combo(eventbox_combo_filter_income_category);
	combo_filter_income_subcategory      = create_combo(eventbox_combo_filter_income_subcategory);
	combo_filter_plan_expend_account     = create_combo(eventbox_combo_filter_plan_expend_account);
	combo_filter_plan_expend_category    = create_combo(eventbox_combo_filter_plan_expend_category);
	combo_filter_plan_expend_subcategory = create_combo(eventbox_combo_filter_plan_expend_subcategory);
	combo_filter_plan_income_account     = create_combo(eventbox_combo_filter_plan_income_account);
	combo_filter_plan_income_category    = create_combo(eventbox_combo_filter_plan_income_category);
	combo_filter_plan_income_subcategory = create_combo(eventbox_combo_filter_plan_income_subcategory);

	//Create Grids controls
	grid_income           = create_grid(scrolledwindow_income);
	grid_expenditure      = create_grid(scrolledwindow_expenditure);
	grid_account_short    = create_grid(scrolledwindow_account_short);
	grid_account_full     = create_grid(scrolledwindow_account_full);
	grid_debt             = create_grid(scrolledwindow_debt);
	grid_credit           = create_grid(scrolledwindow_credit);
	grid_plan_income      = create_grid(scrolledwindow_plan_income);
	grid_plan_expenditure = create_grid(scrolledwindow_plan_expenditure);
	
	//Create dateedit controls
	dateedit_filter_account_from     = create_dateentry (eventbox_dateedit_filter_account_from);
	dateedit_filter_account_to       = create_dateentry (eventbox_dateedit_filter_account_to);
	dateedit_filter_expend_from      = create_dateentry (eventbox_dateedit_filter_expend_from);
	dateedit_filter_expend_to        = create_dateentry (eventbox_dateedit_filter_expend_to);
	dateedit_filter_income_from      = create_dateentry (eventbox_dateedit_filter_income_from);
	dateedit_filter_income_to        = create_dateentry (eventbox_dateedit_filter_income_to);
	dateedit_filter_plan_expend_from = create_dateentry (eventbox_dateedit_filter_plan_expend_from);
	dateedit_filter_plan_expend_to   = create_dateentry (eventbox_dateedit_filter_plan_expend_to);
	dateedit_filter_plan_income_from = create_dateentry (eventbox_dateedit_filter_plan_income_from);
	dateedit_filter_plan_income_to   = create_dateentry (eventbox_dateedit_filter_plan_income_to);


	//Connect signals
	g_signal_connect (G_OBJECT(dateedit_filter_account_from), "changed", G_CALLBACK(fill_grid_account_full),NULL);
	g_signal_connect (G_OBJECT(dateedit_filter_account_to), "changed", G_CALLBACK(fill_grid_account_full),NULL);
	g_signal_connect (G_OBJECT(dateedit_filter_expend_from), "changed", G_CALLBACK(fill_grid_expenditure),NULL);
	g_signal_connect (G_OBJECT(dateedit_filter_expend_to), "changed", G_CALLBACK(fill_grid_expenditure),NULL);
	g_signal_connect (G_OBJECT(dateedit_filter_income_from), "changed", G_CALLBACK(fill_grid_income),NULL);
	g_signal_connect (G_OBJECT(dateedit_filter_income_to), " changed", G_CALLBACK(fill_grid_income),NULL);
	
	g_signal_connect (G_OBJECT(grid_account_short), "selection_changed", G_CALLBACK (on_grid_account_short_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_account_full), "selection_changed", G_CALLBACK (on_grid_account_full_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_expenditure), "selection_changed", G_CALLBACK (on_grid_expenditure_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_income), "selection_changed", G_CALLBACK (on_grid_income_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_debt), "selection_changed", G_CALLBACK (on_grid_debt_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_credit), "selection_changed", G_CALLBACK (on_grid_credit_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_plan_expenditure), "selection_changed", G_CALLBACK (on_grid_plan_expenditure_selection_changed),NULL);
	g_signal_connect (G_OBJECT(grid_plan_income), "selection_changed", G_CALLBACK (on_grid_plan_income_selection_changed),NULL);

	g_signal_connect (G_OBJECT(combo_filter_account_account), "selection-changed", G_CALLBACK (fill_grid_account_full),NULL);
	g_signal_connect (G_OBJECT(combo_filter_expend_account), "selection-changed", G_CALLBACK (fill_grid_expenditure),NULL);
	g_signal_connect (G_OBJECT(combo_filter_expend_category), "selection-changed", G_CALLBACK (on_combo_filter_expend_category_changed),NULL);
	g_signal_connect (G_OBJECT(combo_filter_expend_subcategory), "selection-changed", G_CALLBACK (fill_grid_expenditure),NULL);
	g_signal_connect (G_OBJECT(combo_filter_income_account), "selection-changed", G_CALLBACK (fill_grid_income),NULL);
	g_signal_connect (G_OBJECT(combo_filter_income_category), "selection-changed", G_CALLBACK (on_combo_filter_income_category_changed),NULL);
	g_signal_connect (G_OBJECT(combo_filter_income_subcategory), "selection-changed", G_CALLBACK (fill_grid_income),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_expend_account), "selection-changed", G_CALLBACK (fill_grid_plan_expenditure),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_expend_category), "selection-changed", G_CALLBACK (on_combo_filter_plan_expend_category_changed),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_expend_subcategory), "selection-changed", G_CALLBACK (fill_grid_plan_expenditure),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_income_account), "selection-changed", G_CALLBACK (fill_grid_plan_income),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_income_category), "selection-changed", G_CALLBACK (on_combo_filter_plan_income_category_changed),NULL);
	g_signal_connect (G_OBJECT(combo_filter_plan_income_subcategory), "selection-changed", G_CALLBACK (fill_grid_plan_income),NULL);

	g_signal_connect_swapped(G_OBJECT(grid_account_full), "row-activated", G_CALLBACK(on_button_account_datedetailed_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_account_short), "row-activated", G_CALLBACK(on_account_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_expenditure), "row-activated", G_CALLBACK(on_expenditure_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_income), "row-activated", G_CALLBACK(on_income_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_debt), "row-activated", G_CALLBACK(on_debt_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_credit), "row-activated", G_CALLBACK(on_credit_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_plan_expenditure), "row-activated", G_CALLBACK(on_plan_expenditure_button_edit_clicked), NULL);
	g_signal_connect_swapped(G_OBJECT(grid_plan_income), "row-activated", G_CALLBACK(on_plan_income_button_edit_clicked), NULL);
	

	gtk_dateentry_set_date(dateedit_filter_account_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_filter_expend_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_filter_income_from, get_current_date() - 30);	
	gtk_dateentry_set_date(dateedit_filter_plan_income_from, get_current_date() - 30);	
	
	//Set Combo models 
	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_account_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_expend_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_expend_category, out_category_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_income_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_income_category, in_category_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_plan_expend_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_plan_expend_category, out_category_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_plan_income_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_plan_income_category, in_category_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_account_account, TRUE);
	gdaui_combo_add_null(combo_filter_expend_account, TRUE);
	gdaui_combo_add_null(combo_filter_expend_category, TRUE);
	gdaui_combo_add_null(combo_filter_income_account, TRUE);
	gdaui_combo_add_null(combo_filter_income_category, TRUE);
	gdaui_combo_add_null(combo_filter_plan_expend_account, TRUE);
	gdaui_combo_add_null(combo_filter_plan_expend_category, TRUE);
	gdaui_combo_add_null(combo_filter_plan_income_account, TRUE);
	gdaui_combo_add_null(combo_filter_plan_income_category, TRUE);
	
	fill_grid_expenditure();
	fill_grid_income();
	fill_grid_account_short();
	fill_grid_account_full();
	fill_grid_debt();
	fill_grid_credit();
	fill_grid_plan_expenditure();
	fill_grid_plan_income();

	return window;
}




void fill_grid_expenditure(void)
{
	ExGrid *grid = grid_expenditure;
	
	GValue *dateto = NULL;
	GValue *datefrom = NULL;
	GValue *account = NULL;
	GValue *category = NULL;
	GValue *subcategory = NULL;

	GString *sql = g_string_new("SELECT a.id,a.date,strftime('%d.%m.%Y',date(a.date+1721425)),a.account_id, \
	                             b.category_id,b.subcategory_id,b.quantity,b.unit_id,a.amount*-1,a.currency_id,a.description \
	                             FROM operation a, expin b \
	                             WHERE a.id=b.id AND a.amount < 0");

	if (expend_filter == TRUE) 
	{
		g_string_append(sql," AND a.date>=##datefrom::gint");
		g_string_append(sql," AND a.date<=##dateto::gint");

		datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_expend_from));
		dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_expend_to));
		account     = ex_combo_get_current_row_value(combo_filter_expend_account, 0);
		category    = ex_combo_get_current_row_value(combo_filter_expend_category, 0);
		subcategory = ex_combo_get_current_row_value(combo_filter_expend_subcategory, 0);
			
		if (!gdaui_combo_is_null_selected(combo_filter_expend_account)) 
			g_string_append(sql," AND a.account_id=##account::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_expend_category)) 
			g_string_append(sql," AND b.category_id=##category::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_expend_subcategory)) 
			g_string_append(sql," AND b.subcategory_id=##subcategory::gint::NULL");
	}
	g_string_append(sql, " ORDER BY a.date DESC");
	
	GdaDataModel *db_model = db_exec_select_sql (sql->str,
	                                             "datefrom", datefrom,
	                                             "dateto", dateto,
	                                             "account", account,
	                                             "category", category,
	                                             "subcategory", subcategory,
	                                             NULL);
	

	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid),db_model);
	ex_grid_lookup_field (grid, EX_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field (grid, EX_CATEGORY_ID_COL, category_model, 0);
	ex_grid_lookup_field (grid, EX_SUBCATEGORY_ID_COL, subcategory_model, 0);
	ex_grid_lookup_field (grid, EX_UNIT_ID_COL, unit_model, 0);
	ex_grid_lookup_field (grid, EX_CURRENCY_ID_COL, currency_model, 0);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), EX_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), EX_TIME_T_COL, FALSE);

	ex_grid_column_set_format(grid, EX_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, EX_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, EX_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, EX_CATEGORY_ID_COL, _("Category"));
	ex_grid_column_set_title(grid, EX_SUBCATEGORY_ID_COL, _("Subcategory"));
	ex_grid_column_set_title(grid, EX_QUANTITY_COL, _("Qty."));
	ex_grid_column_set_title(grid, EX_UNIT_ID_COL, _("Unit"));
	ex_grid_column_set_title(grid, EX_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, EX_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, EX_DESCR_COL, _("Description"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_expenditure_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expenditure_del), FALSE);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);
	
	
	GtkCellRenderer *date_renderer = ex_grid_get_column_text_renderer (grid, EX_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);
	
	g_free(datefrom);
	g_free(dateto);
	g_object_unref(db_model);
	g_string_free(sql,TRUE);

}


void fill_grid_income(void)
{
	ExGrid *grid = grid_income;

	GValue *dateto = NULL;
	GValue *datefrom = NULL;
	GValue *account = NULL;
	GValue *category = NULL;
	GValue *subcategory = NULL;

	GString *sql = g_string_new("SELECT a.id,a.date,strftime('%d.%m.%Y',date(a.date+1721425)),a.account_id, \
	                             b.category_id,b.subcategory_id,b.quantity,b.unit_id,a.amount,a.currency_id,a.description \
	                             FROM operation a, expin b \
	                             WHERE a.id=b.id AND a.amount > 0");

	if (income_filter == TRUE) 
	{
		g_string_append(sql," AND a.date>=##datefrom::gint");
		g_string_append(sql," AND a.date<=##dateto::gint");

		datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_from));
		dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_to));
		account     = ex_combo_get_current_row_value(combo_filter_income_account,0);
		category    = ex_combo_get_current_row_value(combo_filter_income_category,0);
		subcategory = ex_combo_get_current_row_value(combo_filter_income_subcategory,0);
			
		if (!gdaui_combo_is_null_selected(combo_filter_income_account)) 
			g_string_append(sql," AND a.account_id=##account::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_category)) 
			g_string_append(sql," AND b.category_id=##category::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_subcategory)) 
			g_string_append(sql," AND b.subcategory_id=##subcategory::gint::NULL");
	}
	g_string_append(sql, " ORDER BY a.date DESC");
	
	GdaDataModel *db_model = db_exec_select_sql (sql->str,
	                                             "datefrom", datefrom,
	                                             "dateto", dateto,
	                                             "account", account,
	                                             "category", category,
	                                             "subcategory", subcategory,
	                                             NULL);
	
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);
	ex_grid_lookup_field (grid, EX_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field (grid, EX_CATEGORY_ID_COL, category_model, 0);
	ex_grid_lookup_field (grid, EX_SUBCATEGORY_ID_COL, subcategory_model, 0);
	ex_grid_lookup_field (grid, EX_UNIT_ID_COL, unit_model, 0);
	ex_grid_lookup_field (grid, EX_CURRENCY_ID_COL, currency_model, 0);
	
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), EX_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), EX_TIME_T_COL, FALSE);

	ex_grid_column_set_format(grid, EX_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, EX_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, EX_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, EX_CATEGORY_ID_COL, _("Category"));
	ex_grid_column_set_title(grid, EX_SUBCATEGORY_ID_COL, _("Subcategory"));
	ex_grid_column_set_title(grid, EX_QUANTITY_COL, _("Qty."));
	ex_grid_column_set_title(grid, EX_UNIT_ID_COL, _("Unit"));
	ex_grid_column_set_title(grid, EX_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, EX_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, EX_DESCR_COL, _("Description"));
	
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_del), FALSE);

	GtkCellRendererText *date_renderer = ex_grid_get_column_text_renderer (grid, EX_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);
	
	g_free(datefrom);
	g_free(dateto);
	g_object_unref(db_model);
	g_string_free(sql,TRUE);	

}

void fill_grid_account_short(void)
{
	ExGrid *grid = grid_account_short;

	GdaDataModel *db_model = db_exec_select_sql ("SELECT a.id,a.name,f.id,f.amount, \
	                                                  cast(sum(case when c.amount < 0 then c.amount else 0 end)*-1 as real),\
	                                                  cast(sum(case when c.amount > 0 then c.amount else 0 end) as real),\
	                                                  cast(coalesce(sum(c.amount),0)+coalesce(f.amount,0) as real),\
	                                                  b.id,coalesce(a.description,'') \
	                                              FROM account a, currency b \
                                                  LEFT JOIN operation c on c.account_id=a.id AND c.currency_id=b.id \
                                                  JOIN account_startup f on f.account_id=a.id AND f.currency_id=b.id AND f.active_currency=1\
                                                  GROUP BY a.id,b.id", NULL);
		
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid),db_model);
	ex_grid_lookup_field (grid, ACS_CURRENCY_ID_COL, currency_model, 0);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), ACS_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), ACS_STARTUP_ID_COL, FALSE);

	ex_grid_column_set_format(grid, ACS_STARTUP_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, ACS_EXPEND_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, ACS_INCOME_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, ACS_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, ACS_NAME_COL, _("Account"));
	ex_grid_column_set_title(grid, ACS_STARTUP_AMOUNT_COL, _("Initial balance"));
	ex_grid_column_set_title(grid, ACS_EXPEND_COL, _("Expenses"));
	ex_grid_column_set_title(grid, ACS_INCOME_COL, _("Income"));
	ex_grid_column_set_title(grid, ACS_AMOUNT_COL, _("Balance"));
	ex_grid_column_set_title(grid, ACS_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, ACS_DESCR_COL, _("Description"));
	
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_edit),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_del),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_transfer),FALSE);

	GtkCellRenderer *renderer = ex_grid_get_column_text_renderer (grid, ACS_NAME_COL);
	g_object_set(G_OBJECT(renderer), "weight", 1000, NULL);
	g_object_set(G_OBJECT(renderer), "weight-set", TRUE, NULL);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);
	
	g_object_unref(db_model);

}

void fill_grid_account_full(void)
{
	ExGrid *grid = grid_account_full;

	GValue *datefrom = NULL;
	GValue *dateto = NULL;
	GValue *account = NULL;
	
	GString *sql = g_string_new("SELECT c.date,strftime('%d.%m.%Y',date(c.date+1721425)), a.id,sum(case when c.amount < 0 then c.amount else 0 end)*-1, \
	                                  sum(case when c.amount > 0 then c.amount else 0 end),sum(c.amount),b.id \
	                                  FROM account a, currency b, operation c \
	                                  WHERE c.account_id=a.id AND c.currency_id=b.id");
	
	if (account_filter == TRUE) 
	{
		if (!gdaui_combo_is_null_selected(combo_filter_account_account)) 
		{
			g_string_append(sql," AND a.id=##id::gint::NULL");
			account  = ex_combo_get_current_row_value(combo_filter_account_account, 0);
		}

		datefrom = ex_value_new_int (gtk_dateentry_get_date(dateedit_filter_account_from));
		dateto   = ex_value_new_int (gtk_dateentry_get_date(dateedit_filter_account_to));
	
		g_string_append(sql," AND c.date>=##datefrom::gint");
		g_string_append(sql," AND c.date<=##dateto::gint");
	}
	g_string_append(sql," GROUP BY c.date,a.id,b.id ORDER BY c.date DESC");

	
	GdaDataModel *db_model = db_exec_select_sql(sql->str,
	                                            "datefrom", datefrom,
	                                            "dateto", dateto,  
	                                            "id", account,
	                                            NULL);
	
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);
	ex_grid_lookup_field (grid, ACF_ID_COL, account_model, 0);
	ex_grid_lookup_field (grid, ACF_CURRENCY_ID_COL, currency_model, 0);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_account_full), ACF_TIME_T_COL, FALSE);

	ex_grid_column_set_format(grid, ACF_AMOUNT_EXPEND_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, ACF_AMOUNT_INCOME_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, ACF_AMOUNT_DIF_COL, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, ACF_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, ACF_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, ACF_AMOUNT_EXPEND_COL, _("Expenses"));
	ex_grid_column_set_title(grid, ACF_AMOUNT_INCOME_COL, _("Income"));
	ex_grid_column_set_title(grid, ACF_AMOUNT_DIF_COL, _("Balance"));
	ex_grid_column_set_title(grid, ACF_CURRENCY_ID_COL, _("Currency"));

	
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_datedetailed), FALSE);

	GtkCellRendererText *date_renderer = ex_grid_get_column_text_renderer (grid, ACF_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);

	g_free(datefrom);
	g_free(dateto);
	g_object_unref(db_model);
	g_string_free(sql, TRUE);

}


void fill_grid_debt(void)
{

	ExGrid *grid = grid_debt;
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT a.id,a.date,strftime('%d.%m.%Y',date(a.date+1721425)),a.account_id,\
	                                                b.person_id,b.percent,b.period,b.is_closed, b.close_date, \
	                                                a.amount*-1,(SELECT sum(xx.amount) FROM operation xx, debtcredit_payment xz WHERE xx.id=xz.id AND xz.debtcredit_id=a.id), \
	                                                a.currency_id,b.remind,b.remind_date,a.description \
	                                              FROM operation a, debtcredit b \
	                                              WHERE a.id=b.id AND a.amount<0\
	                                              ORDER BY a.date DESC", NULL);

	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);
	ex_grid_lookup_field(grid, DEB_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field(grid, DEB_PERSON_ID_COL, person_model, 0);
	ex_grid_lookup_field(grid, DEB_CURRENCY_ID_COL, currency_model, 0);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_TIME_T_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_REMIND_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_REMIND_DATE_COL, FALSE);

	ex_grid_column_set_format(grid, DEB_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, DEB_PAY_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, DEB_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, DEB_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, DEB_PERSON_ID_COL, _("Debtor"));
	ex_grid_column_set_title(grid, DEB_PERCENT_COL, _("Percent"));
	ex_grid_column_set_title(grid, DEB_PERIOD_COL, _("Period"));
	ex_grid_column_set_title(grid, DEB_IS_CLOSED_COL, _("Pay off"));
	ex_grid_column_set_title(grid, DEB_CLOSED_DATE_COL, _("Date pay off"));
	ex_grid_column_set_title(grid, DEB_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, DEB_PAY_AMOUNT_COL, _("Paid"));
	ex_grid_column_set_title(grid, DEB_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, DEB_DESCR_COL, _("Description"));

	
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_del), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_payments), FALSE);

	GtkCellRendererText *date_renderer = ex_grid_get_column_text_renderer (grid, DEB_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);


	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);

	g_object_unref(db_model);

}

void fill_grid_credit(void)
{
	
	ExGrid *grid = grid_credit;

	GdaDataModel *db_model = db_exec_select_sql ("SELECT a.id,a.date,strftime('%d.%m.%Y',date(a.date+1721425)),a.account_id,\
	                                                b.person_id,b.percent,b.period,b.is_closed, b.close_date, \
	                                                a.amount,(SELECT sum(xx.amount) FROM operation xx, debtcredit_payment xz WHERE xx.id=xz.id AND xz.debtcredit_id=a.id), \
	                                                a.currency_id,b.remind,b.remind_date,a.description \
	                                              FROM operation a, debtcredit b \
	                                              WHERE a.id=b.id AND a.amount>0 \
	                                              ORDER BY a.date DESC",
	                                             NULL);
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid),db_model);
	ex_grid_lookup_field(grid, DEB_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field(grid, DEB_PERSON_ID_COL, person_model, 0);
	ex_grid_lookup_field(grid, DEB_CURRENCY_ID_COL, currency_model, 0);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_TIME_T_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_REMIND_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), DEB_REMIND_DATE_COL, FALSE);
	
	ex_grid_column_set_format(grid, DEB_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	ex_grid_column_set_format(grid, DEB_PAY_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, DEB_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, DEB_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, DEB_PERSON_ID_COL, _("Creditor"));
	ex_grid_column_set_title(grid, DEB_PERCENT_COL, _("Percent"));
	ex_grid_column_set_title(grid, DEB_PERIOD_COL, _("Period"));
	ex_grid_column_set_title(grid, DEB_IS_CLOSED_COL, _("Pay off"));
	ex_grid_column_set_title(grid, DEB_CLOSED_DATE_COL, _("Date pay off"));
	ex_grid_column_set_title(grid, DEB_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, DEB_PAY_AMOUNT_COL, _("Paid"));
	ex_grid_column_set_title(grid, DEB_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, DEB_DESCR_COL, _("Description"));
	
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_del), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_payments), FALSE);


	GtkCellRenderer *date_renderer = ex_grid_get_column_text_renderer (grid, DEB_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);
	
	g_object_unref(db_model);
}

void fill_grid_plan_expenditure(void)
{
	GValue *dateto = NULL;
	GValue *datefrom = NULL;
	GValue *account = NULL;
	GValue *category = NULL;
	GValue *subcategory = NULL;

	ExGrid *grid = grid_plan_expenditure;
	
	GString *sql = g_string_new("SELECT id, date, strftime('%d.%m.%Y',date(date+1721425)),account_id, \
	                             category_id, subcategory_id, quantity, unit_id, amount*-1, currency_id, periodicity, periodicity_days, description \
	                             FROM plan \
	                             WHERE amount < 0");

	if (plan_expend_filter == TRUE) 
	{
		g_string_append(sql," AND date>=##datefrom::gint");
		g_string_append(sql," AND date<=##dateto::gint");

		datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_from));
		dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_to));
		account     = ex_combo_get_current_row_value(combo_filter_income_account,0);
		category    = ex_combo_get_current_row_value(combo_filter_income_category,0);
		subcategory = ex_combo_get_current_row_value(combo_filter_income_subcategory,0);
			
		if (!gdaui_combo_is_null_selected(combo_filter_income_account)) 
			g_string_append(sql," AND account_id=##account::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_category)) 
			g_string_append(sql," AND category_id=##category::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_subcategory)) 
			g_string_append(sql," AND subcategory_id=##subcategory::gint::NULL");
	}
	g_string_append(sql, " ORDER BY date DESC");

	
	GdaDataModel *db_model = db_exec_select_sql (sql->str,
	                                             "datefrom", datefrom,
	                                             "dateto", dateto,
	                                             "account", account,
	                                             "category", category,
	                                             "subcategory", subcategory,
	                                             NULL);
	
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);
	ex_grid_lookup_field (grid, PLAN_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field (grid, PLAN_CATEGORY_ID_COL, category_model, 0);
	ex_grid_lookup_field (grid, PLAN_SUBCATEGORY_ID_COL, subcategory_model, 0);
	ex_grid_lookup_field (grid, PLAN_UNIT_ID_COL, unit_model, 0);
	ex_grid_lookup_field (grid, PLAN_CURRENCY_ID_COL, currency_model, 0);
	ex_grid_lookup_field (grid, PLAN_PERIODICITY_COL, periodicity_model, 0);
	
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_TIME_T_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_PERIODICITY_DAYS_COL, FALSE);

	ex_grid_column_set_format(grid, PLAN_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, PLAN_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, PLAN_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, PLAN_CATEGORY_ID_COL, _("Category"));
	ex_grid_column_set_title(grid, PLAN_SUBCATEGORY_ID_COL, _("Subcategory"));
	ex_grid_column_set_title(grid, PLAN_QUANTITY_COL, _("Qty."));
	ex_grid_column_set_title(grid, PLAN_UNIT_ID_COL, _("Unit"));
	ex_grid_column_set_title(grid, PLAN_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, PLAN_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, PLAN_PERIODICITY_COL, _("Periodicity"));
	ex_grid_column_set_title(grid, PLAN_DESCR_COL, _("Description"));
	
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_del), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_exec), FALSE);

	GtkCellRendererText *date_renderer = ex_grid_get_column_text_renderer (grid, EX_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);

	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);

	g_free(datefrom);
	g_free(dateto);
	g_object_unref(db_model);
	g_object_unref(periodicity_model);
	g_string_free(sql,TRUE);	

}

void fill_grid_plan_income(void)
{
	GValue *dateto = NULL;
	GValue *datefrom = NULL;
	GValue *account = NULL;
	GValue *category = NULL;
	GValue *subcategory = NULL;

	ExGrid *grid = grid_plan_income;
	
	GString *sql = g_string_new("SELECT id, date, strftime('%d.%m.%Y',date(date+1721425)),account_id, \
	                             category_id, subcategory_id, quantity, unit_id, amount, currency_id, periodicity, periodicity_days, description \
	                             FROM plan \
	                             WHERE amount > 0");

	if (plan_income_filter == TRUE) 
	{
		g_string_append(sql," AND date>=##datefrom::gint");
		g_string_append(sql," AND date<=##dateto::gint");

		datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_from));
		dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_filter_income_to));
		account     = ex_combo_get_current_row_value(combo_filter_income_account,0);
		category    = ex_combo_get_current_row_value(combo_filter_income_category,0);
		subcategory = ex_combo_get_current_row_value(combo_filter_income_subcategory,0);
			
		if (!gdaui_combo_is_null_selected(combo_filter_income_account)) 
			g_string_append(sql," AND account_id=##account::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_category)) 
			g_string_append(sql," AND category_id=##category::gint::NULL");
		if (!gdaui_combo_is_null_selected(combo_filter_income_subcategory)) 
			g_string_append(sql," AND subcategory_id=##subcategory::gint::NULL");
	}
	g_string_append(sql, " ORDER BY date DESC");
	
	GdaDataModel *db_model = db_exec_select_sql (sql->str,
	                                             "datefrom", datefrom,
	                                             "dateto", dateto,
	                                             "account", account,
	                                             "category", category,
	                                             "subcategory", subcategory,
	                                             NULL);
	
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, PLAN_ACCOUNT_ID_COL, account_model, 0);
	ex_grid_lookup_field (grid, PLAN_CATEGORY_ID_COL, category_model, 0);
	ex_grid_lookup_field (grid, PLAN_SUBCATEGORY_ID_COL, subcategory_model, 0);
	ex_grid_lookup_field (grid, PLAN_UNIT_ID_COL, unit_model, 0);
	ex_grid_lookup_field (grid, PLAN_CURRENCY_ID_COL, currency_model, 0);
	ex_grid_lookup_field (grid, PLAN_PERIODICITY_COL, periodicity_model, 0);
	
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_ID_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_TIME_T_COL, FALSE);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid), PLAN_PERIODICITY_DAYS_COL, FALSE);
	
	ex_grid_column_set_format(grid, PLAN_AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, PLAN_DATE_COL, _("Date"));
	ex_grid_column_set_title(grid, PLAN_ACCOUNT_ID_COL, _("Account"));
	ex_grid_column_set_title(grid, PLAN_CATEGORY_ID_COL, _("Category"));
	ex_grid_column_set_title(grid, PLAN_SUBCATEGORY_ID_COL, _("Subcategory"));
	ex_grid_column_set_title(grid, PLAN_QUANTITY_COL, _("Qty."));
	ex_grid_column_set_title(grid, PLAN_UNIT_ID_COL, _("Unit"));
	ex_grid_column_set_title(grid, PLAN_AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, PLAN_CURRENCY_ID_COL, _("Currency"));
	ex_grid_column_set_title(grid, PLAN_PERIODICITY_COL, _("Periodicity"));
	ex_grid_column_set_title(grid, PLAN_DESCR_COL, _("Description"));
	
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_del), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_exec), FALSE);

	GtkCellRendererText *date_renderer = ex_grid_get_column_text_renderer (grid, EX_DATE_COL);
	g_object_set(G_OBJECT(date_renderer), "weight", 10000, NULL);
	g_object_set(G_OBJECT(date_renderer), "weight-set", TRUE, NULL);


	ex_grid_set_columns_resizable(grid, TRUE);
	ex_grid_set_columns_reordable(grid, TRUE);

	g_free(datefrom);
	g_free(dateto);
	g_object_unref(db_model);
	g_object_unref(periodicity_model);
	g_string_free(sql,TRUE);	

}


void refresh_account_combos()
{
	gint viewcols[1] = {1};
	
	gdaui_combo_set_model(combo_filter_account_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_expend_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_filter_income_account, account_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_account_account, TRUE);
	gdaui_combo_add_null(combo_filter_expend_account, TRUE);
	gdaui_combo_add_null(combo_filter_income_account, TRUE);
}

void refresh_in_category_combos()
{
	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_income_category, in_category_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_income_category, TRUE);
}

void refresh_out_category_combos()
{
	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_expend_category, out_category_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_expend_category, TRUE);

}

void on_tbutton_category_clicked (GtkToolButton *toolbutton, gpointer user_data) 
{
	show_category_window();

}

void on_tbutton_currency_clicked (GtkToolButton *toolbutton, gpointer user_data) 
{
	show_currency_window();
}

void on_tbutton_report_clicked (GtkToolButton *toolbutton, gpointer user_data) 
{
	show_report_window();

}

void on_expenditure_button_add_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_INSERT_EXPENDITURE);
}


void on_expenditure_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_EDIT_EXPENDITURE);
}


void on_expenditure_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_expenditure, EX_ID_COL);	

	db_exec_sql("DELETE FROM operation WHERE id=##id::gint", "id", id, NULL);

	fill_grid_expenditure();
	fill_grid_account_short();
	fill_grid_account_full();
}

void on_income_button_add_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_INSERT_INCOME);
}


void on_income_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_EDIT_INCOME);
}


void on_income_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_income, EX_ID_COL);	

	db_exec_sql("DELETE FROM operation WHERE id=##id::gint", "id", id, NULL);

	fill_grid_income();
	fill_grid_account_short();
	fill_grid_account_full();
}

void on_account_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_editaccount_window (OPER_INSERT);

	if (response == GTK_RESPONSE_OK)
	{
		refresh_account_model();
		refresh_account_combos();
		fill_grid_account_short();
	}
}


void on_account_button_edit_clicked (GtkButton *button, gpointer user_data)
{

	gint response = show_editaccount_window (OPER_UPDATE);

	if (response == GTK_RESPONSE_OK)
	{

		refresh_account_model();
		refresh_account_combos();
		fill_grid_account_short();
	}
	
}

void on_account_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you remove the account, all operations associated with this account will be removed. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;

	GValue *id = ex_grid_get_selected_row_value(grid_account_short,ACS_ID_COL);	

	gchar *sql = "DELETE FROM account WHERE id=##id::gint";

	db_exec_sql(sql, "id", id, NULL);
	
	refresh_account_model();
	refresh_account_combos();
	fill_grid_account_short();
	fill_grid_account_full();
	fill_grid_expenditure();
	fill_grid_income();
	fill_grid_credit();
	fill_grid_debt();
	fill_grid_plan_expenditure();
	fill_grid_plan_income();
}

void on_account_button_edittransfer_clicked (GtkButton *button, gpointer user_data)
{
	show_edittransfer_window();
}

void on_debt_button_add_clicked (GtkButton *button, gpointer user_data)
{
	show_editdebt_window (INSERTDEBT_DEBT);
}

void on_debt_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editdebt_window (EDITDEBT_DEBT);
}


void on_debt_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;

	GValue *id = ex_grid_get_selected_row_value(grid_debt,DEB_ID_COL);	

	gchar *sql = "DELETE FROM operation WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);

	fill_grid_debt();
	fill_grid_account_short();
	fill_grid_account_full();	
}

void on_debt_button_pay_clicked (GtkButton *button, gpointer user_data)
{
	show_debtcredit_payment_window(OPER_DEBT_PAYMENT);
}

void on_credit_button_pay_clicked (GtkButton *button, gpointer user_data)
{
	show_debtcredit_payment_window(OPER_CREDIT_PAYMENT);
}

void on_credit_button_add_clicked (GtkButton *button, gpointer user_data)
{

	show_editdebt_window (INSERTDEBT_CREDIT);

}

void on_credit_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editdebt_window (EDITDEBT_CREDIT);
}


void on_credit_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;

	GValue *id = ex_grid_get_selected_row_value(grid_credit, DEB_ID_COL);	

	gchar *sql = "DELETE FROM operation WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);


	fill_grid_credit();
	fill_grid_account_short();
	fill_grid_account_full();
}

void on_plan_expenditure_button_add_clicked (GtkButton *button, gpointer user_data)
{
	show_editplan_window (OPER_INSERT_PLAN_EXPENDITURE);
}


void on_plan_expenditure_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editplan_window (OPER_EDIT_PLAN_EXPENDITURE);
}


void on_plan_expenditure_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_plan_expenditure, PLAN_ID_COL);	

	db_exec_sql("DELETE FROM plan WHERE id=##id::gint", "id", id, NULL);

	fill_grid_plan_expenditure();
	fill_grid_account_short();
	fill_grid_account_full();
}

void on_plan_expenditure_button_exec_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_PLAN_EXPENDITURE);
}

void on_plan_income_button_add_clicked (GtkButton *button, gpointer user_data)
{
	show_editplan_window (OPER_INSERT_PLAN_INCOME);
}


void on_plan_income_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	show_editplan_window (OPER_EDIT_PLAN_INCOME);
}


void on_plan_income_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_plan_income, PLAN_ID_COL);	

	db_exec_sql("DELETE FROM plan WHERE id=##id::gint", "id", id, NULL);

	fill_grid_plan_income();
	fill_grid_account_short();
	fill_grid_account_full();
}

void on_plan_income_button_exec_clicked (GtkButton *button, gpointer user_data)
{
	show_editexpin_window (OPER_PLAN_INCOME);
}


void on_button_account_datedetailed_clicked(GtkButton *button, gpointer user_data)
{
	show_account_datedetailed_window();
}

void on_expander_filter_account_activate (GtkExpander *expander, gpointer user_data)
{
	account_filter=!gtk_expander_get_expanded(expander);
	fill_grid_account_full();
}

void on_combo_filter_expend_category_changed (GtkComboBox *widget, gpointer user_data)
{
	GValue *val = ex_combo_get_current_row_value(combo_filter_expend_category,0);


	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", val, NULL);
	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_expend_subcategory, db_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_expend_subcategory, TRUE);
	
	fill_grid_expenditure();

	g_object_unref (db_model);
	
}

void on_combo_filter_income_category_changed (GtkComboBox *widget, gpointer user_data)
{
	GValue *val = ex_combo_get_current_row_value(combo_filter_income_category,0);

	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", val, NULL);

	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_income_subcategory, db_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_income_subcategory, TRUE);
	
	fill_grid_income();

	g_object_unref (db_model);
	
}

void on_combo_filter_plan_income_category_changed (GtkComboBox *widget, gpointer user_data)
{
	GValue *val = ex_combo_get_current_row_value(combo_filter_plan_income_category,0);


	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", val, NULL);
	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_plan_income_subcategory, db_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_plan_income_subcategory, TRUE);
	
	fill_grid_plan_income();

	g_object_unref (db_model);
	
}

void on_combo_filter_plan_expend_category_changed (GtkComboBox *widget, gpointer user_data)
{
	GValue *val = ex_combo_get_current_row_value(combo_filter_plan_expend_category,0);


	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", val, NULL);
	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_filter_plan_expend_subcategory, db_model, 1, viewcols);
	gdaui_combo_add_null(combo_filter_plan_expend_subcategory, TRUE);
	
	fill_grid_plan_expenditure();

	g_object_unref (db_model);
	
}

void on_expander_filter_expend_activate (GtkExpander *expander, gpointer user_data)
{
	expend_filter=!gtk_expander_get_expanded(expander);
	fill_grid_expenditure();
}

void on_expander_filter_income_activate (GtkExpander *expander, gpointer user_data)
{
	income_filter = !gtk_expander_get_expanded(expander);
	fill_grid_income();
}

void on_expander_filter_plan_expend_activate (GtkExpander *expander, gpointer user_data)
{
	plan_income_filter = !gtk_expander_get_expanded(expander);
	fill_grid_plan_expenditure();
}

void on_expander_filter_plan_income_activate (GtkExpander *expander, gpointer user_data)
{
	income_filter = !gtk_expander_get_expanded(expander);
	fill_grid_plan_income();
}

void on_grid_account_short_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_transfer), TRUE);
}

void on_grid_expenditure_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_expenditure_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expenditure_del), TRUE);
}

void on_grid_income_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_del), TRUE);
}

void on_grid_debt_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debt_payments), TRUE);
}

void on_grid_credit_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_credit_payments), TRUE);
}

void on_grid_account_full_selection_changed (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_account_datedetailed), TRUE);
}

void on_grid_plan_expenditure_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_expenditure_exec), TRUE);
}

void on_grid_plan_income_selection_changed(GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_del), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_plan_income_exec), TRUE);
}

void on_button_filter_account_clear_clicked (GtkButton *button, gpointer user_data)
{
	gtk_dateentry_set_date(dateedit_filter_account_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_filter_account_to, NULL);
	gtk_combo_box_set_active(combo_filter_account_account, -1);
}

void on_button_filter_expend_clear_clicked (GtkButton *button, gpointer user_data)
{
	gtk_dateentry_set_date(dateedit_filter_expend_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_filter_expend_to, get_current_date());
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_expend_account), -1);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_expend_subcategory), -1);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_expend_category), -1);
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(combo_filter_expend_subcategory), NULL);
}

void on_button_filter_income_clear_clicked (GtkButton *button, gpointer user_data)
{
	gtk_dateentry_set_date(dateedit_filter_income_from, get_current_date () - 30);
	gtk_dateentry_set_date(dateedit_filter_income_to, get_current_date ());
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_income_account), -1);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_income_subcategory), -1);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_filter_income_category), -1);
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(combo_filter_income_subcategory), NULL);

}


void on_imagemenuitem_about_activate  (GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_show_about_dialog(NULL,"program-name", "Dinero",
	                      		"version", VERSION,
	                      		"copyright", "Copyright (c) Dmitry Kosenkov 2011",
	                      		"website", "http://github.com/Junker/dinero"
	                      );
}	





void on_tbutton_print_clicked (GtkToolButton *toolbutton, gpointer user_data) 
{
/*	GError *error = NULL;

	GdaReportDocument *doc;
	GdaReportEngine *eng;

	doc = gda_report_docbook_document_new (NULL);
	g_object_get (G_OBJECT (doc), "engine", &eng, NULL);

	gda_report_document_set_template (doc, "rep-spec.xml");
	g_object_set (G_OBJECT (doc), "fo-stylesheet", "/usr/share/xml/docbook/xsl-stylesheets-1.76.1/fo/docbook.xsl", NULL);
	
	GdaStatement *stmt = gda_sql_parser_parse_string (parser, "SELECT name FROM account", NULL, &error);


	gda_report_engine_declare_object (eng, G_OBJECT (connection), "main_cnc");
	gda_report_engine_declare_object (eng, G_OBJECT (stmt), "account");

	gchar *outfile = "/tmp/customers-report-rml.pdf";
	gda_report_document_run_as_pdf (doc, outfile, &error);

	if (error) g_debug(error->message);
*/
/*	GdaReportEngine *eng = gda_report_engine_new_from_file ("rep-spec.xml");
	GdaStatement *stmt = gda_sql_parser_parse_string (parser, "SELECT name FROM account", NULL, &error);
	gda_report_engine_declare_object (eng, G_OBJECT (connection), "main_cnc");
	gda_report_engine_declare_object (eng, G_OBJECT (stmt), "account");

	
	xmlDocPtr doc = gda_report_engine_run_as_doc (eng, &error);
	if (!doc) {
		g_debug(error->message);
	}
	else {
		xmlSaveFile ("/tmp/report.xml", doc);
		xmlFreeDoc (doc);
	}

	GdaReportDocument *rd = gda_report_docbook_document_new(eng);
	gchar *st;
	g_object_set(rd,"html-stylesheet" , "/usr/share/xml/docbook/xsl-stylesheets-1.76.1/xhtml/table.xsl", NULL);
	g_debug("sheet:%s",st);
	if(!gda_report_document_run_as_html(rd,"/tmp/report.pdf", &error))
		g_debug(error->message);

*/	
}

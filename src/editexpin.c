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
#include <gtk/gtk.h>
#include <libgda-ui/libgda-ui.h>

#include "common.h"
#include "editexpin.h"
#include "db.h"
#include "actions.h"
#include "mainform.h"

#include "sutil.h"

#include "gtkdateentry.h"

static GtkWidget *window;
static GdauiCombo *combo_category,
                  *combo_subcategory,
                  *combo_account,
                  *combo_unit,
                  *combo_currency;

static GtkDateEntry *dateedit;

static GtkSpinButton *spinbutton_quantity,
                     *spinbutton_amount;

static GtkCheckButton *checkbutton_multi;
static GtkTextView *textview_description;

static GtkWidget *button_subcategory_add;

static GValue *id;

static void fill_combo_category(OperType oper);
static void fill_combo_subcategory(OperType oper);

static void on_button_ok_clicked(GtkButton *button,gint oper);
static void on_button_category_add_clicked(GtkButton *button, OperType oper);
static void on_button_subcategory_add_clicked(GtkButton *button, OperType oper);
static void set_fields_values(OperType oper);
static void on_combo_category_changed (GtkComboBox *widget, OperType oper);

void show_editexpin_window (OperType oper)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editexpin.glade");
	gtk_builder_connect_signals (gbuilder, NULL);

	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *eventbox_combo_category    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_category"));
	GtkContainer *eventbox_combo_subcategory = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_subcategory"));
	GtkContainer *eventbox_combo_account     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_account"));
	GtkContainer *eventbox_combo_unit        = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_unit"));
	GtkContainer *eventbox_combo_currency    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_currency"));

	GtkContainer *eventbox_dateedit          = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit"));

	spinbutton_quantity  = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_quantity"));
	checkbutton_multi    = GTK_CHECK_BUTTON(gtk_builder_get_object (gbuilder, "checkbutton_multi"));
	spinbutton_amount    = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_amount"));
	textview_description = GTK_TEXT_VIEW(gtk_builder_get_object (gbuilder, "textview_description"));
	
	GtkWidget *button_ok = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_ok"));
	GtkWidget *button_cancel = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_cancel"));

	GtkWidget *button_category_add = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_category_add"));

	button_subcategory_add = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_subcategory_add"));

	//Create Combo controls
	combo_category    = create_combo(eventbox_combo_category);
	combo_subcategory = create_combo(eventbox_combo_subcategory);
	combo_account     = create_combo(eventbox_combo_account);
	combo_unit        = create_combo(eventbox_combo_unit);
	combo_currency    = create_combo(eventbox_combo_currency);

	dateedit = create_dateentry (eventbox_dateedit);

	id = NULL;

	g_signal_connect (G_OBJECT(button_ok), "clicked", G_CALLBACK (on_button_ok_clicked), oper);
	g_signal_connect (G_OBJECT(button_category_add), "clicked", G_CALLBACK (on_button_category_add_clicked), oper);
	g_signal_connect (G_OBJECT(button_subcategory_add), "clicked", G_CALLBACK (on_button_subcategory_add_clicked), oper);
	g_signal_connect (G_OBJECT(combo_category), "changed", G_CALLBACK (on_combo_category_changed), oper);

	
	fill_combo_category(oper);

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_currency, currency_model, 1, viewcols);
	gdaui_combo_set_model(combo_unit, unit_model, 1, viewcols);

	
	set_fields_values(oper);
		

	gtk_widget_show(window);

}	



void fill_combo_category(OperType oper)
{
	GdaDataModel *db_model;
	if (oper == OPER_EDIT_EXPENDITURE || oper == OPER_INSERT_EXPENDITURE || oper == OPER_PLAN_EXPENDITURE) 
		db_model = out_category_model;
	else 
		db_model = in_category_model;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_category, db_model, 1, viewcols);

	gtk_widget_set_sensitive (GTK_WIDGET(button_subcategory_add), FALSE);
	
}

void fill_combo_subcategory(OperType oper)
{
	GValue *val = ex_combo_get_current_row_value(combo_category,0);

	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint ORDER BY name";
	
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", val, NULL);

	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_subcategory, db_model, 1, viewcols);
	
}




void set_fields_values(OperType oper)
{
	if (oper == OPER_EDIT_EXPENDITURE || oper == OPER_EDIT_INCOME)
	{
		guint date;
		GValue *category;
		guint subcategory;
		guint account;
		guint quantity;
		guint unit;
		gdouble amount;
		guint currency;
		gchar *description;

		ExGrid *grid = oper == OPER_EDIT_EXPENDITURE ? grid_expenditure : grid_income;

		id          = ex_grid_get_selected_row_value (grid, EX_ID_COL);
		date        = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_TIME_T_COL));
		account     = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_ACCOUNT_ID_COL));
		category    = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_CATEGORY_ID_COL));
		subcategory = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_SUBCATEGORY_ID_COL));
		quantity    = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_QUANTITY_COL));
		unit        = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_UNIT_ID_COL));
		amount      = g_value_get_double(ex_grid_get_selected_row_value (grid, EX_AMOUNT_COL));
		currency    = g_value_get_int(ex_grid_get_selected_row_value (grid, EX_CURRENCY_ID_COL));
		description = g_value_get_string(ex_grid_get_selected_row_value (grid, EX_DESCR_COL));
	
		gtk_date_entry_set_date(dateedit, date);
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_category), ex_combo_get_row_by_id(combo_category, category));
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_subcategory), ex_combo_get_row_by_id(combo_subcategory, subcategory));
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_account), ex_combo_get_row_by_id(combo_account,account));
		gtk_spin_button_set_value(spinbutton_quantity,quantity);	
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_unit), ex_combo_get_row_by_id(combo_unit,unit));
		gtk_spin_button_set_value(spinbutton_amount, amount);	
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_currency), ex_combo_get_row_by_id(combo_currency,currency));	
		ex_textview_set_text(textview_description, description);
	}
	else if (oper == OPER_PLAN_EXPENDITURE || oper == OPER_PLAN_INCOME)
	{
		GValue *category;
		guint subcategory;
		guint account;
		guint quantity;
		guint unit;
		gdouble amount;
		guint currency;
		gchar *description;

		ExGrid *grid = oper == OPER_PLAN_EXPENDITURE ? grid_plan_expenditure : grid_plan_income;

		account     = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_ACCOUNT_ID_COL));
		category    = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_CATEGORY_ID_COL));
		subcategory = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_SUBCATEGORY_ID_COL));
		quantity    = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_QUANTITY_COL));
		unit        = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_UNIT_ID_COL));
		amount      = g_value_get_double(ex_grid_get_selected_row_value (grid, PLAN_AMOUNT_COL));
		currency    = g_value_get_int(ex_grid_get_selected_row_value (grid, PLAN_CURRENCY_ID_COL));
		description = g_value_get_string(ex_grid_get_selected_row_value (grid, PLAN_DESCR_COL));
	
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_category), ex_combo_get_row_by_id(combo_category, category));
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_subcategory), ex_combo_get_row_by_id(combo_subcategory, subcategory));
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_account), ex_combo_get_row_by_id(combo_account,account));
		gtk_spin_button_set_value(spinbutton_quantity,quantity);	
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_unit), ex_combo_get_row_by_id(combo_unit,unit));
		gtk_spin_button_set_value(spinbutton_amount, amount);	
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_currency), ex_combo_get_row_by_id(combo_currency,currency));	
		ex_textview_set_text(textview_description, description);

	}
	else
	{
		GValue *currency = db_get_value ("SELECT id FROM currency WHERE is_main=1");
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_currency), 
		                         ex_combo_get_row_by_id(combo_currency,g_value_get_int(currency))
		                        );	
		
	}
}


void on_combo_category_changed (GtkComboBox *widget, OperType oper)
{
	fill_combo_subcategory(oper);

	gtk_widget_set_sensitive (GTK_WIDGET(button_subcategory_add), TRUE);
	
}

void on_button_category_add_clicked(GtkButton *button, OperType oper)
{
	gchar *name_str = show_input_dialog(_("Enter category name:"),NULL);
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	GValue *type = NULL;
	
	if (oper == OPER_EDIT_EXPENDITURE || oper == OPER_INSERT_EXPENDITURE || oper == OPER_PLAN_EXPENDITURE)
		type = ex_value_new_int (1);
	else 
		type = ex_value_new_int (2);
	
	gchar *sql = "INSERT INTO category(name,type) VALUES(##name::string, ##type::gint)";
	db_exec_sql(sql, 
	            "name", name,
	            "type", type,
	            NULL);

	refresh_category_model();
	refresh_out_category_model();
	refresh_in_category_model();

	fill_combo_category(oper);

	g_free(name);
	g_free(name_str);
}

void on_button_subcategory_add_clicked(GtkButton *button, OperType oper)
{
	gchar *name_str = show_input_dialog(_("Enter subcategory name:"),NULL);
	if (strlen(name_str) == 0) return;

	GValue *name     = ex_value_new_string(name_str);
	GValue *category =  ex_combo_get_current_row_value(combo_category, 0);

	gchar *sql = "INSERT INTO subcategory(name,category_id) VALUES(##name::string, ##category::gint)";
	db_exec_sql(sql, 
	            "name", name,
	            "category", category,
	            NULL);

	refresh_subcategory_model();

	fill_combo_subcategory(oper);

	g_free(name);
	g_free(name_str);
}

void on_button_unit_add_clicked(GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter unit name:"),NULL);
	if (strlen(name_str) == 0) return;

	GValue *name     = ex_value_new_string(name_str);
	GValue *category =  ex_combo_get_current_row_value(combo_category, 0);

	gchar *sql = "INSERT INTO unit(name) VALUES(##name::string)";
	db_exec_sql(sql, 
	            "name", name,
	            NULL);

	refresh_unit_model();

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_unit, unit_model, 1, viewcols);

	g_free(name);
	g_free(name_str);
}

void on_button_ok_clicked(GtkButton *button, OperType oper)
{

	if ((gtk_combo_box_get_active(GTK_COMBO_BOX(combo_account))==-1)  || 
	    (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_category))==-1) ||
	    (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_currency))==-1)
	   )
		
	{
		show_error_dialog(_("Warning!"), _("Warning!"), _("Some fields were not filled")); 
		return;
	}

	GValue *category    = ex_combo_get_current_row_value(combo_category, 0);
	GValue *subcategory = ex_combo_get_current_row_value(combo_subcategory, 0);
	GValue *account     = ex_combo_get_current_row_value(combo_account, 0);
	GValue *currency    = ex_combo_get_current_row_value(combo_currency, 0);
	GValue *unit        = ex_combo_get_current_row_value(combo_unit, 0);
	GValue *date        = ex_value_new_int((gint) gtk_date_entry_get_date(dateedit));
	GValue *quantity    = ex_value_new_int(gtk_spin_button_get_value_as_int(spinbutton_quantity));
	gboolean multi      = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_multi));
	GValue *description = ex_value_new_string(ex_textview_get_text(textview_description));

	gdouble amount;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_multi))) 
		amount = gtk_spin_button_get_value(spinbutton_amount) * g_value_get_int(quantity) > 0 ? g_value_get_int(quantity) : 1;
	else 
		amount = gtk_spin_button_get_value(spinbutton_amount);


	
	if (oper == OPER_INSERT_EXPENDITURE || oper == OPER_INSERT_INCOME || oper == OPER_PLAN_EXPENDITURE || oper == OPER_PLAN_INCOME)
	{
		amount = (oper == OPER_INSERT_EXPENDITURE || oper == OPER_PLAN_EXPENDITURE) ? amount * (gdouble)-1 : amount;

		gchar *sql = " INSERT INTO operation(date,account_id,amount,currency_id,description) \
		               VALUES (##date::gint, ##account::gint, ##amount::gdouble, ##currency::gint, ##descr::string)";
	
		GdaSet *row = db_exec_sql(sql,
		                  "date", date,
		                  "account", account,
		                  "amount", ex_value_new_double(amount),
		                  "currency", currency,
		                  "descr", description,
		                  NULL
		                 );


		GValue *id = gda_set_get_holder_value(row, "+0");
		
		gchar *sql2 = "INSERT INTO expin(id,category_id,subcategory_id,quantity,unit_id) \
		               VALUES (##id::gint, ##category::gint::NULL, ##subcategory::gint::NULL, ##quantity::gint::NULL, ##unit::gint::NULL)";

		db_exec_sql(sql2,
		            "category", category,
		            "subcategory", subcategory,
		            "quantity", quantity,
		            "unit", unit,
		            "id", id,
		            NULL
		           );


	}


	if (oper == OPER_EDIT_EXPENDITURE || oper ==  OPER_EDIT_INCOME)
	{
		amount = oper == OPER_EDIT_EXPENDITURE ? amount * -1 : amount;

		gchar *sql = "UPDATE operation SET date=##date::gint, \
		                                 account_id=##account::gint, \
		                                 amount=##amount::gdouble, \
                                         currency_id=##currency::gint, \
                                         description=##descr::string \
		                       WHERE id=##id::gint";    

		db_exec_sql(sql,
		            "date", date,
		            "account", account,
		            "amount", ex_value_new_double(amount),
		            "currency", currency,
		            "descr", description,
		            "id", id,
		            NULL
		           );


		gchar *sql2 = "UPDATE expin SET category_id=##category::gint::NULL, \
		                              subcategory_id=##subcategory::gint::NULL, \
		                              quantity=##quantity::gint::NULL, \
		                              unit_id=##unit::gint::NULL \
		                       WHERE id=##id::gint";   
		
		db_exec_sql(sql2,
		            "category", category,
		            "subcategory", subcategory,
		            "quantity", quantity,
		            "unit", unit,
		            "id", id,
		            NULL
		           );
	}

		
	fill_grid_expenditure();
	fill_grid_income();
	fill_grid_account_short();
	fill_grid_account_full();

	gtk_widget_destroy(window);

	g_free(date);
	g_free(quantity);
	g_free(description);
}	





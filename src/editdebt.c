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
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "main.h"
#include "editdebt.h"
#include "db.h"
#include "sutil.h" 
#include "actions.h" 
#include "mainform.h" 

#include "gtkdateentry.h"

static GtkWidget *window;
static GdauiCombo *combo_person,
                  *combo_account, 
                  *combo_currency;

static GtkDateEntry *dateedit, 
                    *dateedit_remind;

static GtkSpinButton *spinbutton_percent,
                     *spinbutton_amount,
                     *spinbutton_period;
static GtkCheckButton *checkbutton_remind,*checkbutton_is_closed;
static GtkTextView *textview_description;
GValue *id;

static void fill_combo_person(gint oper);
static void on_button_ok_clicked(GtkButton *button,gint oper);
static void set_fields_values(gint oper);



void show_editdebt_window (gint oper)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "editdebt.glade");
	gtk_builder_connect_signals (gbuilder, NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *eventbox_dateedit = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit"));

	GtkContainer *eventbox_combo_person     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_person"));
	GtkContainer *eventbox_combo_account  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_account"));
	GtkContainer *eventbox_combo_currency = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_currency"));

	spinbutton_amount = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_amount"));
	spinbutton_percent = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_percent"));
	spinbutton_period = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_period"));
	checkbutton_is_closed = GTK_CHECK_BUTTON(gtk_builder_get_object (gbuilder, "checkbutton_is_closed"));
	checkbutton_remind = GTK_CHECK_BUTTON(gtk_builder_get_object (gbuilder, "checkbutton_remind"));
	GtkContainer *eventbox_dateedit_remind = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit_remind"));
	textview_description = GTK_TEXT_VIEW(gtk_builder_get_object (gbuilder, "textview_description"));
	
	GtkWidget *button_ok = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_ok"));
	GtkWidget *button_cancel = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_cancel"));
	
	g_signal_connect (G_OBJECT(button_ok), "clicked", G_CALLBACK (on_button_ok_clicked), oper);

	dateedit = create_dateentry(eventbox_dateedit);
	dateedit_remind = create_dateentry(eventbox_dateedit_remind);

	//Create Combo controls
	combo_person   = create_combo(eventbox_combo_person);
	combo_account  = create_combo(eventbox_combo_account);
	combo_currency = create_combo(eventbox_combo_currency);

	
	fill_combo_person(oper);

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_currency, currency_model, 1, viewcols);
	
	if (oper==EDITDEBT_DEBT || oper==EDITDEBT_CREDIT) 
		set_fields_values(oper);

	gtk_widget_show(window);
}	



void fill_combo_person(gint oper)
{
	GdaDataModel *db_model;

	if (oper == EDITDEBT_DEBT || oper == INSERTDEBT_DEBT) db_model = db_exec_select_sql ("SELECT id,name FROM person ORDER BY name",NULL);
	else db_model = db_exec_select_sql ("SELECT id,name FROM person ORDER BY name",NULL);

	if (!db_model) return;

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_person, db_model, 1, viewcols);
}


void set_fields_values(gint oper)
{


	ExGrid *grid = oper == EDITDEBT_DEBT ? grid_debt : grid_credit;

	id                 = ex_grid_get_selected_row_value (grid, DEB_ID_COL);
	guint date         = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_TIME_T_COL));
	guint person       = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_PERSON_ID_COL));
	guint account      = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_ACCOUNT_ID_COL));
	gdouble amount     = g_value_get_double(ex_grid_get_selected_row_value (grid,DEB_AMOUNT_COL));
	guint currency     = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_CURRENCY_ID_COL));
	gdouble percent    = g_value_get_double(ex_grid_get_selected_row_value (grid,DEB_PERCENT_COL));
	guint period       = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_PERIOD_COL));
	gboolean is_closed = g_value_get_boolean(ex_grid_get_selected_row_value (grid,DEB_IS_CLOSED_COL));
	gboolean remind    = g_value_get_boolean(ex_grid_get_selected_row_value (grid, DEB_REMIND_COL));
	guint date_remind  = g_value_get_int(ex_grid_get_selected_row_value (grid,DEB_REMIND_DATE_COL));
	gchar *description = g_value_get_string(ex_grid_get_selected_row_value (grid,DEB_DESCR_COL));
	
	gtk_dateentry_set_date(dateedit, date);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_person), ex_combo_get_row_by_id(combo_person, person));
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_account), ex_combo_get_row_by_id(combo_account,account));
	gtk_spin_button_set_value(spinbutton_amount, amount);	
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_currency), ex_combo_get_row_by_id(combo_currency,currency));	
	gtk_spin_button_set_value(spinbutton_percent, percent);	
	gtk_spin_button_set_value(spinbutton_period,period);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_is_closed), is_closed);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_remind), remind);
	gtk_dateentry_set_date(dateedit_remind, date_remind);
	ex_textview_set_text(textview_description, description);
}



void on_button_ok_clicked(GtkButton *button,gint oper)
{
	if ((gtk_combo_box_get_active(GTK_COMBO_BOX(combo_account)) == -1) || 
	    (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_person)) == -1)  ||
		(gtk_combo_box_get_active(GTK_COMBO_BOX(combo_currency)) == -1)
	   )
	{
		show_error_dialog(_("Warning!"), _("Warning!"), _("Some fields were not filled")); 
		return;
	}


	GValue *date        = ex_value_new_int(gtk_dateentry_get_date(dateedit));
	GValue *person      = ex_combo_get_current_row_value(combo_person, 0);
	GValue *account     = ex_combo_get_current_row_value(combo_account, 0);
	GValue *currency    = ex_combo_get_current_row_value(combo_currency,0);
	GValue *percent     = ex_value_new_double(gtk_spin_button_get_value(spinbutton_percent));
	GValue *period      = ex_value_new_int(gtk_spin_button_get_value_as_int(spinbutton_period));
	GValue *is_closed   = ex_value_new_boolean(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_is_closed)));
	GValue *remind      = ex_value_new_boolean(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_remind)));
	GValue *date_remind = ex_value_new_int(gtk_dateentry_get_date(dateedit_remind));
	GValue *description = ex_value_new_string(ex_textview_get_text(textview_description));
	GValue *amount;
	
	gdouble amountd      = gtk_spin_button_get_value(spinbutton_amount);


	if (oper == INSERTDEBT_DEBT || oper == INSERTDEBT_CREDIT)
	{
		amountd = oper == INSERTDEBT_DEBT ? amountd * (gdouble)-1 : amountd;

		gchar *sql = " INSERT INTO operation(date,account_id,amount,currency_id,description) \
		    		    VALUES (##date::gint, ##account::gint, ##amount::gdouble, ##currency::gint, ##descr::string)";


		amount = ex_value_new_double(amountd);
		
		GdaSet *row = db_exec_sql(sql,
		                  "date", date,
		                  "account", account,
		                  "amount", amount,
		                  "currency", currency,
		                  "descr", description,
		                  NULL
		                 );


		GValue *id = gda_set_get_holder_value(row, "+0");

		gchar *sql2 = "INSERT INTO debtcredit(id, percent, period, is_closed, remind, remind_date, person_id) \
					    VALUES (##id::gint, ##percent::gdouble, ##period::gint::NULL, ##is_closed::gboolean::NULL, ##remind::gboolean::NULL, ##remind_date::gint::NULL, ##person::gint)";

		db_exec_sql(sql2,
		            "id" , id,
		            "percent", percent,
		            "period", period,
		            "is_closed", is_closed,
		            "remind", remind,
		            "remind_date", date_remind,
		            "person", person,
		            NULL
		           );

	}


	if (oper == EDITDEBT_DEBT || oper ==  EDITDEBT_CREDIT)
	{
		amountd = oper == EDITDEBT_DEBT ? amountd * -1 : amountd;

		gchar *sql = "UPDATE operation SET date=##date::gint, \
		                                   account_id=##account::gint, \
		                                   amount=##amount::gdouble, \
		                                 currency_id=##currency::gint, \
		                                 description=##descr::string \
		                WHERE id=##id::gint";    

		amount = ex_value_new_double(amountd);
		
		db_exec_sql(sql,
		            "date", date,
		            "account", account,
		            "amount", amount,
		            "currency", currency,
		            "descr", description,
		            "id", id,
		            NULL
		           );


		gchar *sql2 = "UPDATE debtcredit SET percent=##percent::gdouble, \
		                               period=##period::gint::NULL, \
		                               is_closed=##is_closed::gboolean::NULL, \
		                               remind=##remind::gboolean::NULL, \
		                               remind_date=##remind_date::gint::NULL \
		                        WHERE id=##id::gint";   
		
		db_exec_sql(sql2,
		            "percent", percent,
		            "period", period,
		            "is_closed", is_closed,
		            "remind", remind,
		            "remind_date", date_remind,
		            "id", id,
		            NULL
		           );
	}
	
	fill_grid_debt();
	fill_grid_credit();
	fill_grid_account_short();
	fill_grid_account_full();
	
	gtk_widget_destroy(window);


	g_free(date);
	g_free(period);
	g_free(is_closed);
	g_free(remind);
	g_free(date_remind);
	g_free(description);
	g_free(amount);
	
	
}


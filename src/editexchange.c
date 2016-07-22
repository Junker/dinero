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
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <libgda-ui/libgda-ui.h>

#include "common.h"
#include "editexchange.h"
#include "mainform.h"
#include "db.h"
#include "actions.h"

#include "gtkdateentry.h"


static GtkWidget *window;
static GdauiCombo *combo_account,*combo_in_currency,*combo_out_currency;
static GtkDateEntry *dateedit;
static GtkSpinButton *spinbutton_in_amount,*spinbutton_out_amount;
static GtkTextView *textview_description;

static void on_button_ok_clicked(GtkButton *button,gpointer user_data);


void show_editexchange_window (void)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	gtk_builder_add_from_file (gbuilder,"editexchange.glade", NULL);
	gtk_builder_connect_signals (gbuilder,NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *eventbox_dateedit = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_dateedit"));

	combo_account = GDAUI_COMBO(gtk_builder_get_object (gbuilder, "combo_account"));
	combo_out_currency = GDAUI_COMBO(gtk_builder_get_object (gbuilder, "combo_out_currency"));
	combo_in_currency = GDAUI_COMBO(gtk_builder_get_object (gbuilder, "combo_in_currency"));

	GtkContainer *eventbox_combo_account      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_account"));
	GtkContainer *eventbox_combo_out_currency = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_out_currency"));
	GtkContainer *eventbox_combo_in_currency  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_combo_in_currency"));

	spinbutton_out_amount = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_out_amount"));
	spinbutton_in_amount = GTK_SPIN_BUTTON(gtk_builder_get_object (gbuilder, "spinbutton_in_amount"));
	textview_description = GTK_TEXT_VIEW(gtk_builder_get_object (gbuilder, "textview_description"));
	
	GtkWidget *button_ok = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_ok"));
	GtkWidget *button_cancel = GTK_WIDGET(gtk_builder_get_object (gbuilder, "button_cancel"));
	
	g_signal_connect(G_OBJECT(button_ok), "clicked", G_CALLBACK(on_button_ok_clicked), NULL);

	//Create Combo controls
	combo_account      = create_combo(eventbox_combo_account);
	combo_out_currency = create_combo(eventbox_combo_out_currency);
	combo_in_currency  = create_combo(eventbox_combo_in_currency);

	dateedit = create_dateentry(eventbox_dateedit);
	
	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_account, account_model, 1, viewcols);
	gdaui_combo_set_model(combo_out_currency, currency_model, 1, viewcols);
	gdaui_combo_set_model(combo_in_currency, currency_model, 1, viewcols);

	
	gtk_widget_show(window);
}



void on_button_ok_clicked(GtkButton *button, gpointer user_data)
{
	if ((gtk_combo_box_get_active(GTK_COMBO_BOX(combo_account)) == -1)      || 
	    (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_out_currency)) == -1) ||
	    (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_in_currency)) == -1)
	   ) 
	{
		show_error_dialog(_("Warning!"),_("Warning!"),_("Some fields were not filled")); 
		return;
	}	

	if (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_out_currency)) == gtk_combo_box_get_active(GTK_COMBO_BOX(combo_in_currency))) 
	{
		show_error_dialog(_("Warning!"),_("Warning!"),_("Please, choose different currency"));
		return;
	}
	
	GValue *account      = ex_combo_get_current_row_value(combo_account, 0);
	GValue *out_currency = ex_combo_get_current_row_value(combo_out_currency, 0);
	gdouble out_amount   = gtk_spin_button_get_value(spinbutton_out_amount);
	GValue *in_currency  = ex_combo_get_current_row_value(combo_in_currency, 0);
	gdouble in_amount    = gtk_spin_button_get_value(spinbutton_in_amount);
	GValue *date         = ex_value_new_int(gtk_date_entry_get_date(dateedit));
	GValue *description  = ex_value_new_string(ex_textview_get_text(textview_description));	


	gchar *sql = " INSERT INTO operation(date,account_id,amount,currency_id,description) \
		    		    VALUES (##date::gint, ##account::gint, ##amount::gdouble, ##currency::gint, ##descr::string)";

	GdaSet *row = db_exec_sql(sql,
	                         "date", date,
	                         "account", account,
	                         "amount", ex_value_new_double(out_amount * -1),
	                         "currency", out_currency,
	                         "descr", description,
	                         NULL
	                        );

	GdaSet *row2 = db_exec_sql(sql,
	                         "date", date,
	                         "account", account,
	                         "amount", ex_value_new_double(in_amount ),
	                         "currency", in_currency,
	                         "descr", description,
	                         NULL
	                        );

	GValue *exp_id    = gda_set_get_holder_value(row, "+0");
	GValue *income_id = gda_set_get_holder_value(row2, "+0");
	
	gchar *exch_sql = " INSERT INTO exchange(expenditure_id, income_id) \
		    		    VALUES (##exp::gint, ##income::gint)";

	db_exec_sql(exch_sql,
	            "exp", exp_id,
	            "income", income_id,
	            NULL
	           );

	
	fill_grid_account_short();
	fill_grid_account_full();	

	gtk_widget_destroy(window);

	g_free(date);
	g_free(description);
	
}

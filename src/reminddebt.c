/*
 * reminddebt.c
 *
 * Copyright (C) 2011 - Dmitry Kosenkov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h> 
#include <string.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "main.h"
#include "editaccount.h"
#include "actions.h"
#include "mainform.h"

#include "sutil.h"

static GtkWidget *dialog;
static GdauiRawForm *form;
static GtkCheckButton *checkbutton_remind;


static void fill_form();

static void on_button_close_clicked (GtkButton *button, gpointer user_data);


gint show_remind_debt_window (GValue *id)
{

	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "reminddebt.glade");
	gtk_builder_connect_signals (gbuilder,NULL);
	
	dialog = GTK_WIDGET(gtk_builder_get_object (gbuilder, "dialog"));
	checkbutton_remind = GTK_CHECK_BUTTON(gtk_builder_get_object (gbuilder, "checkbutton_remind"));

	GtkContainer *eventbox_form = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_form"));

	g_signal_connect(gtk_builder_get_object (gbuilder, "button_close"), "clicked", G_CALLBACK(on_button_close_clicked), id);
	
	form = create_form (eventbox_form);
	gtk_widget_show (form);
	
	fill_form(id);
	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	return response;
}

void fill_form(GValue *id)
{
	enum {
		DATE_COL,
		ACCOUNT_COL,
		PERSON_COL,
		PERCENT_COL,
		PERIOD_COL,
		AMOUNT_COL,
		PAY_AMOUNT_COL,
		CURRENCY_COL,
		DESCR_COL,
		CLOSED_DATE_COL,
		IS_CLOSED_COL
	};
	GdaDataModel *db_model = db_exec_select_sql ("SELECT strftime('%d.%m.%Y',date(a.date+1721425)), \
	                                                (SELECT name FROM account WHERE id=a.account_id), \
	                                                (SELECT name FROM person WHERE id=b.person_id), \
	                                                b.percent, b.period, \
	                                                cast(abs(a.amount) as text), \
	                                                cast((SELECT coalesce(sum(xx.amount),0) FROM operation xx, debtcredit_payment xz WHERE xx.id=xz.id AND xz.debtcredit_id=a.id) as text), \
	                                                (SELECT name FROM currency WHERE id=a.currency_id), \
	                                                a.description, \
	                                                coalesce(strftime('%d.%m.%Y',date(b.close_date+1721425)),''), b.is_closed \
	                                              FROM operation a, debtcredit b WHERE a.id=b.id AND a.id=##id::gint", 
	                                             "id", id,
	                                             NULL);
	if (!db_model) return;

	
	gda_data_model_set_column_name(db_model, DATE_COL, _("Date"));
	gda_data_model_set_column_name(db_model, ACCOUNT_COL, _("Account"));
	gda_data_model_set_column_name(db_model, PERSON_COL, _("Debtor"));
	gda_data_model_set_column_name(db_model, PERCENT_COL, _("Percent"));
	gda_data_model_set_column_name(db_model, PERIOD_COL, _("Period"));
	gda_data_model_set_column_name(db_model, IS_CLOSED_COL, _("Pay off"));
	gda_data_model_set_column_name(db_model, CLOSED_DATE_COL, _("Date pay off"));
	gda_data_model_set_column_name(db_model, AMOUNT_COL, _("Amount"));
	gda_data_model_set_column_name(db_model, PAY_AMOUNT_COL, _("Paid"));
	gda_data_model_set_column_name(db_model, CURRENCY_COL, _("Currency"));
	gda_data_model_set_column_name(db_model, DESCR_COL, _("Description"));

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(form), db_model);


}

void on_button_close_clicked (GtkButton *button, gpointer user_data)
{

	GValue *id = user_data;
	
	gboolean remind_bool = !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkbutton_remind));

	if (remind_bool) return;
	
	GValue *remind = ex_value_new_boolean (remind_bool);
	

	db_exec_sql ("UPDATE debtcredit SET remind=##remind::gboolean WHERE id=##id::gint",
	             "remind", remind,
	             "id", id,
	             NULL
	            );

	g_free(remind);
	
}


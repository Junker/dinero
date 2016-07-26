/*
 *
 * Copyright (C) Dmitry Kosenkov
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

#include "config.h"

#include <string.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "export.h"
#include "common.h"
#include "actions.h"
#include "mainform.h"
#include "db.h"
#include "sutil.h"

static GtkWidget *window;
static ExportEntity export_entity;

static GtkRadioButton *radiobutton_csv, *radiobutton_xml, *radiobutton_txt;

static GdaSet* get_export_options();
static GdaDataModel* get_accounts_model();
static GdaDataModel* get_expenses_model();
static GdaDataModel* get_income_model();
static GdaDataModel* get_debts_model();
static GdaDataModel* get_credits_model();

#define CSV_FILE_NAME_SUFFIX ".csv"
#define XML_FILE_NAME_SUFFIX ".xml"
#define TXT_FILE_NAME_SUFFIX ".txt"

void show_export_window(ExportEntity entity)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file(gbuilder, "export.ui");
	gtk_builder_connect_signals(gbuilder, NULL);

	window = GTK_WIDGET(gtk_builder_get_object(gbuilder, "window"));

	radiobutton_csv = GTK_RADIO_BUTTON(gtk_builder_get_object(gbuilder, "radiobutton_csv"));
	radiobutton_xml = GTK_RADIO_BUTTON(gtk_builder_get_object(gbuilder, "radiobutton_xml"));
	radiobutton_txt = GTK_RADIO_BUTTON(gtk_builder_get_object(gbuilder, "radiobutton_txt"));

	export_entity = entity;

	gtk_widget_show(window);
}

void save_export_data()
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	GdaDataModel *model; 
	GError *error = NULL;
	gint res;
	gint export_format;

	filter = gtk_file_filter_new();

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_csv)))
	{
		gtk_file_filter_set_name(filter, "CSV");
		gtk_file_filter_add_pattern(filter, "*.csv");

		export_format = GDA_DATA_MODEL_IO_TEXT_SEPARATED;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_xml)))
	{
		gtk_file_filter_set_name(filter, "XML");
		gtk_file_filter_add_pattern(filter, "*.xml");

		export_format = GDA_DATA_MODEL_IO_DATA_ARRAY_XML;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_txt)))
	{
		gtk_file_filter_set_name(filter, "Text");
		gtk_file_filter_add_pattern(filter, "*.txt");

		export_format = GDA_DATA_MODEL_IO_TEXT_TABLE;
	}

	dialog = gtk_file_chooser_dialog_new(_("Export data"),
	                                      GTK_WINDOW(window),
	                                      GTK_FILE_CHOOSER_ACTION_SAVE,
	                                      _("_Cancel"),
	                                      GTK_RESPONSE_CANCEL,
	                                      _("_Save"),
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);


	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		
		GString *file_path = g_string_new(gtk_file_chooser_get_filename(chooser));

		if (export_format == GDA_DATA_MODEL_IO_TEXT_SEPARATED)
		{
			if (!g_str_has_suffix(file_path->str, CSV_FILE_NAME_SUFFIX))
				g_string_append(file_path, CSV_FILE_NAME_SUFFIX);
		}
		else if (export_format == GDA_DATA_MODEL_IO_DATA_ARRAY_XML)
		{
			if (!g_str_has_suffix(file_path->str, XML_FILE_NAME_SUFFIX))
				g_string_append(file_path, XML_FILE_NAME_SUFFIX);
		}
		else if (export_format == GDA_DATA_MODEL_IO_TEXT_TABLE)
		{
			if (!g_str_has_suffix(file_path->str, TXT_FILE_NAME_SUFFIX))
				g_string_append(file_path, TXT_FILE_NAME_SUFFIX);
		}


		if (export_entity == EXPORT_ACCOUNTS)
			model = get_accounts_model();
		else if (export_entity == EXPORT_EXPENSES)
			model = get_expenses_model();
		else if (export_entity == EXPORT_INCOME)
			model = get_income_model();
		else if (export_entity == EXPORT_DEBTS)
			model = get_debts_model();
		else if (export_entity == EXPORT_CREDITS)
			model = get_credits_model();


		GdaSet *options = get_export_options();

		gda_data_model_export_to_file(model,
                               export_format,
                               file_path->str,
                               NULL,
                               3,
                               NULL,
                               3,
                               options,
                               &error);

		if (error)
		{
			show_warning_dialog(error->message, GTK_WINDOW(window));
			g_error(error->message);
		}

		gtk_widget_destroy(window);
	
		g_string_free(file_path, TRUE);
		g_object_unref(model);
	}

	gtk_widget_destroy(dialog);
}

static GdaSet* get_export_options()
{	
	GdaSet *set;
	GSList *list = NULL;

	GdaHolder *holder = gda_holder_new_boolean("NAMES_ON_FIRST_LINE", TRUE);

	list = g_slist_append(list, holder);

	set = gda_set_new(list);

	g_slist_free(list);

	return set;
}


static GdaDataModel* get_accounts_model()
{
	enum {
		NAME_COL,
		INITIAL_COL,
		EXPENSE_COL,
		INCOME_COL,
		BALANCE_COL,
		CURRENCY_COL,
		DESCR_COL
	};

	GdaDataModel *db_model = db_exec_select_sql("SELECT a.name,f.amount, \
	                                                  CAST(SUM(case when c.amount < 0 then c.amount else 0 end)*-1 AS real) AS expenses,\
	                                                  CAST(SUM(case when c.amount > 0 then c.amount else 0 end) AS real) AS income,\
	                                                  CAST(COALESCE(SUM(c.amount),0)+COALESCE(f.amount,0) AS real) AS balance,\
	                                                  b.id,COALESCE(a.description,'') AS description \
	                                              FROM account a, currency b \
                                                  LEFT JOIN operation c ON c.account_id=a.id AND c.currency_id=b.id \
                                                  JOIN account_startup f ON f.account_id=a.id AND f.currency_id=b.id AND f.active_currency=1\
                                                  GROUP BY a.id,b.id");

	gda_data_model_set_column_name(db_model, NAME_COL, _("Account"));
	gda_data_model_set_column_name(db_model, INITIAL_COL, _("Initial balance"));
	gda_data_model_set_column_name(db_model, EXPENSE_COL, _("Expenses"));
	gda_data_model_set_column_name(db_model, INCOME_COL,  _("Income"));
	gda_data_model_set_column_name(db_model, BALANCE_COL, _("Balance"));
	gda_data_model_set_column_name(db_model, CURRENCY_COL, _("Currency"));
	gda_data_model_set_column_name(db_model, DESCR_COL, _("Description"));

	gda_data_model_set_column_title(db_model, NAME_COL, _("Account"));
	gda_data_model_set_column_title(db_model, INITIAL_COL, _("Initial balance"));
	gda_data_model_set_column_title(db_model, EXPENSE_COL, _("Expenses"));
	gda_data_model_set_column_title(db_model, INCOME_COL,  _("Income"));
	gda_data_model_set_column_title(db_model, BALANCE_COL, _("Balance"));
	gda_data_model_set_column_title(db_model, CURRENCY_COL, _("Currency"));
	gda_data_model_set_column_title(db_model, DESCR_COL, _("Description"));

	return db_model;
}

static GdaDataModel* get_expenses_model()
{
	
}

static GdaDataModel* get_income_model()
{
	
}

static GdaDataModel* get_debts_model()
{
	
}

static GdaDataModel* get_credits_model()
{
	
}


void on_button_save_clicked(GtkButton *button, gpointer user_data)
{
	save_export_data();
}
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

#include <gtk/gtk.h>
#include <string.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "common.h"
#include "db.h"
#include "account_datedetailed.h"
#include "mainform.h"
#include "sutil.h"

enum
{
	NAME_COL,
	AMMOUNT_COL,
	NUM_COLS
} ;


static GtkWidget *window;
static GtkTreeView *treeview;
static GtkLabel *label_date,*label_account;
static GtkTreeStore *treestore;

static void create_and_fill_model ();

void show_account_datedetailed_window ()
{
	GtkBuilder *gbuilder = gtk_builder_new();
	
	ex_builder_load_file (gbuilder, "account_datedetailed.ui");

	
	gtk_builder_connect_signals (gbuilder,NULL);
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));
	treeview = GTK_TREE_VIEW(gtk_builder_get_object (gbuilder, "treeview"));
	label_date = GTK_LABEL(gtk_builder_get_object (gbuilder, "label_date"));
	label_account = GTK_LABEL(gtk_builder_get_object (gbuilder, "label_account"));
	treestore = GTK_TREE_STORE(gtk_builder_get_object (gbuilder, "treestore"));

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gtk_builder_get_object (gbuilder, "scrolledwindow")), 
	                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	gchar text[150];
	g_sprintf(text,_("Date: %s"), g_value_get_string(ex_grid_get_selected_row_value (grid_account_full,ACF_DATE_COL)));
	gtk_label_set_text(label_date, text);
	
	GValue *account = db_get_value ("SELECT name FROM account WHERE id=##id::gint",
	                                                  "id", ex_grid_get_selected_row_value(grid_account_full, ACF_ID_COL),
	                                                  NULL
	                                                 );
	
	gchar *account_str = g_value_get_string(account);
	
	g_sprintf(text, _("Account: %s"), account_str);
	gtk_label_set_text(label_account, text);

	create_and_fill_model();
	
	
	gtk_widget_show(window);
}


static void create_and_fill_model ()
{
	GtkTreeIter toplevel, child;
	gint i;
	
	GValue *date     = ex_grid_get_selected_row_value (grid_account_full, ACF_TIME_T_COL);
	GValue *account  = ex_grid_get_selected_row_value (grid_account_full, ACF_ID_COL);
	GValue *currency = ex_grid_get_selected_row_value (grid_account_full, ACF_CURRENCY_ID_COL);


	// -- Expenditure
	
	gchar *sql = "SELECT (SELECT name FROM category WHERE id=b.category_id),  \
	                     cast(amount*-1 as text)||(SELECT short_name FROM currency WHERE id=a.currency_id)  \
	                FROM operation a, expin b \
	                WHERE a.id=b.id AND date=##date::gint AND account_id=##account::gint AND \
	                      a.currency_id=##currency::gint AND amount < 0";

	GdaDataModel *gda_model = db_exec_select_sql (sql,
	                                              "date", date,
	                                              "account", account,
	                                              "currency", currency,
	                                              NULL);

	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel, NAME_COL, _("Expends"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, NAME_COL, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	g_object_unref(gda_model);	
	
	// -- Income

	gchar *sql2 = "SELECT (SELECT name FROM category WHERE id=b.category_id), \
	                      cast(amount as text)||(SELECT short_name FROM currency WHERE id=a.currency_id) \
	               FROM operation a, expin b \
	               WHERE a.id=b.id AND date=##date::gint AND account_id=##account::gint AND \
	                     a.currency_id=##currency::gint AND amount > 0";

	gda_model = db_exec_select_sql (sql2,
	                                "date", date,
	                                "account", account,
	                                "currency", currency,
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel, NAME_COL, _("Income"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	
	g_object_unref(gda_model);	


	// -- Credits

	gchar *sql_debt = "SELECT (SELECT name FROM person WHERE id=b.person_id),\
	                          cast(a.amount as text)||(SELECT short_name FROM currency WHERE id=a.currency_id) \
	                   FROM operation a, debtcredit b \
	                   WHERE a.id=b.id AND a.date=##date::gint AND a.account_id=##account::gint AND \
	                         a.currency_id=##currency::gint AND a.amount > 0";
		
	gda_model = db_exec_select_sql (sql_debt, 
	                                "date", date, 
	                                "account", account, 
                                    "currency", currency,
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model)>0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,
                     NAME_COL,_("Credit"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	g_object_unref(gda_model);	

	// -- Debts
	
	gchar *sql_credit = "SELECT (SELECT name FROM person WHERE id=b.person_id),\
	                            cast(a.amount*-1 as text)||(SELECT short_name FROM currency WHERE id=a.currency_id) \
	                  FROM operation a, debtcredit b \
	                  WHERE a.id=b.id AND a.date=##date::gint AND a.account_id=##account::gint AND \
	                        a.currency_id=##currency::gint AND a.amount < 0";
		
	gda_model = db_exec_select_sql (sql_credit, 
	                                "date", date, 
	                                "account", account, 
                                    "currency", currency,	
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model)>0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,
                     NAME_COL,_("Debt"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	g_object_unref(gda_model);	
	

	// -- Transfers

	gchar *sql_transfer = "SELECT (SELECT name FROM account WHERE id=b.account_id)||' -> '||(SELECT name FROM account WHERE id=c.account_id), \
	                              cast(c.amount as text)||(SELECT short_name FROM currency WHERE id=b.currency_id), b.description \
	                       FROM transfer a, operation b, operation c \
	                       WHERE a.expenditure_id=b.id AND a.income_id=c.id AND b.date=##date::gint \
	                             AND (b.account_id=##account::gint OR c.account_id=##account::gint) AND b.currency_id=##currency::gint";

	gda_model = db_exec_select_sql (sql_transfer, 
	                                "date", date,
	                                "account", account,
                                    "currency", currency,
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,
                     NAME_COL,_("Transfer"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	g_object_unref(gda_model);	

	// -- Currency exchange

	gchar *sql_exchange ="SELECT (SELECT name FROM account WHERE id=b.account_id),\
	                             cast(b.amount*-1 as text)||(SELECT short_name FROM currency WHERE id=b.currency_id)||' -> '||cast(c.amount as text)||(SELECT short_name FROM currency WHERE id=c.currency_id), b.description \
	                      FROM exchange a, operation b, operation c \
	                      WHERE a.expenditure_id=b.id AND a.income_id=c.id AND b.date=##date::gint AND b.account_id=##account::gint AND \
	                            (b.currency_id=##currency::gint OR c.currency_id=##currency::gint)";

	gda_model = db_exec_select_sql (sql_exchange, 
	                                "date", date,
	                                "account", account,
                                    "currency", currency,	                                
	                                NULL);
	          
	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,
                     NAME_COL,_("Currency Exchange"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	g_object_unref(gda_model);	

	// -- Debt Payments
	
	gchar *sql_debt_payment = "SELECT (SELECT name FROM person WHERE id=(SELECT person_id FROM debtcredit WHERE id=b.debtcredit_id)), \
	                                  cast(amount as text)||(SELECT short_name FROM currency WHERE id=a.currency_id) \
	                           FROM operation a, debtcredit_payment b \
	                           WHERE a.id=b.id AND date=##date::gint AND account_id=##account::gint AND \
	                                 a.currency_id=##currency::gint AND a.amount > 0";

	gda_model = db_exec_select_sql (sql_debt_payment,
	                                "date", date,
	                                "account", account,
                                    "currency", currency,
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel, NAME_COL, _("Debt Payments"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}
	
	g_object_unref(gda_model);	

	// -- Credit Payments

	gchar *sql_credit_payment = "SELECT (SELECT name FROM person WHERE id=(SELECT person_id FROM debtcredit WHERE id=b.debtcredit_id)), \
	                                    cast(amount as text)||(SELECT short_name FROM currency WHERE id=a.currency_id) \
	                           FROM operation a, debtcredit_payment b \
	                           WHERE a.id=b.id AND date=##date::gint AND account_id=##account::gint AND \
	                                 a.currency_id=##currency::gint AND a.amount < 0";

	gda_model = db_exec_select_sql (sql_credit_payment,
	                                "date", date,
	                                "account", account,
                                    "currency", currency,
	                                NULL);

	if (gda_data_model_get_n_rows(gda_model) > 0) 
	{
		gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel, NAME_COL, _("Credit Payments"), -1);

		for(i=0;i<gda_data_model_get_n_rows(gda_model);i++) 
		{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set_value(treestore, &child, NAME_COL, gda_data_model_get_value_at(gda_model, 0, i, NULL));
			gtk_tree_store_set_value(treestore, &child, AMMOUNT_COL, gda_data_model_get_value_at(gda_model, AMMOUNT_COL, i, NULL));
		}
	}

	gtk_tree_view_expand_all (treeview);

	
	g_object_unref(gda_model);
	
}
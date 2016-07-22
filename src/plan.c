/*
 * plan_payment.c
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

#include "config.h"

#include <libgda-ui/libgda-ui.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "common.h"
#include "plan.h"
#include "mainform.h"
#include "actions.h"
#include "db.h"
#include "sutil.h"

static GtkTreeView *grid;
static GtkListStore *store;

static 	GtkWidget *window;
static GtkButton *button_del;




	

static enum {
	STORE_DATE_COL,
	STORE_ACCOUNT_COL,
	STORE_CATEGORY_COL,
	STORE_SUBCATEGORY_COL,
	STORE_QUANTITY_COL,
	STORE_UNIT_COL,
	STORE_PERIODICITY_COL,
	STORE_AMOUNT_TEXT_COL,
	STORE_CURRENCY_COL,
	STORE_PLAN_ID_COL,
	STORE_TIME_T_COL
};

static void fill_grid();

void on_plan_grid_cursor_changed (GtkTreeView *tree_view, gpointer user_data);
void on_plan_button_del_clicked (GtkButton *button,gpointer user_data);


static gboolean proccess_list_store (GtkTreeModel *model,
                                     GtkTreePath *path,
                                     GtkTreeIter *iter,
                                     gpointer data);





void show_plan_payment_window ()
{

	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "plan.glade");
	gtk_builder_connect_signals (gbuilder,NULL);
	
	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	store  = GTK_LIST_STORE(gtk_builder_get_object (gbuilder, "liststore"));
	grid   = GTK_TREE_VIEW(gtk_builder_get_object (gbuilder, "grid"));

	button_del  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_del"));

	gtk_tree_selection_set_mode ( gtk_tree_view_get_selection (grid), GTK_SELECTION_MULTIPLE );
	
	fill_grid();
	
	if (gtk_tree_model_iter_n_children(store, NULL) > 0)
		gtk_widget_show(window);
	else gtk_widget_destroy(window);
}



void fill_grid()
{
	
	static enum {
		ID_COL,
		DATE_COL,
		LAST_DATE_COL,
		PERIODICITY_COL,
		PERIODICITY_DAYS_COL,
		ACCOUNT_COL,
		CATEGORY_COL,
		SUBCATEGORY_COL,
		QUANTITY_COL,
		UNIT_COL,
		AMOUNT_TEXT_COL,
		CURRENCY_COL,
		DESCRIPTION_COL
	};
	
	time_t date_time_t = get_current_date ();

	GValue *date = ex_value_new_int (date_time_t);
	
	GdaDataModel *db_model = db_exec_select_sql("SELECT id, date, last_date, periodicity, periodicity_days, \
	                                              (SELECT name FROM account WHERE id=a.account_id), \
	                                              (SELECT name FROM category WHERE id=a.category_id), \
	                                              (SELECT name FROM subcategory WHERE id=a.subcategory_id), \
	                                              quantity, \
	                                              (SELECT name FROM unit WHERE id=a.unit_id), \
	                                              cast(amount as text), \
	                                              (SELECT name FROM currency WHERE id=a.currency_id), description \
	                                             FROM plan a WHERE date<##date::gint AND (last_date<##date::gint OR last_date IS NULL)", 
	                                            "date", date, NULL);

	g_free(date);

	GDate *gdate = g_date_new ();
	g_date_set_time_t (gdate, date_time_t);

	guint i;
	for(i = 0; i < gda_data_model_get_n_rows(db_model); i++) 
	{
		GValue *plan        = gda_data_model_get_value_at(db_model, ID_COL, i, NULL);
		GValue *account     = gda_data_model_get_value_at(db_model, ACCOUNT_COL, i, NULL);
		GValue *category    = gda_data_model_get_value_at(db_model, CATEGORY_COL, i, NULL);
		GValue *subcategory = gda_data_model_get_value_at(db_model, SUBCATEGORY_COL, i, NULL);
		GValue *quantity    = gda_data_model_get_value_at(db_model, QUANTITY_COL, i, NULL);
		GValue *unit        = gda_data_model_get_value_at(db_model, UNIT_COL, i, NULL);
		GValue *amount_text = gda_data_model_get_value_at(db_model, AMOUNT_TEXT_COL, i, NULL);
		GValue *currency    = gda_data_model_get_value_at(db_model, CURRENCY_COL, i, NULL);
		GValue *description = gda_data_model_get_value_at(db_model, DESCRIPTION_COL, i, NULL);


		
		gint periodicity        = g_value_get_int (gda_data_model_get_value_at(db_model, PERIODICITY_COL, i, NULL));
		gint plan_date_time_t   = g_value_get_int (gda_data_model_get_value_at(db_model, DATE_COL, i, NULL));

		gint plan_last_date_time_t = g_value_get_int (gda_data_model_get_value_at(db_model, LAST_DATE_COL, i, NULL));

		GDate *plan_gdate = g_date_new();
		g_date_set_time_t(plan_gdate, plan_date_time_t);

		GValue *periodicity_val;
		
		switch (periodicity)
		{
			case PERIODICITY_ONCE:
				periodicity_val = ex_value_new_string (_("Once"));
				break;
			case PERIODICITY_DAY:
				periodicity_val = ex_value_new_string (_("Daily"));
				break;
			case PERIODICITY_MONTH:
				periodicity_val = ex_value_new_string (_("Monthly"));
				break;
			case PERIODICITY_QUARTER:
				periodicity_val = ex_value_new_string (_("Quarterly"));
				break;
			case PERIODICITY_YEAR:
				periodicity_val = ex_value_new_string (_("Yearly"));
		}
			

		
		if (periodicity == PERIODICITY_DAY) //Days
		{
			gint periodicity_days = g_value_get_int(gda_data_model_get_value_at(db_model, PERIODICITY_DAYS_COL, i, NULL));
			
			if (plan_last_date_time_t > 0)
				g_date_set_time_t(plan_gdate, plan_last_date_time_t);

			while (g_date_compare (gdate, plan_gdate) != 0)
			{
				g_date_add_days (plan_gdate, 1);
				if (((g_date_get_weekday (plan_gdate) == G_DATE_MONDAY) && (periodicity_days & PERIODICITY_MONDAY))       ||
					((g_date_get_weekday (plan_gdate) == G_DATE_TUESDAY) && (periodicity_days & PERIODICITY_TUESDAY))     ||
				    ((g_date_get_weekday (plan_gdate) == G_DATE_WEDNESDAY) && (periodicity_days & PERIODICITY_WEDNESDAY)) ||
				    ((g_date_get_weekday (plan_gdate) == G_DATE_THURSDAY) && (periodicity_days & PERIODICITY_THURSDAY))   ||
				    ((g_date_get_weekday (plan_gdate) == G_DATE_FRIDAY) && (periodicity_days & PERIODICITY_FRIDAY))       ||
				    ((g_date_get_weekday (plan_gdate) == G_DATE_SATURDAY) && (periodicity_days & PERIODICITY_SATURDAY))   ||
				    ((g_date_get_weekday (plan_gdate) == G_DATE_SUNDAY) && (periodicity_days & PERIODICITY_SUNDAY))
				   )
				{	
					GtkTreeIter iter;
					gtk_list_store_append(store, &iter);

					GValue *plan_date = ex_date_get_formated_date_value(plan_gdate);

					gtk_list_store_set_value (store, &iter, STORE_DATE_COL, plan_date);
					gtk_list_store_set_value (store, &iter, STORE_ACCOUNT_COL, account);
					gtk_list_store_set_value (store, &iter, STORE_CATEGORY_COL, category);
					gtk_list_store_set_value (store, &iter, STORE_QUANTITY_COL, quantity);
					gtk_list_store_set_value (store, &iter, STORE_SUBCATEGORY_COL, subcategory);
					gtk_list_store_set_value (store, &iter, STORE_UNIT_COL, unit);
					gtk_list_store_set_value (store, &iter, STORE_AMOUNT_TEXT_COL, amount_text);
					gtk_list_store_set_value (store, &iter, STORE_CURRENCY_COL, currency);
					gtk_list_store_set_value (store, &iter, STORE_PERIODICITY_COL, periodicity_val);
					gtk_list_store_set_value (store, &iter, STORE_PLAN_ID_COL, plan);
					gtk_list_store_set_value (store, &iter, STORE_TIME_T_COL, ex_value_new_int (gtk_date_entry_get_date(plan_gdate)));


					
//					g_free(plan_date);
//					g_free(plan_date_str);
					
				}
			}
		}

		if (periodicity == PERIODICITY_MONTH) //Month
		{
			if (plan_last_date_time_t > 0)
			{
				GDate *plan_last_gdate = g_date_new();
				g_date_set_time_t(plan_last_gdate, plan_last_date_time_t);
				while (g_date_compare (plan_gdate, plan_last_gdate) < 0)
					g_date_add_months(plan_gdate, 1);
			}

			while (g_date_compare (gdate, plan_gdate) > 0)
			{
				GtkTreeIter iter;
				gtk_list_store_append(store, &iter);

				GValue *plan_date = ex_date_get_formated_date_value(plan_gdate);

				gtk_list_store_set_value (store, &iter, STORE_DATE_COL, plan_date);
				gtk_list_store_set_value (store, &iter, STORE_ACCOUNT_COL, account);
				gtk_list_store_set_value (store, &iter, STORE_CATEGORY_COL, category);
				gtk_list_store_set_value (store, &iter, STORE_QUANTITY_COL, quantity);
				gtk_list_store_set_value (store, &iter, STORE_SUBCATEGORY_COL, subcategory);
				gtk_list_store_set_value (store, &iter, STORE_UNIT_COL, unit);
				gtk_list_store_set_value (store, &iter, STORE_AMOUNT_TEXT_COL, amount_text);
				gtk_list_store_set_value (store, &iter, STORE_CURRENCY_COL, currency);
				gtk_list_store_set_value (store, &iter, STORE_PERIODICITY_COL, periodicity_val);
				gtk_list_store_set_value (store, &iter, STORE_PLAN_ID_COL, plan);
				gtk_list_store_set_value (store, &iter, STORE_TIME_T_COL, ex_value_new_int (gtk_date_entry_get_date(plan_gdate)));

				g_date_add_months(plan_gdate, 1);
			}
			
		}
		if (periodicity == PERIODICITY_QUARTER) //Quarter
		{
			if (plan_last_date_time_t > 0)
			{
				GDate *plan_last_gdate = g_date_new();
				g_date_set_time_t(plan_last_gdate, plan_last_date_time_t);
				while (g_date_compare (plan_gdate, plan_last_gdate) < 0)
					g_date_add_months(plan_gdate, 4);
			}

			while (g_date_compare (gdate, plan_gdate) > 0)
			{
				GtkTreeIter iter;
				gtk_list_store_append(store, &iter);

				GValue *plan_date = ex_date_get_formated_date_value(plan_gdate);

				gtk_list_store_set_value (store, &iter, STORE_DATE_COL, plan_date);
				gtk_list_store_set_value (store, &iter, STORE_ACCOUNT_COL, account);
				gtk_list_store_set_value (store, &iter, STORE_CATEGORY_COL, category);
				gtk_list_store_set_value (store, &iter, STORE_QUANTITY_COL, quantity);
				gtk_list_store_set_value (store, &iter, STORE_SUBCATEGORY_COL, subcategory);
				gtk_list_store_set_value (store, &iter, STORE_UNIT_COL, unit);
				gtk_list_store_set_value (store, &iter, STORE_AMOUNT_TEXT_COL, amount_text);
				gtk_list_store_set_value (store, &iter, STORE_CURRENCY_COL, currency);
				gtk_list_store_set_value (store, &iter, STORE_PERIODICITY_COL, periodicity_val);
				gtk_list_store_set_value (store, &iter, STORE_PLAN_ID_COL, plan);
				gtk_list_store_set_value (store, &iter, STORE_TIME_T_COL, ex_value_new_int (gtk_date_entry_get_date(plan_gdate)));

				g_date_add_months(plan_gdate, 4);
			}
			
		}		
		if (periodicity == PERIODICITY_YEAR) //Year
		{
			if (plan_last_date_time_t > 0)
			{
				GDate *plan_last_gdate = g_date_new();
				g_date_set_time_t(plan_last_gdate, plan_last_date_time_t);
				while (g_date_compare (plan_gdate, plan_last_gdate) < 0)
					g_date_add_years(plan_gdate, 1);
			}

			while (g_date_compare (gdate, plan_gdate) > 0)
			{
				GtkTreeIter iter;
				gtk_list_store_append(store, &iter);

				GValue *plan_date = ex_date_get_formated_date_value(plan_gdate);

				gtk_list_store_set_value (store, &iter, STORE_DATE_COL, plan_date);
				gtk_list_store_set_value (store, &iter, STORE_ACCOUNT_COL, account);
				gtk_list_store_set_value (store, &iter, STORE_CATEGORY_COL, category);
				gtk_list_store_set_value (store, &iter, STORE_QUANTITY_COL, quantity);
				gtk_list_store_set_value (store, &iter, STORE_SUBCATEGORY_COL, subcategory);
				gtk_list_store_set_value (store, &iter, STORE_UNIT_COL, unit);
				gtk_list_store_set_value (store, &iter, STORE_AMOUNT_TEXT_COL, amount_text);
				gtk_list_store_set_value (store, &iter, STORE_CURRENCY_COL, currency);
				gtk_list_store_set_value (store, &iter, STORE_PERIODICITY_COL, periodicity_val);
				gtk_list_store_set_value (store, &iter, STORE_PLAN_ID_COL, plan);
				gtk_list_store_set_value (store, &iter, STORE_TIME_T_COL, ex_value_new_int (gtk_date_entry_get_date(plan_gdate)));
				
				g_date_add_years(plan_gdate, 1);
			}
			
		}		

	} 

	g_object_unref(db_model);
}


void on_plan_button_ok_clicked (GtkButton *button, gpointer user_data)
{
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id, account_id, category_id, subcategory_id, quantity, unit_id, amount, currency_id FROM plan");

	gtk_tree_model_foreach (GTK_TREE_MODEL(store), proccess_list_store, db_model);

	g_object_unref (db_model);

	fill_grid_account_full();
	fill_grid_account_short();
	fill_grid_expenditure();
	fill_grid_income();
	
	gtk_widget_destroy(window);
}


void on_plan_button_del_clicked (GtkButton *button, gpointer user_data)
{
	
	GList *iter_list = ex_tree_view_get_rows(grid);
	
	while(iter_list) 
	{
		gtk_list_store_remove (store, iter_list -> data);
		iter_list = iter_list -> next;
	}


	g_list_foreach (iter_list, (GFunc) gtk_tree_iter_free, NULL);
	g_list_free(iter_list);	
	
}

void on_plan_grid_cursor_changed (GtkTreeView *tree_view, gpointer user_data)
{
	gtk_widget_set_sensitive (button_del, TRUE);
}




gboolean proccess_list_store (GtkTreeModel *model,
                              GtkTreePath *path,
                              GtkTreeIter *iter,
                              gpointer data)
{
	GdaDataModel *db_model = (GdaDataModel*) data;

	
	enum {
		ID_COL,
		ACCOUNT_COL,
		CATEGORY_COL,
		SUBCATEGORY_COL,
		QUANTITY_COL,
		UNIT_COL,
		AMOUNT_COL,
		CURRENCY_COL
	};

	
	GValue plan = {0};
	GValue date = {0};
	gtk_tree_model_get_value (model, iter, STORE_PLAN_ID_COL, &plan);
	gtk_tree_model_get_value (model, iter, STORE_TIME_T_COL, &date);

	gint id = g_value_get_int(&plan);
	gint row_idx = ex_data_model_get_row_by_id (db_model, id);
	
	GValue *account     = gda_data_model_get_value_at(db_model, ACCOUNT_COL, row_idx, NULL);
	GValue *category    = gda_data_model_get_value_at(db_model, CATEGORY_COL, row_idx, NULL);
	GValue *subcategory = gda_data_model_get_value_at(db_model, SUBCATEGORY_COL, row_idx, NULL);
	GValue *quantity    = gda_data_model_get_value_at(db_model, QUANTITY_COL, row_idx, NULL);
	GValue *unit        = gda_data_model_get_value_at(db_model, UNIT_COL, row_idx, NULL);
	GValue *amount      = gda_data_model_get_value_at(db_model, AMOUNT_COL, row_idx, NULL);
	GValue *currency    = gda_data_model_get_value_at(db_model, CURRENCY_COL, row_idx, NULL);
	
	

	gchar *oper_sql = " INSERT INTO operation(date,account_id,amount,currency_id,description) \
	                    VALUES (##date::gint, ##account::gint, ##amount::gdouble, ##currency::gint, ##descr::string::NULL)";
	
	GdaSet *row = db_exec_sql(oper_sql,
	                          "date", &date,
	                          "account", account,
	                          "amount", amount,
	                          "currency", currency,
	                          "descr", NULL,
	                          NULL
	                         );


	GValue *expin_id = gda_set_get_holder_value(row, "+0");
		                                                      
	gchar *expin_sql = "INSERT INTO expin(id, category_id, subcategory_id, quantity, unit_id) \
	                    VALUES (##id::gint, ##category::gint::NULL, ##subcategory::gint::NULL, ##quantity::gint::NULL, ##unit::gint::NULL)";

	db_exec_sql(expin_sql,
	                "category", category,
	                "subcategory", subcategory,
	                "quantity", quantity,
	                "unit", unit,
	                "id", expin_id,
	                NULL
	               );


	time_t current_date_time_t = get_current_date ();
	GValue *current_date = ex_value_new_int (current_date_time_t);
	
	db_exec_sql ("UPDATE plan SET last_date=##date::gint WHERE id=##id::gint",
	             "date", current_date,
	             "id", &plan,
	             NULL
	            );

	g_free(current_date);

	return FALSE;
}
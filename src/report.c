/***************************************************************************
 *            report.c
 *
 *  Wed Oct 11 13:54:10 2011
 *  Copyright  2011  Dmitry Kosenkov
 *  <junker@front.ru>
 ****************************************************************************/

/*
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
#include <libgda-ui/libgda-ui.h>
#include <string.h>
#include <glib/gi18n-lib.h>

#include "main.h"
#include "actions.h" 
#include "report.h" 
#include "mainform.h"

#include "gtkdateentry.h"

static GtkWidget *window;

static ExGrid      *grid_rep1_expenditure,
                    *grid_rep1_income,
                    *grid_rep1_total,
                    *grid_rep2_expenditure,
                    *grid_rep2_income,
                    *grid_rep2_total,
                    *grid_rep3;

static GtkDateEntry  *dateedit_rep1_from,
                     *dateedit_rep1_to,
                     *dateedit_rep2_from,
                     *dateedit_rep2_to,
                     *dateedit_rep3_from,
                     *dateedit_rep3_to;


static GtkToggleButton *cbutton_rep1_accounts,
                       *cbutton_rep1_categories,
                       *cbutton_rep1_subcategories,
                       *cbutton_rep1_quantity,
                       *cbutton_rep1_dateinterval,
                       *cbutton_rep2_dateinterval,
                       *cbutton_rep3_dateinterval;

static GdauiCombo *combo_rep1_currency,
                  *combo_rep2_currency,
                  *combo_rep3_account;

static GtkComboBox *combobox_rep2_group,
                   *combobox_rep3_group;

static void	fill_grid_rep1_expenditure();
static void	fill_grid_rep1_income();
static void	fill_grid_rep1_total();
static void	fill_grid_rep2_expenditure();
static void	fill_grid_rep2_income();
static void	fill_grid_rep2_total();
static void	fill_grid_rep3();

void show_report_window (void)
{
	
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "report.glade");
	gtk_builder_connect_signals (gbuilder,NULL);

	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	GtkContainer *scrolledwindow_rep1_expenditure = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep1_expenditure"));
	GtkContainer *scrolledwindow_rep1_income      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep1_income"));
	GtkContainer *scrolledwindow_rep1_total       = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep1_total"));
	GtkContainer *scrolledwindow_rep2_expenditure = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep2_expenditure"));
	GtkContainer *scrolledwindow_rep2_income      = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep2_income"));
	GtkContainer *scrolledwindow_rep2_total       = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep2_total"));
	GtkContainer *scrolledwindow_rep3_grid        = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_rep3_grid"));

	GtkContainer *eventbox_rep1_currency = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep1_currency"));
	GtkContainer *eventbox_rep2_currency = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep2_currency"));
	GtkContainer *eventbox_rep3_account  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep3_account"));

	GtkContainer *eventbox_rep1_dateedit_from = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep1_dateedit_from"));
	GtkContainer *eventbox_rep1_dateedit_to   = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep1_dateedit_to"));
	GtkContainer *eventbox_rep2_dateedit_from = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep2_dateedit_from"));
	GtkContainer *eventbox_rep2_dateedit_to   = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep2_dateedit_to"));
	GtkContainer *eventbox_rep3_dateedit_from = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep3_dateedit_from"));
	GtkContainer *eventbox_rep3_dateedit_to   = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "eventbox_rep3_dateedit_to"));

	
	cbutton_rep1_accounts      = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep1_accounts"));
	cbutton_rep1_categories    = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep1_categories"));
	cbutton_rep1_subcategories = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep1_subcategories"));
	cbutton_rep1_quantity      = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep1_quantity"));
	cbutton_rep1_dateinterval  = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep1_dateinterval"));
	cbutton_rep2_dateinterval  = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep2_dateinterval"));
	cbutton_rep3_dateinterval  = GTK_TOGGLE_BUTTON(gtk_builder_get_object (gbuilder, "cbutton_rep3_dateinterval"));

	combobox_rep2_group  = GTK_COMBO_BOX(gtk_builder_get_object (gbuilder, "combobox_rep2_group"));
	combobox_rep3_group  = GTK_COMBO_BOX(gtk_builder_get_object (gbuilder, "combobox_rep3_group"));

	
	dateedit_rep1_from = create_dateentry (eventbox_rep1_dateedit_from);
	dateedit_rep1_to   = create_dateentry (eventbox_rep1_dateedit_to);
	dateedit_rep2_from = create_dateentry (eventbox_rep2_dateedit_from);
	dateedit_rep2_to   = create_dateentry (eventbox_rep2_dateedit_to);
	dateedit_rep3_from = create_dateentry (eventbox_rep3_dateedit_from);
	dateedit_rep3_to   = create_dateentry (eventbox_rep3_dateedit_to);

	grid_rep1_expenditure = create_grid(scrolledwindow_rep1_expenditure);
	grid_rep1_income      = create_grid(scrolledwindow_rep1_income);
	grid_rep1_total       = create_grid(scrolledwindow_rep1_total);
	grid_rep2_expenditure = create_grid(scrolledwindow_rep2_expenditure);
	grid_rep2_income      = create_grid(scrolledwindow_rep2_income);
	grid_rep2_total       = create_grid(scrolledwindow_rep2_total);
	grid_rep3             = create_grid(scrolledwindow_rep3_grid);


	combo_rep1_currency = create_combo(eventbox_rep1_currency);
	combo_rep2_currency = create_combo(eventbox_rep2_currency);
	combo_rep3_account = create_combo(eventbox_rep3_account);
	
	gtk_dateentry_set_date(dateedit_rep1_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_rep2_from, get_current_date() - 30);
	gtk_dateentry_set_date(dateedit_rep3_from, get_current_date() - 30);

	gint viewcols[1] = {1};
	gdaui_combo_set_model(combo_rep1_currency, currency_model, 1, viewcols);
	gdaui_combo_add_null(combo_rep1_currency, TRUE);	
	gdaui_combo_set_model(combo_rep2_currency, currency_model, 1, viewcols);
	gdaui_combo_add_null(combo_rep2_currency, TRUE);	
	gdaui_combo_set_model(combo_rep3_account, account_model, 1, viewcols);
	gdaui_combo_add_null(combo_rep3_account, TRUE);	
	
	gtk_widget_show(window);
}	

void on_report_button_rep1_generate_clicked (GtkButton *button, gpointer user_data)
{
	fill_grid_rep1_expenditure();
	fill_grid_rep1_income();
	fill_grid_rep1_total();
}

void on_report_button_rep2_generate_clicked (GtkButton *button, gpointer user_data)
{
	fill_grid_rep2_expenditure();
	fill_grid_rep2_income();
	fill_grid_rep2_total();
}

void on_report_button_rep3_generate_clicked (GtkButton *button, gpointer user_data)
{
	fill_grid_rep3();
}



//================================ REP 1

void fill_grid_rep1_expenditure()
{
	ExGrid *grid = grid_rep1_expenditure;

	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);
	
	
	if (gtk_toggle_button_get_active(cbutton_rep1_accounts))
	{
		gda_sql_builder_select_add_field(builder, "account_id", "operation", NULL); 
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.account_id"));
		//sql_builder_add_group(builder,);
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_categories)) 
	{
		gda_sql_builder_select_add_field(builder, "category_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.category_id"));

	}
	if (gtk_toggle_button_get_active(cbutton_rep1_subcategories))
	{
		gda_sql_builder_select_add_field(builder, "subcategory_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.subcategory_id"));
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_quantity)) 
	{
		gda_sql_builder_select_add_field(builder, "sum(expin.quantity)", NULL, "sum_quantity");
		gda_sql_builder_select_add_field(builder, "unit_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "expin.unit_id"));
	}

	gda_sql_builder_select_add_field(builder, "sum(operation.amount)*-1", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);
	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));


	
	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LT, 
	                                                   gda_sql_builder_add_field_id (builder, "amount", "operation"),
	                                                   gda_sql_builder_add_id (builder, "0"),
	                                                   0
	                                                  );
	
	GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );
	id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);

	
	if (gtk_toggle_button_get_active(cbutton_rep1_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep1_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep1_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));

	
	g_object_unref(db_model);
	g_object_unref(builder);
	
}

void fill_grid_rep1_income()
{
	ExGrid *grid = grid_rep1_income;
	
	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);
	
	
	if (gtk_toggle_button_get_active(cbutton_rep1_accounts))
	{
		gda_sql_builder_select_add_field(builder, "account_id", "operation", NULL); 
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.account_id"));
		//sql_builder_add_group(builder,);
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_categories)) 
	{
		gda_sql_builder_select_add_field(builder, "category_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.category_id"));

	}
	if (gtk_toggle_button_get_active(cbutton_rep1_subcategories))
	{
		gda_sql_builder_select_add_field(builder, "subcategory_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.subcategory_id"));
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_quantity)) 
	{
		gda_sql_builder_select_add_field(builder, "sum(expin.quantity)", NULL, "sum_quantity");
		gda_sql_builder_select_add_field(builder, "unit_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "expin.unit_id"));
	}

	gda_sql_builder_select_add_field(builder, "sum(operation.amount)", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);
	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));
	


	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GT, 
	                                                   gda_sql_builder_add_field_id (builder, "amount", "operation"),
	                                                   gda_sql_builder_add_id (builder, "0"),
	                                                   0
	                                                  );
	GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );
	id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
		
	if (gtk_toggle_button_get_active(cbutton_rep1_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep1_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep1_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, in_category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));

	
	g_object_unref(db_model);
	g_object_unref(builder);
}




void fill_grid_rep1_total()
{
		ExGrid *grid = grid_rep1_total;
	
	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);
	
	
	if (gtk_toggle_button_get_active(cbutton_rep1_accounts))
	{
		gda_sql_builder_select_add_field(builder, "account_id", "operation", NULL); 
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.account_id"));
		//sql_builder_add_group(builder,);
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_categories)) 
	{
		gda_sql_builder_select_add_field(builder, "category_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.category_id"));
 
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_subcategories))
	{
		gda_sql_builder_select_add_field(builder, "subcategory_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "expin.subcategory_id"));
	}
	if (gtk_toggle_button_get_active(cbutton_rep1_quantity)) 
	{
		gda_sql_builder_select_add_field(builder, "sum(expin.quantity)", NULL, "sum_quantity");
		gda_sql_builder_select_add_field(builder, "unit_id", "expin", NULL);
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "expin.unit_id"));
	}

	gda_sql_builder_select_add_field(builder, "sum(operation.amount)", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);
	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));
	


	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );

	if (gtk_toggle_button_get_active(cbutton_rep1_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep1_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);

	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep1_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep1_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	g_free(datefrom);
	g_free(dateto);
	
	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));

	
	g_object_unref(db_model);
	g_object_unref(builder);
}






//============== REP 2 ===============


void fill_grid_rep2_expenditure()
{
	ExGrid *grid = grid_rep2_expenditure;

	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);

	if (gtk_combo_box_get_active(combobox_rep2_group) == 0)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%d.%m.%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.date"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 1)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%m.%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%m.%Y',date(operation.date+1721425))"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 2)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%Y',date(operation.date+1721425))"));
 	}
	
	
	gda_sql_builder_select_add_field(builder, "sum(operation.amount)*-1", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);

	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));


	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LT, 
	                                                   gda_sql_builder_add_field_id (builder, "amount", "operation"),
	                                                   gda_sql_builder_add_id (builder, "0"),
	                                                   0
	                                                  );
	
	GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );
	
	
	id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	
	
	if (gtk_toggle_button_get_active(cbutton_rep2_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep2_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep2_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	g_free(datefrom);
	g_free(dateto);
	
	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));

	
	g_object_unref(db_model);
	g_object_unref(builder);
	
}

void fill_grid_rep2_income()
{

	ExGrid *grid = grid_rep2_income;

	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);

	if (gtk_combo_box_get_active(combobox_rep2_group) == 0)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%d.%m.%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.date"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 1)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%m.%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%m.%Y',date(operation.date+1721425))"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 2)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%Y',operation.date++1721425)", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%Y',date(operation.date+1721425))"));
 	}
	
	
	gda_sql_builder_select_add_field(builder, "sum(operation.amount)", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);

	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));


	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GT, 
	                                                   gda_sql_builder_add_field_id (builder, "amount", "operation"),
	                                                   gda_sql_builder_add_id (builder, "0"),
	                                                   0
	                                                  );
	
	GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );
	
	
	id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	
	
	if (gtk_toggle_button_get_active(cbutton_rep2_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep2_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep2_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	g_free(datefrom);
	g_free(dateto);
	
	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));


	
	g_object_unref(db_model);
	g_object_unref(builder);
	
}

void fill_grid_rep2_total()
{
		ExGrid *grid = grid_rep2_total;

	GdaSqlBuilder *builder = gda_sql_builder_new (GDA_SQL_STATEMENT_SELECT);

	gda_sql_builder_select_add_target(builder, "operation", NULL);
	gda_sql_builder_select_add_target(builder, "expin", NULL);

	if (gtk_combo_box_get_active(combobox_rep2_group) == 0)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%d.%m.%Y',date(operation.date))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "operation.date"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 1)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%m.%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%m.%Y',date(operation.date+1721425))"));
 	}
	else if (gtk_combo_box_get_active(combobox_rep2_group) == 2)
	{
		gda_sql_builder_select_add_field(builder, "strftime('%Y',date(operation.date+1721425))", NULL, "date");
		gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id (builder, "strftime('%Y',date(operation.date+1721425))"));
 	}
	
	
	gda_sql_builder_select_add_field(builder, "sum(operation.amount)", NULL, "sum_amount");
	gda_sql_builder_select_add_field(builder, "currency_id", "operation", NULL);

	gda_sql_builder_select_group_by(builder, gda_sql_builder_add_id(builder, "operation.currency_id"));



	GdaSqlBuilderId id_cond = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, 
	                                                   gda_sql_builder_add_field_id (builder, "id", "operation"),
	                                                   gda_sql_builder_add_field_id (builder, "id", "expin"),
	                                                   0
	                                                  );
	
	
	if (gtk_toggle_button_get_active(cbutton_rep2_dateinterval))
	{
		GdaSqlBuilderId datefunc = gda_sql_builder_add_id (builder, "date");
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_GEQ, datefunc, gda_sql_builder_add_param (builder, "datefrom", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond2 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_LEQ, datefunc, gda_sql_builder_add_param (builder, "dateto", G_TYPE_INT, FALSE), 0);
		GdaSqlBuilderId id_cond_ = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond1, id_cond2, 0);
		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond_, 0);
	}


	if (!gdaui_combo_is_null_selected(combo_rep2_currency))
	{
		GdaSqlBuilderId field = gda_sql_builder_add_field_id (builder, "currency_id", "operation");
		
		GdaSqlBuilderId id_cond1 = gda_sql_builder_add_cond(builder, GDA_SQL_OPERATOR_TYPE_EQ, field, gda_sql_builder_add_param (builder, "currency", G_TYPE_INT, FALSE), 0);

		id_cond = gda_sql_builder_add_cond (builder, GDA_SQL_OPERATOR_TYPE_AND, id_cond, id_cond1, 0);
	}
	
	gda_sql_builder_set_where(builder, id_cond);

	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep2_to));
	GValue *currency = ex_combo_get_current_row_value(combo_rep2_currency, 0);
	
	GdaDataModel *db_model = db_exec_select_sql(gda_statement_to_sql(gda_sql_builder_get_statement(builder, NULL), NULL, NULL),
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "currency", currency,
	                                           NULL
	                                          );

	g_free(datefrom);
	g_free(dateto);
	
	if (!db_model) return;

	
	gint col_account     = gda_data_model_get_column_index(db_model,"account_id");
	gint col_category    = gda_data_model_get_column_index(db_model,"category_id");
	gint col_subcategory = gda_data_model_get_column_index(db_model,"subcategory_id");
	gint col_quantity    = gda_data_model_get_column_index(db_model,"sum_quantity");
	gint col_unit        = gda_data_model_get_column_index(db_model,"unit_id");
	gint col_amount      = gda_data_model_get_column_index(db_model,"sum_amount");
	gint col_currency    = gda_data_model_get_column_index(db_model,"currency_id");

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, col_account, account_model, 0);
	ex_grid_lookup_field (grid, col_category, category_model, 0);
	ex_grid_lookup_field (grid, col_subcategory, subcategory_model, 0);
	ex_grid_lookup_field (grid, col_unit, unit_model, 0);
	ex_grid_lookup_field (grid, col_currency, currency_model, 0);

	ex_grid_column_set_format(grid, col_amount, EX_GRID_COL_FORMAT_MONEY);
	
	ex_grid_column_set_title(grid, col_account,_ ("Account"));
	ex_grid_column_set_title(grid, col_category, _("Category"));
	ex_grid_column_set_title(grid, col_subcategory, _("Subcategory"));
	ex_grid_column_set_title(grid, col_quantity, _("Qty."));
	ex_grid_column_set_title(grid, col_unit, _("Unit"));
	ex_grid_column_set_title(grid, col_amount, _("Amount"));
	ex_grid_column_set_title(grid, col_currency, _("Currency"));


	
	g_object_unref(db_model);
	g_object_unref(builder);
}


//============== REP 3 ===============


void fill_grid_rep3()
{

	ExGrid *grid = grid_rep3;

	enum {
		DATE_COL,
		ACCOUNT_COL,
		CURRENCY_COL,
		AMOUNT_COL
	};

	GValue *datef   = NULL;
	GValue *datef2  = NULL;
	GValue *account = NULL;
	
	gint pos = gtk_combo_box_get_active(combobox_rep3_group);
	switch (pos)
	{
		case 0:
			datef = ex_value_new_string ("%d.%m.%Y");
			datef2 = ex_value_new_string ("%Y.%m.%d");
			break;
		case 1:
			datef = ex_value_new_string ("%m.%Y");
			datef2 = ex_value_new_string ("%Y.%m");
			break;
		case 2:
			datef = ex_value_new_string ("%Y");
			datef2 = ex_value_new_string ("%Y");
			break;
	}

	GString *sql = g_string_new("SELECT strftime(##datef::string,date(date+1721425)), account_id, currency_id, \
                                        (SELECT sum(z.amount) FROM operation z  WHERE strftime(##datef2::string,date(z.date+1721425))<=strftime(##datef2::string,date(a.date+1721425)) AND z.currency_id=a.currency_id AND z.account_id=a.account_id) \
	                             FROM operation a WHERE 1=1"
	                           );

//								 WHERE currency_id IN (SELECT x.currency_id FROM account_startup x WHERE x.account_id=a.account_id AND x.active_currency=1)"

	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(cbutton_rep3_dateinterval))) 
	{
		g_string_append(sql," AND a.date>=##datefrom::gint");
		g_string_append(sql," AND a.date<=##dateto::gint");
	}

	if (!gdaui_combo_is_null_selected(combo_rep3_account))
	{
		g_string_append(sql," AND a.account_id>=##account::gint");
		account = ex_combo_get_current_row_value(combo_rep3_account, 0);
	}

	g_string_append(sql, " GROUP BY strftime(##datef2::string,date(date+1721425)), account_id, currency_id");

	
	GValue *datefrom = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep3_from));
	GValue *dateto   = ex_value_new_int(gtk_dateentry_get_date(dateedit_rep3_to));
	
	GdaDataModel *db_model = db_exec_select_sql(sql->str,
	                                           "datefrom", datefrom,
	                                           "dateto", dateto,
	                                           "datef", datef,
	                                           "datef2", datef2,
	                                           "account", account,
	                                            NULL
	                                          );

	g_free(datefrom);
	g_free(dateto);
	g_string_free(sql,TRUE);
	
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid), db_model);

	ex_grid_lookup_field (grid, ACCOUNT_COL, account_model, 0);
	ex_grid_lookup_field (grid, CURRENCY_COL, currency_model, 0);

	ex_grid_column_set_format(grid, AMOUNT_COL, EX_GRID_COL_FORMAT_MONEY);

	ex_grid_column_set_title(grid, DATE_COL,_ ("Date"));
	ex_grid_column_set_title(grid, ACCOUNT_COL,_ ("Account"));
	ex_grid_column_set_title(grid, AMOUNT_COL, _("Amount"));
	ex_grid_column_set_title(grid, CURRENCY_COL, _("Currency"));


	
	g_object_unref(db_model);
	
}

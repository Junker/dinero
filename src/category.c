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

#include <glib/gprintf.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <libgda-ui/libgda-ui.h>

#include "db.h"
#include "common.h"
#include "category.h"
#include "actions.h"
#include "mainform.h"
#include "sutil.h"
#include "ex-grid.h"

static GtkWidget *window;
static ExGrid *grid_in_category,
              *grid_in_subcategory,
              *grid_out_category,
              *grid_out_subcategory,
              *grid_unit,*grid_debtor,
              *grid_creditor;

static GtkButton *button_expend_category_add,
                 *button_expend_category_edit,
                 *button_expend_category_del,
                 *button_expend_subcategory_add,
                 *button_expend_subcategory_edit,
                 *button_expend_subcategory_del,
                 *button_income_category_add,
                 *button_income_category_edit,
                 *button_income_category_del,
                 *button_income_subcategory_add,
                 *button_income_subcategory_edit,
                 *button_income_subcategory_del,
                 *button_unit_add,
                 *button_unit_edit,
                 *button_unit_del,
                 *button_debtor_add,
                 *button_debtor_edit,
                 *button_debtor_del;


static void fill_grid_in_category(void);
static void fill_grid_in_subcategory(GValue *id);
static void fill_grid_out_category(void);
static void fill_grid_out_subcategory(GValue *id);
static void fill_grid_unit();
static void fill_grid_debtor();
static void fill_grid_creditor();	 

static void on_grid_out_category_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_in_category_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_out_subcategory_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_in_subcategory_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_unit_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);
static void on_grid_debtor_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data);


void show_category_window (void)
{
	
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "category.glade");
	gtk_builder_connect_signals (gbuilder,NULL);

	window = GTK_WIDGET(gtk_builder_get_object (gbuilder, "window"));

	
	//Buttons
	button_expend_category_add     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_category_add"));
	button_expend_category_edit    = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_category_edit"));
	button_expend_category_del     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_category_del"));
	button_expend_subcategory_add  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_subcategory_add"));
	button_expend_subcategory_edit = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_subcategory_edit"));
	button_expend_subcategory_del  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_expend_subcategory_del"));
	button_income_category_add     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_category_add"));
	button_income_category_edit    = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_category_edit"));
	button_income_category_del     = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_category_del"));
	button_income_subcategory_add  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_subcategory_add"));
	button_income_subcategory_edit = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_subcategory_edit"));
	button_income_subcategory_del  = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_income_subcategory_del"));
	button_unit_add                = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_unit_add"));
	button_unit_edit               = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_unit_edit"));
	button_unit_del                = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_unit_del"));
	button_debtor_add              = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debtor_add"));
	button_debtor_edit             = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debtor_edit"));
	button_debtor_del              = GTK_BUTTON(gtk_builder_get_object (gbuilder, "button_debtor_del"));
	
	
	GtkContainer *scrolledwindow_grid_out_category    = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_out_category"));
	GtkContainer *scrolledwindow_grid_out_subcategory = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_out_subcategory"));
	GtkContainer *scrolledwindow_grid_in_category     = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_in_category"));
	GtkContainer *scrolledwindow_grid_in_subcategory  = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_in_subcategory"));
	GtkContainer *scrolledwindow_grid_unit            = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_unit"));
	GtkContainer *scrolledwindow_grid_debtor          = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_debtor"));
	GtkContainer *scrolledwindow_grid_creditor        = GTK_CONTAINER(gtk_builder_get_object (gbuilder, "scrolledwindow_grid_creditor"));


	grid_out_category    = create_grid (scrolledwindow_grid_out_category);
	grid_out_subcategory = create_grid (scrolledwindow_grid_out_subcategory);
	grid_in_category     = create_grid (scrolledwindow_grid_in_category);
	grid_in_subcategory  = create_grid (scrolledwindow_grid_in_subcategory);
	grid_unit            = create_grid (scrolledwindow_grid_unit);
	grid_debtor          = create_grid (scrolledwindow_grid_debtor);
	grid_creditor        = create_grid (scrolledwindow_grid_creditor);
	
	g_signal_connect (G_OBJECT(grid_out_category), "selection-changed", G_CALLBACK (on_grid_out_category_change_selection), NULL);
	g_signal_connect (G_OBJECT(grid_in_category), "selection-changed", G_CALLBACK (on_grid_in_category_change_selection), NULL);
	g_signal_connect (G_OBJECT(grid_out_subcategory), "selection-changed", G_CALLBACK (on_grid_out_subcategory_change_selection), NULL);
	g_signal_connect (G_OBJECT(grid_in_subcategory), "selection-changed", G_CALLBACK (on_grid_in_subcategory_change_selection), NULL);
	g_signal_connect (G_OBJECT(grid_unit), "selection-changed", G_CALLBACK (on_grid_unit_change_selection), NULL);
	g_signal_connect (G_OBJECT(grid_debtor), "selection-changed", G_CALLBACK (on_grid_debtor_change_selection), NULL);

	fill_grid_out_category();
	fill_grid_out_subcategory(NULL);
	fill_grid_in_category();
	fill_grid_in_subcategory(NULL);
	fill_grid_unit();	
	fill_grid_debtor();
	fill_grid_creditor();

	gtk_widget_show(window);
}

void fill_grid_out_category(void)
{
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id, name FROM category WHERE type=1 ORDER BY name", NULL);

	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_out_category), db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_out_category), 0, FALSE);

	ex_grid_column_set_title(grid_out_category, 1, _("Category"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_category_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_category_del), FALSE);

	g_object_unref(db_model);
}

void fill_grid_out_subcategory(GValue *id)
{
	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", id, NULL);
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_out_subcategory),db_model);

	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_out_subcategory), 0, FALSE);

	ex_grid_column_set_title(grid_out_subcategory, 1, _("Subcategory"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_subcategory_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_subcategory_del), FALSE);
	
	g_object_unref(db_model);	
}

void fill_grid_in_category(void)
{
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id,name FROM category WHERE type=2 ORDER BY name",NULL);
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_in_category),db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_in_category), 0, FALSE);

	ex_grid_column_set_title(grid_in_category, 1, _("Category"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_income_category_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_category_del), FALSE);
	
	g_object_unref(db_model);
}

void fill_grid_in_subcategory(GValue *id)
{
	gchar *sql = "SELECT id,name FROM subcategory WHERE category_id=##id::gint::NULL ORDER BY name";
	GdaDataModel *db_model = db_exec_select_sql (sql, "id", id, NULL);
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_in_subcategory), db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_in_subcategory), 0, FALSE);

	ex_grid_column_set_title(grid_in_subcategory, 1, _("Subcategory"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_income_subcategory_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_subcategory_del), FALSE);
	
	g_object_unref(db_model);
}

void fill_grid_unit(void)
{
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id,name FROM unit ORDER BY name",NULL);
	if (!db_model) return;

	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_unit),db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_unit), 0, FALSE);

	ex_grid_column_set_title(grid_unit, 1, _("Unit"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_unit_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_unit_del), FALSE);
	
	g_object_unref(db_model);
}

void fill_grid_debtor(void)
{
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id,name FROM person ORDER BY name",NULL);
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_debtor), db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_debtor), 0, FALSE);

	ex_grid_column_set_title(grid_debtor, 1, _("Debtor"));

	gtk_widget_set_sensitive(GTK_WIDGET(button_debtor_edit), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debtor_del), FALSE);
	
	g_object_unref(db_model);
}

void fill_grid_creditor(void)
{
	
	GdaDataModel *db_model = db_exec_select_sql ("SELECT id,name FROM person ORDER BY name",NULL);
	if (!db_model) return;
	
	gdaui_data_selector_set_model(GDAUI_DATA_SELECTOR(grid_creditor), db_model);
	gdaui_data_selector_set_column_visible(GDAUI_DATA_SELECTOR(grid_creditor), 0, FALSE);

	ex_grid_column_set_title(grid_creditor, 1, _("Creditor"));
	
	g_object_unref(db_model);
}

void on_grid_out_category_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	fill_grid_out_subcategory(ex_grid_get_selected_row_value(EX_GRID(dbrawgrid), 0));

	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_category_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_category_del), TRUE);
	
}

void on_grid_in_category_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	fill_grid_in_subcategory(ex_grid_get_selected_row_value(EX_GRID(dbrawgrid),0));

	gtk_widget_set_sensitive(GTK_WIDGET(button_income_category_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_category_del), TRUE);	
}

void on_grid_out_subcategory_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_subcategory_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_expend_subcategory_del), TRUE);
	
}
void on_grid_in_subcategory_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_subcategory_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_income_subcategory_del), TRUE);
	
}
void on_grid_unit_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_unit_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_unit_del), TRUE);
	
}

void on_grid_debtor_change_selection (GdauiRawGrid *dbrawgrid, gboolean arg1, gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(button_debtor_edit), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button_debtor_del), TRUE);
}


void on_out_category_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter category name:"), NULL);
	if (strlen(name_str) == 0) return;


	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO category(name,type) VALUES(##name::string, 1)";
	db_exec_sql(sql, "name", name, NULL);

	fill_grid_out_category();
	refresh_category_model();
	refresh_out_category_model();
	refresh_out_category_combos();

	g_free(name);
	g_free(name_str);
}

void on_out_subcategory_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter subcategory name:"), NULL);
	if (strlen(name_str) == 0) return;

	GValue *category = ex_grid_get_selected_row_value(grid_out_category, 0);

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO subcategory(name, category_id) VALUES(##name::string, ##category::gint)";
	db_exec_sql(sql, "name", name, "category", category, NULL);
	
	fill_grid_out_subcategory(category);
	refresh_subcategory_model();
	refresh_out_category_combos();

	g_free(name);
	g_free(name_str);	

}

void on_in_category_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter category name:"), NULL);
	if (strlen(name_str) == 0) return;
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO category(name,type) VALUES(##name::string, 2)";
	db_exec_sql(sql, "name", name, NULL);

	fill_grid_in_category();
	refresh_category_model();
	refresh_in_category_model();
	refresh_in_category_combos();

	g_free(name);
	g_free(name_str);	
}

void on_in_subcategory_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter subcategory name:"),NULL);
	if (strlen(name_str) == 0) return;

	GValue *category = ex_grid_get_selected_row_value(grid_in_category, 0);
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO subcategory(name, category_id) VALUES(##name::string, ##category::gint)";
	db_exec_sql(sql, "name", name, "category", category, NULL);
	
	fill_grid_in_subcategory(category);
	refresh_subcategory_model();
	refresh_in_category_combos();

	g_free(name);
	g_free(name_str);	
}

void on_out_category_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_out_category, 0);	
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_out_category, 1));
	
	gchar *name_str = show_input_dialog(_("Enter category name:"), old_name);
	
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE category SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);
	
	fill_grid_out_category();
	refresh_category_model();
	refresh_out_category_model();
	refresh_out_category_combos();

	fill_grid_expenditure();
	fill_grid_plan_expenditure();

	
	g_free(name);
	g_free(name_str);	
}

void on_out_subcategory_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_out_subcategory, 0);
	GValue *category = ex_grid_get_selected_row_value(grid_out_category, 0);
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_out_subcategory, 1));

	gchar *name_str = show_input_dialog(_("Enter subcategory name:"), old_name);
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE subcategory SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_out_subcategory(category);
	refresh_subcategory_model();
	refresh_out_category_combos();

	fill_grid_expenditure();
	fill_grid_plan_expenditure();

	
	g_free(name);
	g_free(name_str);	
}

void on_in_category_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_in_category, 0);
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_in_category, 1));
	
	gchar *name_str = show_input_dialog(_("Enter category name:"), old_name);
	
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE category SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_in_category();
	refresh_category_model();
	refresh_in_category_model();
	refresh_in_category_combos();

	fill_grid_income();
	fill_grid_plan_income();

	g_free(name);
	g_free(name_str);	
}

void on_in_subcategory_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_in_subcategory, 0);	
	GValue *category = ex_grid_get_selected_row_value(grid_in_category, 0);
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_in_subcategory, 1));

	gchar *name_str = show_input_dialog(_("Enter subcategory name:"), old_name);
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE subcategory SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_in_subcategory(category);
	refresh_subcategory_model();
	refresh_in_category_combos();

	fill_grid_income();
	fill_grid_plan_income();
	
	g_free(name);
	g_free(name_str);	
}

void on_out_category_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you delete a category, field 'category' will be cleared in all the operations associated with this category. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid_out_category, 0);	
	gchar *sql = "DELETE FROM category WHERE id=##id::gint";
	
	db_exec_sql(sql, "id", id, NULL);

	fill_grid_out_category();
	refresh_category_model();
	refresh_out_category_model();
	refresh_subcategory_model();
	refresh_out_category_combos();

	fill_grid_expenditure();
	fill_grid_plan_expenditure();

	
}

void on_out_subcategory_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you delete a category, field 'category' will be cleared in all the operations associated with this category. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid_out_subcategory, 0);	
	GValue *category = ex_grid_get_selected_row_value(grid_out_category, 0);

	gchar *sql = "DELETE FROM subcategory WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);
	
	fill_grid_out_subcategory(category);
	refresh_subcategory_model();
	refresh_out_category_combos();

	fill_grid_expenditure();
	fill_grid_plan_expenditure();

}

void on_in_category_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you delete a category, field 'category' will be cleared in all the operations associated with this category. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid_in_category, 0);	

	gchar *sql = "DELETE FROM category WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);
	
	fill_grid_in_category();
	refresh_category_model();
	refresh_in_category_model();
	refresh_subcategory_model();
	refresh_in_category_combos();

	fill_grid_income();
	fill_grid_plan_income();

}

void on_in_subcategory_button_del_clicked (GtkButton *button, gpointer user_data)
{
	gint response = show_warning_dialog(_("If you delete a category, field 'category' will be cleared in all the operations associated with this category. Are you sure you want to delete?"),GTK_WINDOW(window));
	if (response  != GTK_RESPONSE_YES) return;
	
	GValue *id = ex_grid_get_selected_row_value(grid_in_subcategory, 0);	
	GValue *category = ex_grid_get_selected_row_value(grid_in_category, 0);

	gchar *sql = "DELETE FROM subcategory WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);

	fill_grid_in_subcategory(category);
	refresh_subcategory_model();
	refresh_in_category_combos();

	fill_grid_income();
	fill_grid_plan_income();	

}


void on_unit_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter unit name:"), NULL);
	if (strlen(name_str) == 0) return;

	gchar *sql = "INSERT INTO unit(name) VALUES(##name::string)";

	GValue *name = ex_value_new_string(name_str);
	db_exec_sql(sql, "name", name, NULL);

	fill_grid_unit();
	refresh_unit_model();

	g_free(name);
	g_free(name_str);	
}

void on_unit_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_unit, 0);	
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_unit, 1));
	gchar *name_str = show_input_dialog(_("Enter unit name:"), old_name);
	if (strlen(name_str) == 0) return;
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE unit SET name=##name::string where id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_unit();
	refresh_unit_model();	

	fill_grid_expenditure();
	fill_grid_income();
	fill_grid_plan_expenditure();
	fill_grid_plan_income();
	
	g_free(name);
	g_free(name_str);	
}

void on_unit_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_unit, 0);
	
	gchar *sql = "DELETE FROM unit WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);
	
	fill_grid_unit();
	refresh_unit_model();
	
	fill_grid_expenditure();
	fill_grid_income();
	fill_grid_plan_expenditure();
	fill_grid_plan_income();
	
}


void on_debtor_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter debtor name:"), NULL);
	if (strlen(name_str) == 0) return;

	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO person(name) values(##name::string)";
	db_exec_sql(sql, "name", name, NULL);

	fill_grid_debtor();
	refresh_person_model();	

	g_free(name);
	g_free(name_str);	
}

void on_debtor_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_debtor, 0);	
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_debtor, 1));
	gchar *name_str = show_input_dialog(_("Enter debtor name:"), old_name);

	if (strlen(name_str) == 0) return;
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE person SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_debtor();
	refresh_person_model();	
	fill_grid_credit();
	fill_grid_debt();

	g_free(name_str);
	g_free(name);
}

void on_debtor_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_debtor, 0);
	
	gchar *sql = "DELETE FROM person WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);
	
	fill_grid_debtor();
	refresh_person_model();	
	fill_grid_credit();
	fill_grid_debt();
	
}

void on_creditor_button_add_clicked (GtkButton *button, gpointer user_data)
{
	gchar *name_str = show_input_dialog(_("Enter creditor name:"), NULL);
	if (strlen(name_str) == 0) return;
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "INSERT INTO person(name) VALUES(##name::string)";
	db_exec_sql(sql, "name", name, NULL);
	
	fill_grid_creditor();
	refresh_person_model();	

	g_free(name);
	g_free(name_str);	
}

void on_creditor_button_edit_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = ex_grid_get_selected_row_value(grid_creditor, 0);	
	
	gchar *old_name = g_value_get_string(ex_grid_get_selected_row_value(grid_creditor, 1));
	gchar *name_str = show_input_dialog("Enter creditor name:", old_name);
	if (strlen(name_str) == 0) return;
	
	GValue *name = ex_value_new_string(name_str);
	gchar *sql = "UPDATE person SET name=##name::string WHERE id=##id::gint";
	db_exec_sql(sql, "name", name, "id", id, NULL);

	fill_grid_creditor();
	refresh_person_model();	
	
	g_free(name);
	g_free(name_str);	
}

void on_creditor_button_del_clicked (GtkButton *button, gpointer user_data)
{
	GValue *id = g_value_get_int(ex_grid_get_selected_row_value(grid_creditor, 0));	
	
	gchar *sql = "DELETE FROM person WHERE id=##id::gint";
	db_exec_sql(sql, "id", id, NULL);
	
	fill_grid_creditor();
	refresh_person_model();	
}



/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * homefinances_gnome
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
 * 
 * homefinances_gnome is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * homefinances_gnome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libgda-ui/libgda-ui.h>

#include "ex-grid.h"

static void ex_grid_populate_popup (ExGrid *grid, GtkMenu *arg1, gpointer user_data);

G_DEFINE_TYPE (ExGrid, ex_grid, GDAUI_TYPE_RAW_GRID);

static void ex_grid_init (ExGrid *object)
{
	g_signal_connect (G_OBJECT(object), "populate-popup", G_CALLBACK (ex_grid_populate_popup), NULL);
//	g_object_set(G_OBJECT(object), "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

	GtkTreeSelection *select = gtk_tree_view_get_selection (GTK_TREE_VIEW (object));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	g_object_set (G_OBJECT (object), "has-tooltip", FALSE, NULL);
}

static void ex_grid_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (ex_grid_parent_class)->finalize (object);
}

static void ex_grid_class_init (ExGridClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GdauiRawGridClass* parent_class = GDAUI_RAW_GRID_CLASS (klass);

	object_class->finalize = ex_grid_finalize;
}

ExGrid* ex_grid_new ()
{
	ExGrid *grid = EX_GRID(g_object_new (EX_TYPE_GRID, NULL));

	return grid;
}

//Get value from selected row by column 
GValue* ex_grid_get_selected_row_value(ExGrid *grid, const guint n_column)
{
	GdaDataModelIter *modeliter = gdaui_data_selector_get_data_set(GDAUI_DATA_SELECTOR(grid));
	if (!gda_data_model_iter_is_valid(modeliter)) return NULL;
	
	GValue *val = gda_data_model_iter_get_value_at(modeliter, n_column);
	return val;
}

//Make Lookup field
void ex_grid_lookup_field (ExGrid *grid, gint n_col, GdaDataModel *model, gint model_col)
{
	GdaDataModelIter *iter;
	GdaHolder *holder;

	if (n_col == -1) return;

	iter = gdaui_data_selector_get_data_set(grid);
	holder = gda_data_model_iter_get_holder_for_field (iter, n_col);

	ex_grid_set_column_width(grid, n_col, 500);
	
	g_assert (gda_holder_set_source_model (holder, model, model_col, NULL));

}


GtkCellRenderer* ex_grid_get_column_text_renderer(ExGrid *grid, guint col)
{
	GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(grid), col);

	GList *renderer_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(column));


	renderer_list = g_list_first(renderer_list);
	while (renderer_list)
	{
		gchar *zz = G_OBJECT_TYPE_NAME(renderer_list -> data);
		if (strcmp(G_OBJECT_TYPE_NAME(renderer_list -> data),"GdauiDataCellRendererTextual") == 0)
			return renderer_list -> data;
		renderer_list = g_list_next(renderer_list);
	};

	g_list_free(renderer_list);
	
	return NULL;
}

GtkCellRenderer* ex_grid_get_column_combo_renderer(ExGrid *grid, guint col)
{
	GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(grid), col);

	GList *renderer_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(column));


	renderer_list = g_list_first(renderer_list);
	while (renderer_list)
	{
		gchar *zz = G_OBJECT_TYPE_NAME(renderer_list -> data);
		if (strcmp(G_OBJECT_TYPE_NAME(renderer_list -> data),"GdauiDataCellRendererCombo") == 0)
			return renderer_list -> data;
		renderer_list = g_list_next(renderer_list);
	};

	g_list_free(renderer_list);
	
	return NULL;
}



void ex_grid_column_set_format(ExGrid *grid, guint col, ExGridColumnFormat format)
{
	if (format == EX_GRID_COL_FORMAT_MONEY)
	{
/*		GtkCellRenderer *renderer = ex_grid_get_column_text_renderer(grid, col);

		g_object_set(renderer, "options","NB_DECIMALS=2",NULL);
*/
		GdaDataModelIter *iter = gdaui_data_selector_get_data_set (GDAUI_DATA_SELECTOR (grid));
		GdaHolder *param = gda_data_model_iter_get_holder_for_field (iter, col);
	
		GValue *value = gda_value_new_from_string ("number:NB_DECIMALS=2", G_TYPE_STRING);
		gda_holder_set_attribute_static (param, GDAUI_ATTRIBUTE_PLUGIN, value);
		
	}
	else if (format == EX_GRID_COL_FORMAT_BOOL)
	{
		GdaDataModelIter *iter = gdaui_data_selector_get_data_set (GDAUI_DATA_SELECTOR (grid));
		GdaHolder *param = gda_data_model_iter_get_holder_for_field (iter, col);

		GValue *value = gda_value_new_from_string ("boolean", G_TYPE_STRING);
		gda_holder_set_attribute_static (param, GDAUI_ATTRIBUTE_PLUGIN, value);
		
	}
	
	
}	

void ex_grid_column_set_title(ExGrid *grid, guint col, gchar *title)
{
	GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(grid), col);
	gtk_tree_view_column_set_title(column, title);
}

void ex_grid_set_columns_resizable(ExGrid *grid, gboolean val) 
{
	GList *columns;
	gint i;
	
	columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(grid));
	
	for (i=1;i<=g_list_length(columns);i++)
		if GTK_IS_TREE_VIEW_COLUMN(g_list_nth_data(columns,i))  
			gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(g_list_nth_data(columns,i)), val);
			}

void ex_grid_set_columns_reordable(ExGrid *grid, gboolean val) 
{
	GList *columns;
	gint i;
	
	columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(grid));
	
	for (i=1;i<=g_list_length(columns);i++)
		if GTK_IS_TREE_VIEW_COLUMN(g_list_nth_data(columns,i))  
			gtk_tree_view_column_set_reorderable(GTK_TREE_VIEW_COLUMN(g_list_nth_data(columns,i)), val);
}

void ex_grid_populate_popup (ExGrid *grid, GtkMenu *arg1, gpointer user_data)
{
	gtk_container_forall(arg1, gtk_widget_destroy, NULL);
//	gtk_menu_append(arg1,gtk_menu_item_new_with_label("adasdasd"));
}


void ex_grid_set_column_data_cb (ExGrid *grid, gint column, GtkTreeCellDataFunc func)
{
	GtkCellRenderer *renderer = ex_grid_get_column_text_renderer(grid, column);
	gtk_cell_layout_set_cell_data_func(gtk_tree_view_get_column(GTK_TREE_VIEW(grid), column), renderer, func, NULL, NULL);
}

void ex_grid_set_column_width(ExGrid *grid, gint column, guint width)
{
	GtkTreeViewColumn *treeviewcolumn = gtk_tree_view_get_column(GTK_TREE_VIEW(grid), column);

/*    treeviewcolumn->resized_width = width;
    treeviewcolumn->use_resized_width = TRUE;

	g_object_set(G_OBJECT(treeviewcolumn), 
                 "expand", FALSE,       // Needed for use_resized_width to be effective.
                 NULL);
*/

}

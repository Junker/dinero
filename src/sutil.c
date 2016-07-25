/*
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
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

#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "sutil.h"




#define GLADE_PATH PACKAGE_DATA_DIR"/ui/"

GValue* ex_value_new_string(const gchar *string)
{
	GValue *value = g_try_new0(GValue,1);
	
	g_value_init(value, G_TYPE_STRING);
	g_value_set_string (value, string);

	return value;
}

GValue* ex_value_new_int(const gint num)
{
	GValue *value = g_try_new0(GValue,1);
	
	g_value_init(value, G_TYPE_INT);
	g_value_set_int (value, num);

	return value;
}


GValue* ex_value_new_double(const gdouble val)
{
	GValue *value = g_try_new0(GValue,1);
	
	g_value_init(value, G_TYPE_DOUBLE);
	g_value_set_double (value, val);

	return value;
}

GValue* ex_value_new_boolean(const gboolean val)
{
	GValue *value = g_try_new0(GValue,1);
	
	g_value_init(value, G_TYPE_BOOLEAN);
	g_value_set_boolean (value, val);

	return value;
}

double ex_value_get_double(const GValue *value)
{
	gdouble res = 0;
	
	if (G_VALUE_TYPE(value) == G_TYPE_DOUBLE)
	{
		res = g_value_get_double(value);
	}
	else if (G_VALUE_TYPE(value) == G_TYPE_INT)
	{
		res = (gdouble) g_value_get_int(value);
	}

	return res;
	
}

gchar* ex_textview_get_text(GtkTextView *textview)
{
	GtkTextIter start_iter, end_iter;
	GtkTextBuffer *buf = gtk_text_view_get_buffer(textview);
	gtk_text_buffer_get_start_iter(buf, &start_iter);
	gtk_text_buffer_get_end_iter(buf, &end_iter);
	
	return gtk_text_buffer_get_text(buf,&start_iter, &end_iter,FALSE);
}

void ex_textview_set_text(GtkTextView *textview, gchar *text)
{
	if (!text) return;
	
	GtkTextBuffer *buf = gtk_text_view_get_buffer(textview);
	gtk_text_buffer_set_text(buf,text,strlen(text));
}

GList *ex_tree_view_get_rows(GtkTreeView *treeview)
{
	GList *iter_list = NULL;

	GtkTreeSelection *sel = gtk_tree_view_get_selection(treeview);

	if (gtk_tree_selection_count_selected_rows(sel) == 0)
      return NULL;
	
	GList *path_list = gtk_tree_selection_get_selected_rows(sel, NULL);

	
	while(path_list) 
	{
		GtkTreeIter iter;
		GtkTreeIter *it;

		gtk_tree_model_get_iter(gtk_tree_view_get_model (treeview), &iter, path_list -> data);

		it = gtk_tree_iter_copy (&iter);		
		iter_list = g_list_append(iter_list, it);
		path_list = path_list -> next;
	}

	g_list_foreach (path_list, (GFunc) gtk_tree_path_free, NULL);	
	g_list_free(path_list);	
	
	return iter_list;

}


gint ex_data_model_get_row_by_id(GdaDataModel *db_model, const gint id)
{
	GSList *list = NULL; 
	GValue idval = {0};
	g_value_init (&idval, G_TYPE_INT);
	g_value_set_int(&idval, id);
	list = g_slist_append(list, &idval);
	gint col[1] = {0};
	
	gint row_idx = gda_data_model_get_row_from_values(db_model, list, col);

	g_slist_free(list);
		
	return row_idx;
}

void ex_form_lookup_field (GdauiRawForm *form, gint n_col, GdaDataModel *model, gint model_col)
{
	GdaDataModelIter *iter;
	GdaHolder *holder;

	if (n_col == -1) return;

	iter = gdaui_data_selector_get_data_set(GDAUI_DATA_SELECTOR(form));
	holder = gda_data_model_iter_get_holder_for_field (iter, n_col);
	
	g_assert (gda_holder_set_source_model (holder, model, model_col, NULL));
}


const GValue* ex_combo_get_current_row_value(GdauiCombo *combo, const guint n_column)
{
	const GValue *value;
	//If i reassign a new model to Combo, then gdaui_data_selector_get_data_set return wrong iter
/*	GdaDataModelIter *iter = gdaui_data_selector_get_data_set(GDAUI_DATA_SELECTOR(combo));
	if (!iter) return NULL;

	g_debug("ITER EXISTS! ROW:%i",gda_data_model_iter_get_row(iter));
	
	
	value = gda_data_model_iter_get_value_at(iter, n_column);

	return value;
*/

	//DIRTY HACK!

	GError *error = NULL;
	
	gint aindex = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));

	if (aindex == -1) return NULL;
	
	GdaDataModel *db_model = gdaui_data_selector_get_model(GDAUI_DATA_SELECTOR(combo));

	
	if (gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(combo)), NULL ) != gda_data_model_get_n_rows(db_model))
		aindex--;


	value = gda_data_model_get_value_at(db_model, n_column, aindex, &error);
	if (error) return NULL;

	return value;

}



gint ex_combo_get_row_by_id(GdauiCombo *combo, const gint id)
{
	GdaDataModel *db_model = gdaui_data_selector_get_model(GDAUI_DATA_SELECTOR(combo));

	return ex_data_model_get_row_by_id(db_model, id);
}



void ex_builder_load_file(GtkBuilder *builder, const gchar *filename)
{
	gchar *filepath = g_strconcat(GLADE_PATH, filename, NULL);

	if (g_file_test(filepath, G_FILE_TEST_EXISTS))
	{
		gtk_builder_add_from_file (builder, filepath, NULL);
	}
	else
	{
		g_error("%s not found", filepath);
	}
	
	g_free(filepath);
}

GValue * ex_date_get_formated_date_value(GDate *date)
{
	gchar date_str[50];

	g_date_strftime (date_str, 50, "%d.%m.%Y", date);
	
	return ex_value_new_string (date_str);
}



guint32 get_current_date()
{
	GDate *date = g_date_new ();

	g_date_set_time_t(date, time(NULL));


	guint32 julian = g_date_get_julian (date);

	g_date_free(date);
	
	return julian;
}

void str_replace_character(char *str, char *replace, char *with)
{
	char *pch;
    do
	{
        pch = strstr (str, replace);
        if(pch != NULL)
        strncpy (pch, with, 1);
    }
    while(pch != NULL);
}

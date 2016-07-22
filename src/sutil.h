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


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libgda-ui/libgda-ui.h>

GValue* ex_value_new_string(const gchar *string);
GValue* ex_value_new_int(const gint num);
GValue* ex_value_new_double(const gdouble val);
GValue* ex_value_new_boolean(const gboolean val);
double ex_value_get_double(const GValue *value);
gchar* ex_textview_get_text(GtkTextView *textview);
void ex_textview_set_text(GtkTextView *textview, gchar *text);
GList *ex_tree_view_get_rows(GtkTreeView *treeview);
void ex_builder_load_file(GtkBuilder *builder, const gchar *filename);
GValue * ex_date_get_date_value(GDate *date);
time_t ex_date_get_time_t (GDate *date);
const GValue* ex_combo_get_current_row_value(GdauiCombo *combo, const guint n_column);
gint ex_combo_get_row_by_id(GdauiCombo *combo, const gint id);
gint ex_data_model_get_row_by_id(GdaDataModel *db_model, const gint id);
void ex_form_lookup_field (GdauiRawForm *form, gint n_col, GdaDataModel *model, gint model_col);

guint32 get_current_date();
void str_replace_character(char *str, char *replace, char *with);
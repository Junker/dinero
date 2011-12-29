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

#ifndef _EX_GRID_H_
#define _EX_GRID_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define EX_TYPE_GRID             (ex_grid_get_type ())
#define EX_GRID(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EX_TYPE_GRID, ExGrid))
#define EX_GRID_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EX_TYPE_GRID, ExGridClass))
#define EX_IS_GRID(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EX_TYPE_GRID))
#define EX_IS_GRID_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EX_TYPE_GRID))
#define EX_GRID_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EX_TYPE_GRID, ExGridClass))

typedef struct _ExGridClass ExGridClass;
typedef struct _ExGrid ExGrid;

struct _ExGridClass
{
	GdauiRawGridClass parent_class;
};

struct _ExGrid
{
	GdauiRawGrid parent_instance;
};

GType ex_grid_get_type (void) G_GNUC_CONST;

G_END_DECLS

typedef enum {
	EX_GRID_COL_FORMAT_MONEY,
	EX_GRID_COL_FORMAT_BOOL
} ExGridColumnFormat;

GValue* ex_grid_get_selected_row_value(ExGrid *grid, const guint n_column);
void ex_grid_lookup_field (ExGrid *grid, gint n_col, GdaDataModel *model, gint model_col);
GtkCellRenderer* ex_grid_get_column_text_renderer(ExGrid *grid, guint col);
GtkCellRenderer* ex_grid_get_column_combo_renderer(ExGrid *grid, guint col);
void ex_grid_column_set_format(ExGrid *grid, guint col, ExGridColumnFormat format);
void ex_grid_column_set_title(ExGrid *grid, guint col, gchar *title);
void ex_grid_set_columns_resizable(ExGrid *grid, gboolean val);
void ex_grid_set_columns_reordable(ExGrid *grid, gboolean val);
void ex_grid_set_column_data_cb (ExGrid *grid, gint column, GtkTreeCellDataFunc func);
void ex_grid_set_column_width(ExGrid *grid, gint column, guint width);

#endif /* _EX_GRID_H_ */

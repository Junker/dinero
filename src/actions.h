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
#include <gtk/gtk.h>
#include <libgda-ui/libgda-ui.h>


#include "ex-grid.h"
#include "gtkdateentry.h"


void show_error_dialog (const gchar *title,const  gchar *message,const  gchar *error); 
gchar* show_input_dialog(const gchar *label_text,const gchar *entry_text);
gint show_warning_dialog(gchar *message, GtkWindow *parrent);

GtkDateEntry* create_dateentry(GtkContainer *container);
ExGrid* create_grid(GtkContainer *container);
GdauiCombo* create_combo(GtkContainer *container);
GdauiRawForm* create_form(GtkContainer *container);
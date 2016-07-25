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
#include <libgda/libgda.h>
#include <libgda-ui/libgda-ui.h>

GdaConnection *connection;
GdaSqlParser *parser;

GdaSet* db_exec_sql (const gchar *sql, ...);
GdaDataModel* db_exec_select_sql (const gchar *sql, ...);
const GValue* db_get_value (const gchar *sql, ...);
gboolean db_connect (gchar * dsn);
void db_disconnect();
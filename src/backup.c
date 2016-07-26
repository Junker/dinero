/*
 *
 * Copyright (C) Dmitry Kosenkov
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

#include <string.h>
#include <glib/gprintf.h>
#include <glib/gi18n-lib.h>

#include "common.h"
#include "mainform.h"
#include "db.h"
#include "sutil.h"

static GtkWidget *window;

#define DB_FILE_NAME_SUFFIX ".sqlite" 

void restore_backup(GtkWindow *parent_window)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	GError *error = NULL;
	gint res;

	dialog = gtk_file_chooser_dialog_new(_("Load backup file"),
	                                      parent_window,
	                                      GTK_FILE_CHOOSER_ACTION_OPEN,
	                                      _("_Cancel"),
	                                      GTK_RESPONSE_CANCEL,
	                                      _("_Load"),
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "SQLite");
    gtk_file_filter_add_pattern(filter, "*.sqlite");

    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		gchar *filename = gtk_file_chooser_get_filename(chooser);

		gchar *db_file_path = g_build_filename(home_path, DB_FILE_NAME, NULL);

		GFile *source_file = g_file_new_for_path(filename);
		GFile *dest_file = g_file_new_for_path(db_file_path);

		g_file_copy(source_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error);

		if (error)
		{
			show_warning_dialog(error->message, parent_window);
		}
		else
		{
			db_disconnect();
			open_main_db();

			create_lookup_models();

			gtk_widget_destroy(main_window);

			main_window = create_main_window();
			gtk_widget_show(main_window);
		}

		g_free(db_file_path);
		g_object_unref(source_file);
		g_object_unref(dest_file);
	}


	gtk_widget_destroy (dialog);
}


void save_backup(GtkWindow *parent_window)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	GError *error = NULL;
	gint res;

	dialog = gtk_file_chooser_dialog_new(_("Save backup file"),
	                                      parent_window,
	                                      GTK_FILE_CHOOSER_ACTION_SAVE,
	                                      _("_Cancel"),
	                                      GTK_RESPONSE_CANCEL,
	                                      _("_Save"),
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);


	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "SQLite");
    gtk_file_filter_add_pattern(filter, "*.sqlite");

	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		
		GString *file_path = g_string_new(gtk_file_chooser_get_filename(chooser));

		if (!g_str_has_suffix(file_path->str, DB_FILE_NAME_SUFFIX))
			g_string_append(file_path, DB_FILE_NAME_SUFFIX);

		gchar *db_file_path = g_build_filename(home_path, DB_FILE_NAME, NULL);

		GFile *source_file = g_file_new_for_path(db_file_path);
		GFile *dest_file   = g_file_new_for_path(file_path->str);

		g_file_copy(source_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error);

		if (error)
		{
			show_warning_dialog(error->message, parent_window);
		}

		g_string_free(file_path, TRUE);
		g_free(db_file_path);
		g_object_unref(source_file);
		g_object_unref(dest_file);
	}


	gtk_widget_destroy(dialog);
}
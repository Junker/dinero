/*
 * Copyright (C) Dmitry Kosenkov <junker@front.ru>
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

#include <string.h>


#include "actions.h"
#include "common.h"
#include "db.h"
#include "sutil.h"
#include "gtkdateentry.h"

#include "ex-grid.h"




void show_error_dialog (const gchar *title, const gchar *message, const gchar *error)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new_with_markup (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
		error ? "<span weight=\"heavy\" size=\"larger\">%s</span>" : "%s", message);

	if (error)
		gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog), "%s", error);

	gtk_window_set_title (GTK_WINDOW (dialog), title ? title : "Error");
	gdk_beep ();
	
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

gchar* show_input_dialog(const gchar *label_text, const gchar *entry_text)
{
	GtkBuilder *gbuilder = gtk_builder_new();
	ex_builder_load_file (gbuilder, "inputdialog.ui");
	gtk_builder_connect_signals (gbuilder,NULL);
	
	GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object (gbuilder, "dialog"));
	GtkLabel *label = GTK_LABEL(gtk_builder_get_object (gbuilder, "label"));
	GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object (gbuilder, "entry"));

	gtk_label_set_text(label,label_text);
	gtk_entry_set_text(entry,entry_text);

	gchar *result = "";
	
	if (gtk_dialog_run(dialog) == GTK_RESPONSE_OK) 
	{
		result = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
		g_strstrip(result);
	}
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
	
	return result;
}	
	
gint show_warning_dialog(gchar *message, GtkWindow *parrent)
{
	GtkWidget *dialog = gtk_message_dialog_new (parrent,
	                              GTK_DIALOG_DESTROY_WITH_PARENT,
	                              GTK_MESSAGE_WARNING,
	                              GTK_BUTTONS_YES_NO,
	                              message);

	gint result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (GTK_WIDGET(dialog));
	
	return result;
}



GtkDateEntry* create_dateentry(GtkContainer *container) 
{
	GtkDateEntry *dateentry = gtk_date_entry_new();
	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(dateentry));
	gtk_widget_show(GTK_WIDGET(dateentry));
	
	return dateentry;
}

ExGrid* create_grid(GtkContainer *container) 
{
	//dirty hack, need rewrite or send bugreport to glade command
/*	if (G_OBJECT_TYPE(container) == GTK_TYPE_SCROLLED_WINDOW)
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (container), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
*/
	
	ExGrid *grid = ex_grid_new(NULL);
	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(grid));
	gtk_widget_show(GTK_WIDGET(grid));
	
	return grid;
}

GdauiCombo* create_combo(GtkContainer *container) 
{

	GdauiCombo *combo = gdaui_combo_new();
	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(combo));
	gtk_widget_show(GTK_WIDGET(combo));

	g_object_set(G_OBJECT(combo), "as-list", TRUE, NULL);
	
	return combo;
}

GdauiRawForm* create_form(GtkContainer *container) 
{

	GdauiRawForm *form = gdaui_raw_form_new(NULL);
	gtk_container_add(GTK_CONTAINER(container), GTK_WIDGET(form));
	gtk_widget_show(GTK_WIDGET(form));
	
	return form;
}
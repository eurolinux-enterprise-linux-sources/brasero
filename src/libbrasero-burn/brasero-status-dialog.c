/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Libbrasero-burn
 * Copyright (C) Philippe Rouquier 2005-2009 <bonfire-app@wanadoo.fr>
 *
 * Libbrasero-burn is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The Libbrasero-burn authors hereby grant permission for non-GPL compatible
 * GStreamer plugins to be used and distributed together with GStreamer
 * and Libbrasero-burn. This permission is above and beyond the permissions granted
 * by the GPL license by which Libbrasero-burn is covered. If you modify this code
 * you may extend this exception to your version of the code, but you are not
 * obligated to do so. If you do not wish to do so, delete this exception
 * statement from your version.
 * 
 * Libbrasero-burn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>

#include <gtk/gtk.h>

#include "brasero-misc.h"

#include "brasero-units.h"

#include "brasero-track-data-cfg.h"

#include "brasero-enums.h"
#include "brasero-session.h"
#include "brasero-status-dialog.h"

typedef struct _BraseroStatusDialogPrivate BraseroStatusDialogPrivate;
struct _BraseroStatusDialogPrivate
{
	BraseroBurnSession *session;
	GtkWidget *progress;
	GtkWidget *action;

	guint id;
};

#define BRASERO_STATUS_DIALOG_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), BRASERO_TYPE_STATUS_DIALOG, BraseroStatusDialogPrivate))

enum {
	PROP_0,
	PROP_SESSION
};

G_DEFINE_TYPE (BraseroStatusDialog, brasero_status_dialog, GTK_TYPE_MESSAGE_DIALOG);


static void
brasero_status_dialog_update (BraseroStatusDialog *self,
			      BraseroStatus *status)
{
	gchar *string;
	gchar *size_str;
	goffset session_bytes;
	gchar *current_action;
	BraseroBurnResult res;
	BraseroTrackType *type;
	BraseroStatusDialogPrivate *priv;

	priv = BRASERO_STATUS_DIALOG_PRIVATE (self);

	current_action = brasero_status_get_current_action (status);
	if (current_action) {
		gchar *string;

		string = g_strdup_printf ("<i>%s</i>", current_action);
		gtk_label_set_markup (GTK_LABEL (priv->action), string);
		g_free (string);
	}
	else
		gtk_label_set_markup (GTK_LABEL (priv->action), "");

	g_free (current_action);

	if (brasero_status_get_progress (status) < 0.0)
		gtk_progress_bar_pulse (GTK_PROGRESS_BAR (priv->progress));
	else
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress),
					       brasero_status_get_progress (status));

	res = brasero_burn_session_get_size (priv->session,
					     NULL,
					     &session_bytes);
	if (res != BRASERO_BURN_OK)
		return;

	type = brasero_track_type_new ();
	brasero_burn_session_get_input_type (priv->session, type);

	if (brasero_track_type_get_has_stream (type)) {
		if (BRASERO_STREAM_FORMAT_HAS_VIDEO (brasero_track_type_get_stream_format (type))) {
			guint64 free_time;

			/* This is an embarassing problem: this is an approximation based on the fact that
			 * 2 hours = 4.3GiB */
			free_time = session_bytes * 72000LL / 47LL;
			size_str = brasero_units_get_time_string (free_time,
			                                          TRUE,
			                                          TRUE);
		}
		else
			size_str = brasero_units_get_time_string (session_bytes, TRUE, FALSE);
	}
	/* NOTE: this is perfectly fine as brasero_track_type_get_medium_type ()
	 * will return BRASERO_MEDIUM_NONE if this is not a MEDIUM track type */
	else if (brasero_track_type_get_medium_type (type) & BRASERO_MEDIUM_HAS_AUDIO)
		size_str = brasero_units_get_time_string (session_bytes, TRUE, FALSE);
	else
		size_str = g_format_size_for_display (session_bytes);

	brasero_track_type_free (type);

	string = g_strdup_printf (_("Estimated size: %s"), size_str);
	g_free (size_str);

	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progress), string);
	g_free (string);
}

static gboolean
brasero_status_dialog_wait_for_ready_state (BraseroStatusDialog *dialog)
{
	BraseroStatusDialogPrivate *priv;
	BraseroBurnResult result;
	BraseroStatus *status;

	priv = BRASERO_STATUS_DIALOG_PRIVATE (dialog);

	status = brasero_status_new ();
	result = brasero_burn_session_get_status (priv->session, status);

	if (result != BRASERO_BURN_NOT_READY) {
		gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
		brasero_status_free (status);
		priv->id = 0;
		return FALSE;
	}

	brasero_status_dialog_update (dialog, status);
	brasero_status_free (status);
	return TRUE;
}

static gboolean
brasero_status_dialog_deep_directory_cb (BraseroTrackDataCfg *project,
					 const gchar *name,
					 BraseroStatusDialog *dialog)
{
	gint answer;
	gchar *string;
	GtkWidget *message;

	gtk_widget_hide (GTK_WIDGET (dialog));

	string = g_strdup_printf (_("Do you really want to add \"%s\" to the selection?"), name);
	message = gtk_message_dialog_new (GTK_WINDOW (dialog),
					  GTK_DIALOG_DESTROY_WITH_PARENT|
					  GTK_DIALOG_MODAL,
					  GTK_MESSAGE_WARNING,
					  GTK_BUTTONS_NONE,
					  string);
	g_free (string);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message),
						  _("The children of this directory will have 7 parent directories."
						    "\nBrasero can create an image of such a file hierarchy and burn it; but the disc may not be readable on all operating systems."
						    "\nNOTE: Such a file hierarchy is known to work on linux."));

	gtk_dialog_add_button (GTK_DIALOG (message), GTK_STOCK_CANCEL, GTK_RESPONSE_NO);
	gtk_dialog_add_button (GTK_DIALOG (message), _("_Add File"), GTK_RESPONSE_YES);

	answer = gtk_dialog_run (GTK_DIALOG (message));
	gtk_widget_destroy (message);

	gtk_widget_show (GTK_WIDGET (dialog));

	return (answer != GTK_RESPONSE_YES);
}

static gboolean
brasero_status_dialog_2G_file_cb (BraseroTrackDataCfg *track,
				  const gchar *name,
				  BraseroStatusDialog *dialog)
{
	gint answer;
	gchar *string;
	GtkWidget *message;

	gtk_widget_hide (GTK_WIDGET (dialog));

	string = g_strdup_printf (_("Do you really want to add \"%s\" to the selection and use the third version of ISO9660 standard to support it?"), name);
	message = gtk_message_dialog_new (GTK_WINDOW (dialog),
					  GTK_DIALOG_DESTROY_WITH_PARENT|
					  GTK_DIALOG_MODAL,
					  GTK_MESSAGE_WARNING,
					  GTK_BUTTONS_NONE,
					  string);
	g_free (string);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message),
						  _("The size of the file is over 2 GiB. Files larger than 2 GiB are not supported by ISO9660 standard in its first and second versions (the most widespread ones)."
						    "\nIt is recommended to use the third version of ISO9660 standard which is supported by most of the operating systems including Linux and all versions of Windows ©."
						    "\nHowever MacOS X cannot read images created with version 3 of ISO9660 standard."));

	gtk_dialog_add_button (GTK_DIALOG (message), GTK_STOCK_CANCEL, GTK_RESPONSE_NO);
	gtk_dialog_add_button (GTK_DIALOG (message), _("_Add File"), GTK_RESPONSE_YES);

	answer = gtk_dialog_run (GTK_DIALOG (message));
	gtk_widget_destroy (message);

	gtk_widget_show (GTK_WIDGET (dialog));

	return (answer != GTK_RESPONSE_YES);
}

static void
brasero_status_dialog_joliet_rename_cb (BraseroTrackData *track,
					BraseroStatusDialog *dialog)
{
	GtkResponseType answer;
	GtkWidget *message;
	gchar *secondary;

	gtk_widget_hide (GTK_WIDGET (dialog));

	message = gtk_message_dialog_new (GTK_WINDOW (dialog),
					  GTK_DIALOG_DESTROY_WITH_PARENT|
					  GTK_DIALOG_MODAL,
					  GTK_MESSAGE_WARNING,
					  GTK_BUTTONS_NONE,
					  _("Should files be renamed to be fully Windows-compatible?"));

	secondary = g_strdup_printf ("%s\n%s",
				     _("Some files don't have a suitable name for a fully Windows-compatible CD."),
				     _("Those names should be changed and truncated to 64 characters."));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s", secondary);
	g_free (secondary);

	gtk_dialog_add_button (GTK_DIALOG (message),
			       _("_Disable Full Windows Compatibility"),
			       GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (message),
			       _("_Rename for Full Windows Compatibility"),
			       GTK_RESPONSE_YES);

	answer = gtk_dialog_run (GTK_DIALOG (message));
	gtk_widget_destroy (message);

	if (answer != GTK_RESPONSE_YES)
		brasero_track_data_rm_fs (track, BRASERO_IMAGE_FS_JOLIET);
	else
		brasero_track_data_add_fs (track, BRASERO_IMAGE_FS_JOLIET);

	gtk_widget_show (GTK_WIDGET (dialog));
}

static void
brasero_status_dialog_wait_for_session (BraseroStatusDialog *dialog)
{
	BraseroStatus *status;
	BraseroBurnResult result;
	BraseroTrackType *track_type;
	BraseroStatusDialogPrivate *priv;

	priv = BRASERO_STATUS_DIALOG_PRIVATE (dialog);

	/* Make sure we really need to run this dialog */
	status = brasero_status_new ();
	result = brasero_burn_session_get_status (priv->session, status);
	if (result != BRASERO_BURN_NOT_READY) {
		brasero_status_free (status);
		return;
	}

	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);

	track_type = brasero_track_type_new ();
	brasero_burn_session_get_input_type (priv->session, track_type);
	if (brasero_track_type_get_has_data (track_type)) {
		GSList *tracks;
		BraseroTrack *track;

		tracks = brasero_burn_session_get_tracks (priv->session);
		track = tracks->data;

		if (BRASERO_IS_TRACK_DATA_CFG (track)) {
			g_signal_connect (track,
					  "joliet-rename",
					  G_CALLBACK (brasero_status_dialog_joliet_rename_cb),
					  dialog);
			g_signal_connect (track,
					  "2G-file",
					  G_CALLBACK (brasero_status_dialog_2G_file_cb),
					  dialog);
			g_signal_connect (track,
					  "deep-directory",
					  G_CALLBACK (brasero_status_dialog_deep_directory_cb),
					  dialog);
		}
	}
	brasero_track_type_free (track_type);

	brasero_status_dialog_update (dialog, status);
	brasero_status_free (status);
	priv->id = g_timeout_add (200,
				  (GSourceFunc) brasero_status_dialog_wait_for_ready_state,
				  dialog);
}

static void
brasero_status_dialog_init (BraseroStatusDialog *object)
{
	BraseroStatusDialogPrivate *priv;
	GtkWidget *main_box;
	GtkWidget *box;

	priv = BRASERO_STATUS_DIALOG_PRIVATE (object);

	gtk_dialog_add_button (GTK_DIALOG (object),
			       GTK_STOCK_CANCEL,
			       GTK_RESPONSE_CANCEL);

	box = gtk_vbox_new (FALSE, 4);
	gtk_widget_show (box);
	main_box = gtk_dialog_get_content_area (GTK_DIALOG (object));
	gtk_box_pack_end (GTK_BOX (main_box),
			  box,
			  TRUE,
			  TRUE,
			  0);

	priv->progress = gtk_progress_bar_new ();
	gtk_widget_show (priv->progress);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progress), " ");
	gtk_box_pack_start (GTK_BOX (box),
			    priv->progress,
			    TRUE,
			    TRUE,
			    0);

	priv->action = gtk_label_new ("");
	gtk_widget_show (priv->action);
	gtk_label_set_use_markup (GTK_LABEL (priv->action), TRUE);
	gtk_misc_set_alignment (GTK_MISC (priv->action), 0.0, 0.5);
	gtk_box_pack_start (GTK_BOX (box),
			    priv->action,
			    FALSE,
			    TRUE,
			    0);
}

static void
brasero_status_dialog_set_property (GObject *object,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *pspec)
{
	BraseroStatusDialogPrivate *priv;

	g_return_if_fail (BRASERO_IS_STATUS_DIALOG (object));

	priv = BRASERO_STATUS_DIALOG_PRIVATE (object);

	switch (prop_id)
	{
	case PROP_SESSION: /* Readable and only writable at creation time */
		priv->session = BRASERO_BURN_SESSION (g_value_get_object (value));
		g_object_ref (priv->session);
		brasero_status_dialog_wait_for_session (BRASERO_STATUS_DIALOG (object));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
brasero_status_dialog_get_property (GObject *object,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *pspec)
{
	BraseroStatusDialogPrivate *priv;

	g_return_if_fail (BRASERO_IS_STATUS_DIALOG (object));

	priv = BRASERO_STATUS_DIALOG_PRIVATE (object);

	switch (prop_id)
	{
	case PROP_SESSION:
		g_value_set_object (value, priv->session);
		g_object_ref (priv->session);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
brasero_status_dialog_finalize (GObject *object)
{
	BraseroStatusDialogPrivate *priv;

	priv = BRASERO_STATUS_DIALOG_PRIVATE (object);
	if (priv->session) {
		g_object_unref (priv->session);
		priv->session = NULL;
	}

	if (priv->id) {
		g_source_remove (priv->id);
		priv->id = 0;
	}

	G_OBJECT_CLASS (brasero_status_dialog_parent_class)->finalize (object);
}

static void
brasero_status_dialog_class_init (BraseroStatusDialogClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (BraseroStatusDialogPrivate));

	object_class->finalize = brasero_status_dialog_finalize;
	object_class->set_property = brasero_status_dialog_set_property;
	object_class->get_property = brasero_status_dialog_get_property;

	g_object_class_install_property (object_class,
					 PROP_SESSION,
					 g_param_spec_object ("session",
							      "The session",
							      "The session to work with",
							      BRASERO_TYPE_BURN_SESSION,
							      G_PARAM_READWRITE|G_PARAM_CONSTRUCT_ONLY));
}

GtkWidget *
brasero_status_dialog_new (BraseroBurnSession *session,
			   GtkWidget *parent)
{
	return g_object_new (BRASERO_TYPE_STATUS_DIALOG,
			     "session", session,
			     "transient-for", parent,
			     "modal", TRUE,
			     "title",  _("Size Estimation"),
			     "message-type", GTK_MESSAGE_OTHER,
			     "text", _("Please wait until the estimation of the size is completed."),
			     "secondary-text", _("All files need to be analysed to complete this operation."),
			     NULL);
}

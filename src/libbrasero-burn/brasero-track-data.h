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

#ifndef _BRASERO_TRACK_DATA_H_
#define _BRASERO_TRACK_DATA_H_

#include <glib-object.h>

#include <brasero-track.h>

G_BEGIN_DECLS

struct _BraseroGraftPt {
	gchar *uri;
	gchar *path;
};
typedef struct _BraseroGraftPt BraseroGraftPt;

void
brasero_graft_point_free (BraseroGraftPt *graft);

BraseroGraftPt *
brasero_graft_point_copy (BraseroGraftPt *graft);


#define BRASERO_TYPE_TRACK_DATA             (brasero_track_data_get_type ())
#define BRASERO_TRACK_DATA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BRASERO_TYPE_TRACK_DATA, BraseroTrackData))
#define BRASERO_TRACK_DATA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BRASERO_TYPE_TRACK_DATA, BraseroTrackDataClass))
#define BRASERO_IS_TRACK_DATA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BRASERO_TYPE_TRACK_DATA))
#define BRASERO_IS_TRACK_DATA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BRASERO_TYPE_TRACK_DATA))
#define BRASERO_TRACK_DATA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BRASERO_TYPE_TRACK_DATA, BraseroTrackDataClass))

typedef struct _BraseroTrackDataClass BraseroTrackDataClass;
typedef struct _BraseroTrackData BraseroTrackData;

struct _BraseroTrackDataClass
{
	BraseroTrackClass parent_class;

	/* virtual functions */
	BraseroBurnResult	(*set_source)		(BraseroTrackData *track,
							 GSList *grafts,
							 GSList *unreadable);
	BraseroBurnResult       (*add_fs)		(BraseroTrackData *track,
							 BraseroImageFS fstype);

	BraseroBurnResult       (*rm_fs)		(BraseroTrackData *track,
							 BraseroImageFS fstype);

	BraseroImageFS		(*get_fs)		(BraseroTrackData *track);
	GSList*			(*get_grafts)		(BraseroTrackData *track);
	GSList*			(*get_excluded)		(BraseroTrackData *track);
	guint64			(*get_file_num)		(BraseroTrackData *track);
};

struct _BraseroTrackData
{
	BraseroTrack parent_instance;
};

GType brasero_track_data_get_type (void) G_GNUC_CONST;

BraseroTrackData *
brasero_track_data_new (void);

BraseroBurnResult
brasero_track_data_set_source (BraseroTrackData *track,
			       GSList *grafts,
			       GSList *unreadable);

BraseroBurnResult
brasero_track_data_add_fs (BraseroTrackData *track,
			   BraseroImageFS fstype);

BraseroBurnResult
brasero_track_data_rm_fs (BraseroTrackData *track,
			  BraseroImageFS fstype);

BraseroBurnResult
brasero_track_data_set_data_blocks (BraseroTrackData *track,
				    goffset blocks);

BraseroBurnResult
brasero_track_data_set_file_num (BraseroTrackData *track,
				 guint64 number);

GSList *
brasero_track_data_get_grafts (BraseroTrackData *track);

GSList *
brasero_track_data_get_excluded (BraseroTrackData *track,
				 gboolean copy);

BraseroBurnResult
brasero_track_data_get_paths (BraseroTrackData *track,
			      gboolean use_joliet,
			      const gchar *grafts_path,
			      const gchar *excluded_path,
			      const gchar *emptydir,
			      const gchar *videodir,
			      GError **error);

BraseroBurnResult
brasero_track_data_get_file_num (BraseroTrackData *track,
				 guint64 *file_num);

BraseroImageFS
brasero_track_data_get_fs (BraseroTrackData *track);

G_END_DECLS

#endif /* _BRASERO_TRACK_DATA_H_ */

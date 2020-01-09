/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/***************************************************************************
*            search.h
*
*  dim mai 22 11:20:54 2005
*  Copyright  2005  Philippe Rouquier
*  brasero-app@wanadoo.fr
****************************************************************************/

/*
 *  Brasero is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Brasero is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef SEARCH_H
#define SEARCH_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BRASERO_TYPE_SEARCH         (brasero_search_get_type ())
#define BRASERO_SEARCH(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), BRASERO_TYPE_SEARCH, BraseroSearch))
#define BRASERO_SEARCH_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), BRASERO_TYPE_SEARCH, BraseroSearchClass))
#define BRASERO_IS_SEARCH(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), BRASERO_TYPE_SEARCH))
#define BRASERO_IS_SEARCH_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), BRASERO_TYPE_SEARCH))
#define BRASERO_SEARCH_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), BRASERO_TYPE_SEARCH, BraseroSearchClass))
typedef struct BraseroSearchPrivate BraseroSearchPrivate;

typedef struct {
	GtkVBox parent;
	BraseroSearchPrivate *priv;
} BraseroSearch;

typedef struct {
	GtkVBoxClass parent_class;;
} BraseroSearchClass;

GType brasero_search_get_type (void);
GtkWidget *brasero_search_new (void);

G_END_DECLS

#endif

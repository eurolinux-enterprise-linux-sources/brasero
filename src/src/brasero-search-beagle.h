/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Brasero
 * Copyright (C) Philippe Rouquier 2005-2009 <bonfire-app@wanadoo.fr>
 * 
 *  Brasero is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 * brasero is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with brasero.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifndef _BRASERO_SEARCH_BEAGLE_H_
#define _BRASERO_SEARCH_BEAGLE_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define BRASERO_TYPE_SEARCH_BEAGLE             (brasero_search_beagle_get_type ())
#define BRASERO_SEARCH_BEAGLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BRASERO_TYPE_SEARCH_BEAGLE, BraseroSearchBeagle))
#define BRASERO_SEARCH_BEAGLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BRASERO_TYPE_SEARCH_BEAGLE, BraseroSearchBeagleClass))
#define BRASERO_IS_SEARCH_BEAGLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BRASERO_TYPE_SEARCH_BEAGLE))
#define BRASERO_IS_SEARCH_BEAGLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BRASERO_TYPE_SEARCH_BEAGLE))
#define BRASERO_SEARCH_BEAGLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BRASERO_TYPE_SEARCH_BEAGLE, BraseroSearchBeagleClass))

typedef struct _BraseroSearchBeagleClass BraseroSearchBeagleClass;
typedef struct _BraseroSearchBeagle BraseroSearchBeagle;

struct _BraseroSearchBeagleClass
{
	GObjectClass parent_class;
};

struct _BraseroSearchBeagle
{
	GObject parent_instance;
};

GType brasero_search_beagle_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _BRASERO_SEARCH_BEAGLE_H_ */

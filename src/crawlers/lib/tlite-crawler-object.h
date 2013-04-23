/*
 * Copyright (C) 2013, Wind River Systems
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __TLITE_CRAWLER_OBJECT_H__
#define __TLITE_CRAWLER_OBJECT_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define TLITE_TYPE_CRAWLER            (tlite_crawler_get_type ())
#define TLITE_CRAWLER(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), TLITE_TYPE_CRAWLER, TLiteCrawler))
#define TLITE_CRAWLER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TLITE_TYPE_CRAWLER, TLiteCrawlerClass))
#define TLITE_IS_CRAWLER(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), TLITE_TYPE_CRAWLER))
#define TLITE_IS_CRAWLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TLITE_TYPE_CRAWLER))
#define TLITE_CRAWLER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TLITE_TYPE_CRAWLER, TLiteCrawlerClass))

typedef struct TLiteCrawler         TLiteCrawler;
typedef struct TLiteCrawlerClass    TLiteCrawlerClass;
typedef struct TLiteCrawlerPrivate  TLiteCrawlerPrivate;

struct TLiteCrawler {
	GObject parent;
	TLiteCrawlerPrivate *priv;
};

struct TLiteCrawlerClass {
	GObjectClass parent;

	void     (* found)                    (TLiteCrawler *crawler);
	void     (* finished)                 (TLiteCrawler *crawler);
};

GType           tlite_crawler_get_type     (void);
TLiteCrawler   *tlite_crawler_new          (void);
gboolean        tlite_crawler_start        (TLiteCrawler *crawler,
                                            GMount       *mount);

GMount   	   *tlite_crawler_get_mount    (TLiteCrawler *crawler);

G_END_DECLS

#endif /* __LIBTRACKER_MINER_CRAWLER_H__ */

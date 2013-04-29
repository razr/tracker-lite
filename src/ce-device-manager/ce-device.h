#ifndef __TLITE_CE_DEVICE_H__
#define __TLITE_CE_DEVICE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TLITE_TYPE_CE_DEVICE         (tlite_ce_device_get_type())
#define TLITE_CE_DEVICE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TLITE_TYPE_CE_DEVICE, TLiteCeDevice))
#define TLITE_CE_DEVICE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c),    TLITE_TYPE_CE_DEVICE, TLiteCeDevice))
#define TLITE_IS_CE_DEVICE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TLITE_TYPE_CE_DEVICE))
#define TLITE_IS_CE_DEVICE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c),    TLITE_TYPE_CE_DEVICE))
#define TLITE_CE_DEVICE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o),  TLITE_TYPE_CE_DEVICE, TLiteCeDevice))

#define TLITE_CE_DEVICE_ERROR tlite_ce_device_error_quark ()

typedef struct _TLiteCeDevice TLiteCeDevice;
typedef struct _TLiteCeDevicePrivate TLiteCeDevicePrivate;

struct _TLiteCeDevice {
	GObject parent_instance;
	TLiteCeDevicePrivate *priv;
};

typedef struct {
	GObjectClass parent_class;

	void (* indexed)   (TLiteCeDevice *device, gchar *name);
//	void (* content_modified)   (TLiteCeDevice *device);
} TLiteCeDeviceClass;

GType                tlite_ce_device_get_type           (void) G_GNUC_CONST;
GQuark               tlite_ce_device_error_quark        (void) G_GNUC_CONST;

TLiteCeDevice *tlite_ce_device_new          (GMount *mount);
gboolean tlite_ce_device_add_db				(TLiteCeDevice	*device);
char *   tlite_ce_device_get_name           (TLiteCeDevice  *device);
gboolean tlite_ce_device_is_indexed         (TLiteCeDevice  *device);
void tlite_ce_device_indexed     	        (TLiteCeDevice  *device);

G_END_DECLS

#endif /* __TLITE_CE_DEVICE_H__ */

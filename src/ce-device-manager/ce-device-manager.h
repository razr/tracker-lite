#ifndef __TLITE_CE_DEVICE_MANAGER_H__
#define __TLITE_CE_DEVICE_MANAGER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TLITE_TYPE_CE_DEVICE_MANAGER         (tlite_ce_device_manager_get_type())
#define TLITE_CE_DEVICE_MANAGER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManager))
#define TLITE_CE_DEVICE_MANAGER_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c),    TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManagerClass))
#define TLITE_IS_CE_DEVICE_MANAGER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TLITE_TYPE_CE_DEVICE_MANAGER))
#define TLITE_IS_CE_DEVICE_MANAGER_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c),    TLITE_TYPE_CE_DEVICE_MANAGER))
#define TLITE_CE_DEVICE_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o),  TLITE_TYPE_CE_DEVICE_MANAGER, TLiteCeDeviceManagerClass))

#define TLITE_CE_DEVICE_MANAGER_ERROR tlite_ce_device_manager_error_quark ()

typedef struct _TLiteCeDeviceManager TLiteCeDeviceManager;
typedef struct _TLiteCeDeviceManagerPrivate TLiteCeDeviceManagerPrivate;

struct _TLiteCeDeviceManager {
	GObject parent_instance;
	TLiteCeDeviceManagerPrivate *priv;
};

typedef struct {
	GObjectClass parent_class;

	void (* ce_device_added)   (TLiteCeDeviceManager *manager,
	                            const gchar          *device_name);
	void (* ce_device_removed) (TLiteCeDeviceManager *manager,
	                            const gchar          *device_name);
	void (* indexing_started)  (TLiteCeDeviceManager *manager,
	                            const gchar          *device_name);
	void (* indexing_progress) (TLiteCeDeviceManager *manager,
	                            const gchar          *device_name);
	void (* indexing_finished) (TLiteCeDeviceManager *manager,
	                            const gchar          *device_name);
} TLiteCeDeviceManagerClass;

GType                tlite_ce_device_manager_get_type           (void) G_GNUC_CONST;
GQuark               tlite_ce_device_manager_error_quark        (void) G_GNUC_CONST;

TLiteCeDeviceManager *tlite_ce_device_manager_new                (void);

G_END_DECLS

#endif /* __TLITE_CE_DEVICE_MANAGER_H__ */

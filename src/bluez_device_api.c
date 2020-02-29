/**
* Author: Kyle Van Cleae
*/
#include <stdio.h>

#include "bluez_device_api.h"
#include "bluez_dbus_names.h"

/*
* Private Function Declerations
*/
static void bluez_signal_device_changed(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *signal,
                    GVariant *params,
                    void *userdata);
static void bluez_device_parse_properties(const char * key, GVariant * propertyValue);

/*
*	Private Variables
*/
GDBusConnection * mCon;

// GDBUS signals
static guint device_changed;

/*
* Accessors
*/


/*
* Modifiers
*
*/
void bluez_device_init_signals(void)
{
	device_changed = g_dbus_connection_signal_subscribe(mCon,
						BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
						"org.freedesktop.DBus.Properties",
						"PropertiesChanged",
						NULL,
						BLUEZ_DEVICE_INTERFACE,				// defined in bluez_dbus_names.h
						G_DBUS_SIGNAL_FLAGS_NONE,
						bluez_signal_device_changed,
						NULL,
						NULL);
}
void bluez_device_mute_signals(void)
{
	g_dbus_connection_signal_unsubscribe(mCon, device_changed);
}

/*
* Other
*/
int bluez_device_init(GDBusConnection * conn)
{
	printf("Initializing Device...\n");
	
	mCon = conn;
	
	if(mCon == NULL) {
		g_printerr("Not able to get connection to system bus\n");
		/**TODO Need to organize Error Codes */
		return -3;
	}
	
	//bluez_adapter_init_signals();
	
	return 0;
}

bool bluez_device_trust_device(const char * objectPath)
{
	GVariant *result;
	GError *error = NULL;
	bool succeed = true;

	printf("Trusting Device: %s\n", objectPath);
	
	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,
					     objectPath,
					     "org.freedesktop.DBus.Properties",
					     "Set",
					     g_variant_new("(ssv)", BLUEZ_DEVICE_INTERFACE, "Trusted", g_variant_new("b", TRUE)),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
						 
	g_variant_unref(result);
	
	if(error != NULL)
	{
		g_print("Error: Device Trust. Message: %s", error->message);
		succeed = false;
	}
	
	return succeed;
}

bool bluez_device_pair_device(const char *objectPath)
{
	GVariant *result;
	GError *error = NULL;
	bool succeed = true;

	printf("Pairing Device: %s\n", objectPath);
	
	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,
					     objectPath,
					     BLUEZ_DEVICE_INTERFACE,
					     "Pair",
					     NULL,
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
						 
	g_variant_unref(result);
	
	if(error != NULL)
	{
		g_print("Error:Device Pair. Message: %s", error->message);
		succeed = false;
	}
	
	return succeed;
}


static void bluez_signal_device_changed(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *signal,
                    GVariant *params,
                    void *userdata)
{
    (void)conn;
    (void)sender;
    (void)path;
    (void)interface;
    (void)userdata;
	static GError *error;
    GVariantIter *properties = NULL;
    GVariantIter *unknown = NULL;
    const char *iface;
    const char *key;
    GVariant *value = NULL;
    const gchar *signature = g_variant_get_type_string(params);
	
	g_print("***\tDevice Signal Properties Changed\t***\n");
	g_print("- Path:\t%s\n", path);
	g_print("- Interface:\t%s\n", interface); 

    if(strcmp(signature, "(sa{sv}as)") != 0) {
        g_print("Invalid signature for %s: %s != %s", signal, signature, "(sa{sv}as)");
        goto done;
    }

    g_variant_get(params, "(&sa{sv}as)", &iface, &properties, &unknown);
    while(g_variant_iter_next(properties, "{&sv}", &key, &value)) {
		bluez_device_parse_properties(key, value);
		
		//g_print("[ Key %s  value: %s]\n", key, g_variant_print(value,TRUE));
        //if(!g_strcmp0(key, "Connected")) {
           // if(!g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN)) {
               // g_print("Invalid argument type for %s: %s != %s", key,
                        //g_variant_get_type_string(value), "b");
               // goto done;
            //}
           // g_print("Device is \"%s\"\n", g_variant_get_boolean(value) ? "Connected" : "Disconnected");
        //}
    }
	g_print("***\tDevice Signal Properties Changed\t***\n");
done:
    if(properties != NULL)
        g_variant_iter_free(properties);
    if(value != NULL)
        g_variant_unref(value);
	
	
}

static void bluez_device_parse_properties(const char * propertyKey, GVariant * propertyValue)
{
	g_print("[ Property Key %s  value: %s]\n", propertyKey, g_variant_print(propertyValue,TRUE));
}

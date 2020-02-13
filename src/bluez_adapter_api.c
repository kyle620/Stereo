/*
 * Author: Kyle Van Cleave
 * 	bluez_adapter_api.c - Is meant to implement the funcionality of the adapter-api.txt file
 * 	that is described by bluez.org
 * 	- The main purpose of this file is to scan for new devices after powering the adapter, if any devices
 * 	  appeared in /org/hciX/dev_XX_YY_ZZ_AA_BB_CC, it is monitered using "InterfaceAdded"
 *	  signal and all the properties of the device are printed
 *	- Once a device is found a bluetooth_device is created which stores alot of properties aboutthe remote device
 *	  
 *	- Required flags, and libs for compiling
 * 		gcc `pkg-config --cflags glib-2.0 gio-2.0` `pkg-config --libs glib-2.0 gio-2.0`
 */

#include <stdio.h>
#include "bluez_adapter_api.h"
#include "bluez_dbus_names.h"		// holds defines for bluez bus name and interfaces
#include "bluetooth_device.h"		// holds information about remote devices discovered during scan

/**
* Private Variable Declerations
**/
static GDBusConnection *mCon;

// GDBUS signals
static guint prop_changed;
static guint iface_added;
static guint iface_removed;


/**
* Private Function Declerations
**/
static void bluez_signal_adapter_changed(GDBusConnection *conn,
					const gchar *sender,
					const gchar *path,
					const gchar *interface,
					const gchar *signal,
					GVariant *params,
					void *userdata);
static void bluez_device_appeared(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static void bluez_device_disappeared(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static int bluez_adapter_call_method( const char *method, GVariant *param);
static int bluez_adapter_set_property(const char *prop, GVariant *value);
static void bluez_get_discovery_filter_cb(GObject *con,GAsyncResult *res,gpointer data);
static void bluez_property_value(const gchar *key, GVariant *value);
static int bluez_set_discovery_filter();

/** 
* Modifiers
**/

void bluez_adapter_set_filter_default(void)
{
	DiscoveryFilter filter;
	
	
	filter.RSSI = -100;
	strcpy(filter.TRANSPORT,"auto");
	
	// add the UUID's we want to filter for
	strcpy(filter.UUID_ARRAY[0], UUID_AUDIO_SOURCE);
	
	g_print("Filter UUID: %s", filter.UUID_ARRAY[0]);
}

int bluez_adapter_init(GDBusConnection * conn)
{
	
	printf("Initializing Adapter...\n");
	
	mCon = conn;
	
	if(mCon == NULL) {
		g_printerr("Not able to get connection to system bus\n");
		/**TODO Need to organize Error Codes */
		return -3;
	}
	
	bluez_adapter_init_signals();
	
	return 0;
}

void bluez_adapter_deinit()
{
	//TODO handle cleanup
	
	g_print("Adapter Deinitializing...\n");
	
	bluez_adapter_mute_signals();
	
	int numberDevices = bluetooth_device_get_number_devices();
	int i;
	char addr[BT_ADDRESS_STRING_SIZE];
	
	for(i = 0; i < numberDevices; i++)
	{
		bluetooth_get_device_address_at_index(i, addr, true);
		bluez_adapter_remove_device_found(addr);
	}
	
	bluez_adapter_power_off();

}

bool bluez_adapter_power_on(void)
{
	int rc = 0;
	bool ret = true;
	rc  = bluez_adapter_set_property("Powered", g_variant_new("b", TRUE));
	
	if(rc) {
		g_print("Not able to Power on the adapter\n");
		bluez_adapter_mute_signals();
		ret = false;
	}
	
	g_print("Adapter is Powered On!\n");
	return ret;
}	

bool bluez_adapter_power_off(void)
{
	int rc = 0;
	
	rc = bluez_adapter_set_property("Powered", g_variant_new("b", FALSE));
	
	return rc;
}

bool bluez_adapter_scan_on(void)
{
	int rc = 0;
	bool ret = true;
	printf("Starting Scan...\n");
	
	rc = bluez_adapter_call_method("StartDiscovery",NULL);
	if(rc) {
		g_print("Not able to scan for new devices\n");
		bluez_adapter_mute_signals();
		ret = false;
	}

	return ret;
}

bool bluez_adapter_scan_off(void)
{
	int rc = 0;
	bool ret = true;
	
	printf("Stopping Scan...\n");
	
	rc = bluez_adapter_call_method("StopDiscovery",NULL);
	if(rc)
	{
		g_print("Not able to stop scanning\n");
		ret = false;
	}
	
	g_usleep(100);

	return ret;
}

bool bluez_adapter_pairable(bool value)
{
	int rc = 0;
	bool ret = true;
	
	 g_print("Aapter is Pairable: \"%s\"\n", value ? "Ture" : "False");
	
	rc = bluez_adapter_set_property("Pairable", g_variant_new("b", value));
	
	if(rc) {
			g_print("Not able to set adapter pairable\n");
			ret = false;
	}
	
	return ret;
}

void bluez_adapter_init_signals(void)
{
	/* TODO move to appropiate file, doesnt belong to adapter
		guint pro_changed = g_dbus_connection_signal_subscribe(mCon,
						"org.bluez",
						"org.freedesktop.DBus.Properties",
						"PropertiesChanged",
						NULL,
						"org.bluez.MediaPlayer1",
						G_DBUS_SIGNAL_FLAGS_NONE,
						bluez_signal_device_changed,
						NULL,
						NULL);
						*/
	
	prop_changed = g_dbus_connection_signal_subscribe(mCon,
						BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
						"org.freedesktop.DBus.Properties",
						"PropertiesChanged",
						NULL,
						BLUEZ_ADAPTER_INTERFACE,			// defined in bluez_dbus_names.h
						G_DBUS_SIGNAL_FLAGS_NONE,
						bluez_signal_adapter_changed,
						NULL,
						NULL);

	iface_added = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.ObjectManager",
							"InterfacesAdded",
							NULL,
							NULL,
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_device_appeared,
							NULL,
							NULL);

	iface_removed = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.ObjectManager",
							"InterfacesRemoved",
							NULL,
							NULL,
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_device_disappeared,
							NULL,
							NULL);
}

void bluez_adapter_mute_signals(void)
{
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed);
	g_dbus_connection_signal_unsubscribe(mCon, iface_added);
	g_dbus_connection_signal_unsubscribe(mCon, iface_removed);
}

void bluez_adapter_remove_device_found(const char * address)
{
	GVariant * addr;
	addr = g_variant_new_string(address);
	
	g_print("Removing device %s\n",address);
		
	bluez_adapter_call_method("RemoveDevice",g_variant_new_tuple(&addr, 1));
	
}


/** 
* Private Functions
**/

/* TODO Move to appropiate file
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
	
	g_print("Path: %s\n", path);
	g_print("Interface: %s\n", interface); 

    if(strcmp(signature, "(sa{sv}as)") != 0) {
        g_print("Invalid signature for %s: %s != %s", signal, signature, "(sa{sv}as)");
        goto done;
    }

    g_variant_get(params, "(&sa{sv}as)", &iface, &properties, &unknown);
    while(g_variant_iter_next(properties, "{&sv}", &key, &value)) {
		g_print("[ Key %s  value: %s]\n", key, g_variant_print(value,TRUE));
        if(!g_strcmp0(key, "Connected")) {
            if(!g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN)) {
                g_print("Invalid argument type for %s: %s != %s", key,
                        g_variant_get_type_string(value), "b");
                goto done;
            }
            g_print("Device is \"%s\"\n", g_variant_get_boolean(value) ? "Connected" : "Disconnected");
        }
    }
done:
    if(properties != NULL)
        g_variant_iter_free(properties);
    if(value != NULL)
        g_variant_unref(value);
}
*/

static void bluez_property_value(const gchar *key, GVariant *value)
{
	const gchar *type = g_variant_get_type_string(value);
	
	g_print("\t%s : ", key);
	switch(*type) {
		case 'o':
		case 's':
			if(strcmp(key,"Address") == 0)
				bluetooth_device_add_device(value,g_variant_get_string(value, NULL));
			g_print("%s\n", g_variant_get_string(value, NULL));
			break;
		case 'b':
			g_print("%d\n", g_variant_get_boolean(value));
			break;
		case 'u':
			g_print("%d\n", g_variant_get_uint32(value));
			break;
		case 'a':
		/* TODO Handling only 'as', but not array of dicts */
			if(g_strcmp0(type, "as"))
				break;
			g_print("\n");
			const gchar *uuid;
			GVariantIter i;
			g_variant_iter_init(&i, value);
			while(g_variant_iter_next(&i, "s", &uuid))
				g_print("\t\t%s\n", uuid);
			break;
		default:
			g_print("Other\n");
			break;
	}
}

static void bluez_device_appeared(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data)
{
	(void)sig;
	(void)sender_name;
	(void)object_path;
	(void)interface;
	(void)signal_name;
	(void)user_data;

	GVariantIter *interfaces;
	const char *object;
	const gchar *interface_name;
	GVariant *properties;
	
	g_print("\n****\t Device Appeared \t****\n");
	
	g_variant_get(parameters, "(&oa{sa{sv}})", &object, &interfaces);
	
	while(g_variant_iter_next(interfaces, "{&s@a{sv}}", &interface_name, &properties)) {
		if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
			g_print("[ %s ]\n", object);
			// TODO Create and add bluetoothdevce
			/*
			if(mDeviceCount < MAX_NUMBER_DEVICES)
			{
				strcpy(mDeviceList[mDeviceCount],object);
				mDeviceCount++;
			}
			*/
			const gchar *property_name;
			GVariantIter i;
			GVariant *prop_val;
			g_variant_iter_init(&i, properties);
			while(g_variant_iter_next(&i, "{&sv}", &property_name, &prop_val))
				bluez_property_value(property_name, prop_val);
			g_variant_unref(prop_val);
		}
		g_variant_unref(properties);
	}
	return;
}

static void bluez_device_disappeared(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data)
{
	(void)sig;
	(void)sender_name;
	(void)object_path;
	(void)interface;
	(void)signal_name;

	GVariantIter *interfaces;
	const char *object;
	const gchar *interface_name;
	char address[BT_ADDRESS_STRING_SIZE] = {'\0'};

	g_variant_get(parameters, "(&oas)", &object, &interfaces);
	while(g_variant_iter_next(interfaces, "s", &interface_name)) {
		if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
			int i;
			char *tmp = g_strstr_len(object, -1, "dev_") + 4;

			for(i = 0; *tmp != '\0'; i++, tmp++) {
				if(*tmp == '_') {
					address[i] = ':';
					continue;
				}
				address[i] = *tmp;
			}
			g_print("\nDevice %s removed\n", address);
		}
	}
	return;
}

static void bluez_signal_adapter_changed(GDBusConnection *conn,
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
	
	g_print("***\t Adapter Properties Changed ***\n");

	GVariantIter *properties = NULL;
	GVariantIter *unknown = NULL;
	const char *iface;
	const char *key;
	GVariant *value = NULL;
	const gchar *signature = g_variant_get_type_string(params);

	if(g_strcmp0(signature, "(sa{sv}as)") != 0) {
		g_print("Invalid signature for %s: %s != %s", signal, signature, "(sa{sv}as)");
		goto done;
	}

	g_variant_get(params, "(&sa{sv}as)", &iface, &properties, &unknown);
	while(g_variant_iter_next(properties, "{&sv}", &key, &value)) {
		if(!g_strcmp0(key, "Powered")) {
			if(!g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN)) {
				g_print("Invalid argument type for %s: %s != %s", key,
						g_variant_get_type_string(value), "b");
				goto done;
			}
			g_print("Adapter is Powered \"%s\"\n", g_variant_get_boolean(value) ? "on" : "off");
		}
		if(!g_strcmp0(key, "Discovering")) {
			if(!g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN)) {
				g_print("Invalid argument type for %s: %s != %s", key,
						g_variant_get_type_string(value), "b");
				goto done;
			}
			g_print("Adapter scan \"%s\"\n", g_variant_get_boolean(value) ? "on" : "off");
		}
	}
done:
	if(properties != NULL)
		g_variant_iter_free(properties);
	if(value != NULL)
		g_variant_unref(value);
}

static int bluez_adapter_call_method( const char *method, GVariant *param)
{
	GVariant *result;
	GError *error = NULL;

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,
					/* TODO Find the adapter path runtime */
					     BLUEZ_HCI0_PATH,
					     BLUEZ_ADAPTER_INTERFACE,
					     method,
					     param,
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
	if(error != NULL)
		return 1;

	g_variant_unref(result);
	return 0;
}

static int bluez_adapter_set_property(const char *prop, GVariant *value)
{
	GVariant *result;
	GError *error = NULL;

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     BLUEZ_HCI0_PATH,						// defined in bluez_dbus_names.h
					     "org.freedesktop.DBus.Properties",
					     "Set",
					     g_variant_new("(ssv)", BLUEZ_ADAPTER_INTERFACE, prop, value),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
	if(error != NULL)
		return -1;

	g_variant_unref(result);
	return 0;
}


bool bluez_adapter_set_filter(DiscoveryFilter * filterSettings)
{
	return true;
}


/*
TODO make sure we unref mCon somewhere in project
static void fail_cleanup()
{
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed);
	g_dbus_connection_signal_unsubscribe(mCon, iface_added);
	g_dbus_connection_signal_unsubscribe(mCon, iface_removed);
	g_object_unref(mCon);
}
*/

static int bluez_set_discovery_filter()
{
	int rc;
	GVariantBuilder *b = g_variant_builder_new(G_VARIANT_TYPE_VARDICT);
	g_variant_builder_add(b, "{sv}", "Transport", g_variant_new_string("auto"));
	g_variant_builder_add(b, "{sv}", "RSSI", g_variant_new_int16(-100));
	g_variant_builder_add(b, "{sv}", "DuplicateData", g_variant_new_boolean(FALSE));

	GVariantBuilder *u = g_variant_builder_new(G_VARIANT_TYPE_STRING_ARRAY);
	g_variant_builder_add(u, "s", "0000110a-0000-1000-8000-00805f9b34fb");
	g_variant_builder_add(b, "{sv}", "UUIDs", g_variant_builder_end(u));

	GVariant *device_dict = g_variant_builder_end(b);
	g_variant_builder_unref(u);
	g_variant_builder_unref(b);
	rc = bluez_adapter_call_method("SetDiscoveryFilter", g_variant_new_tuple(&device_dict, 1));
	if(rc) {
		g_print("Not able to set discovery filter\n");
		return 1;
	}

	return 0;
}

static void bluez_get_discovery_filter_cb(GObject *con,GAsyncResult *res,gpointer data)
{
	(void)data;
	GVariant *result = NULL;
	result = g_dbus_connection_call_finish((GDBusConnection *)con, res, NULL);
	if(result == NULL)
		g_print("Unable to get result for GetDiscoveryFilter\n");

	if(result) {
		result = g_variant_get_child_value(result, 0);
		bluez_property_value("GetDiscoveryFilter", result);
	}
	g_variant_unref(result);
}

/** TODO move to device-api file
int bluez_connect(const char * objectPath)
{
	GVariant *result;
	GError *error = NULL;

	printf("Calling Connect: %s\n", objectPath);
	
	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,
					     objectPath,
					     "org.freedesktop.DBus.Properties",
					     "Set",
					     g_variant_new("(ssv)", "org.bluez.Device1", "Trusted", g_variant_new("b", TRUE)),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
	if(error != NULL)
		return 1;

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,
					// TODO Find the adapter path runtime 
					     objectPath,
					     "org.bluez.Device1",
					     "Pair",
					     NULL,
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
						 
	if(error != NULL)
		return 1;
	
	return 0;
}
*/

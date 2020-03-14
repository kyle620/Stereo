/**
	* @file bluez_device_api.c
	* @author Kyle Van Cleave
	* @date March 14,2020
	* @brief Is meant to implement the functionality of the device-api.txt file that is described by bluez.org
	*
	* 	- The main purpose of this file is to pair, trust, and connect to new devices after powering the adapter, if any devices
	* 	  appeared in /org/hciX/dev_XX_YY_ZZ_AA_BB_CC, it is monitered using "InterfaceAdded"
	*	  signal and all the properties of the device are printed
	*	- Once a device connects a bluetooth_device is created which stores alot of properties about the remote device
	*	  
	*	- Required flags, and libs for compiling
	* 		gcc `pkg-config --cflags glib-2.0 gio-2.0` `pkg-config --libs glib-2.0 gio-2.0`
	*/
#include <stdio.h>

#include "bluez_device_api.h"
#include "bluez_dbus_names.h"
#include "bluetooth_device.h"

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
static void bluez_device_parse_properties(const char* path, const char * key, GVariant * propertyValue);
static void bluez_device_read_property(const char * path, const char *property);
static void bluez_device_get_property_cb(GObject *con,GAsyncResult *res,gpointer data);
/*
*	Private Variables
*/
GDBusConnection * mCon;

// GDBUS signals
static guint device_changed;

/*
* Accessors
*/
void bluez_device_read_remote_device_properties(const char * devicePath)
{
	bluez_device_read_property(devicePath, PROPERTY_ADDRESS);
	bluez_device_read_property(devicePath, PROPERTY_NAME);
	bluez_device_read_property(devicePath, PROPERTY_ICON);
	bluez_device_read_property(devicePath, PROPERTY_PAIRED);
	bluez_device_read_property(devicePath, PROPERTY_TRUSTED);
	bluez_device_read_property(devicePath, PROPERTY_ALIAS);
}

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
		g_error_free(error);
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
		g_error_free(error);
		succeed = false;
	}
	
	return succeed;
}

/*
 * Private Functions
*/
static void bluez_signal_device_changed(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *signal,
                    GVariant *params,
                    void *userdata)
{
    //(void)conn;
    //(void)sender;
    (void)path;
    (void)interface;
    //(void)userdata;
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
    while(g_variant_iter_next(properties, "{&sv}", &key, &value))
		bluez_device_parse_properties(path,key, value);

	g_print("***\tDevice Signal Properties Changed\t***\n");
	
	done:
		if(properties != NULL)
			g_variant_iter_free(properties);
		if(value != NULL)
			g_variant_unref(value);
}

static void bluez_device_read_property(const char * path, const char *property)
{

	GVariant *userData;
	
	GVariantBuilder *u = g_variant_builder_new(G_VARIANT_TYPE_VARDICT);
	g_variant_builder_add(u, "{sv}", "Path", g_variant_new_string(path));
	g_variant_builder_add(u, "{sv}", "Property", g_variant_new_string(property));
	userData = g_variant_builder_end(u);

	g_dbus_connection_call(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     path,						// defined in bluez_dbus_names.h
					     "org.freedesktop.DBus.Properties",
					     "Get",
					     g_variant_new("(ss)", BLUEZ_DEVICE_INTERFACE, property),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
						 NULL,
					     bluez_device_get_property_cb,
					     g_variant_new_tuple(&userData,1));
	
}

static void bluez_device_get_property_cb(GObject *con,GAsyncResult *res,gpointer  userData)
{
	GVariant *result = NULL;
	GVariant * propertyValue = NULL;
	GVariant *userDataValue;
	GVariantIter *params = NULL;
	GError * error = NULL;
	char path[100];
	char property[100];
	char * key;
	
	g_print("***\t Inside Property Callback\t***\n");
	
	// initialize the values to null
	strcpy(path,"NULL");
	strcpy(property,"NULL");
	
	// userData is a tuple containing the path and the property we asked to get
	g_variant_get((GVariant *)userData, "(a{sv})",&params);
	
	// grab the user data
    while(g_variant_iter_next(params, "{sv}", &key, &userDataValue))
	{
		if(strcmp(key,"Path") == 0)
			strcpy(path,g_variant_get_string(userDataValue,NULL));
		else if(strcmp(key,"Property") == 0)
			strcpy(property,g_variant_get_string(userDataValue,NULL));		
	}
	
	if(strcmp(path,"NULL") == 0 || strcmp(property,"NULL") == 0)
	{
		g_print("\t- Error: Invalid path or property\n");
		g_variant_unref(userDataValue);
		return;
	}
	
	// was the call successful?
	result = g_dbus_connection_call_finish((GDBusConnection *)con, res, &error);
	if(result == NULL)
	{
		g_print("\t- Unable to get result for Property: %s\n", property);
		g_print("\t- Error Reason: %s\n",error->message);
		g_error_free(error);
	}
	
	// okay lets update the property we asked for
	if(result) {
		result = g_variant_get_child_value(result, 0);
		if(g_variant_n_children(result) > 0)
			propertyValue = g_variant_get_child_value(result,0);
		
		if(propertyValue)
		{
			g_print("\t- Updating <%s>, Value <%s>\n",property,g_variant_print(propertyValue,FALSE));
			bluez_device_parse_properties(path, property, propertyValue);
		}
	}
	g_variant_unref(result);
	g_variant_unref(userDataValue);
	g_variant_unref(propertyValue);
}

static void bluez_device_parse_properties(const char * path, const char * propertyKey, GVariant * propertyValue)
{
	g_print("[ Property: '%s'  value: '%s']\n", propertyKey, g_variant_print(propertyValue,TRUE));
	
	GVariantIter iter;
	GVariant *uuid;
	BluetoothDevice currentDevice;
	
	strcpy(currentDevice.PATH,path);
	
	if(strcmp(propertyKey, "Connected") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_BOOLEAN))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "b");
		else
		{
			// Did we receive a disconnect event
			if(!g_variant_get_boolean(propertyValue))
			{
				bluetooth_device_remove_device_by_path(path);		
			}
			else
			{
				// device will only get appended if it is new, and currenlty does not exist
				bluetooth_device_add_device(&currentDevice);
				bluetooth_device_property_update_connection(path,true);
				bluez_device_read_remote_device_properties(path);
			}
		}
	}
	else if(strcmp(propertyKey, "Address") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_STRING))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "s");
		else
			bluetooth_device_property_update_address(path,g_variant_get_string(propertyValue,NULL));
	}
	else if(strcmp(propertyKey, "Paired") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_BOOLEAN))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "b");
		else
			bluetooth_device_property_update_paired(path,g_variant_get_boolean(propertyValue));
	}
	else if(strcmp(propertyKey, "Trusted") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_BOOLEAN))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "b");
		else
		{
			if(!g_variant_get_boolean(propertyValue))
			{
				// we want to Trust a device we have paired with
				// find device
				BluetoothDevice * device = NULL;
				device = bluetooth_get_device_by_path(path);
				if(device != NULL)
					if(bluetooth_device_get_property_paired(device))
						bluez_device_trust_device(path);
			}
			
			bluetooth_device_property_update_trusted(path,g_variant_get_boolean(propertyValue));
		}
	}
	else if(strcmp(propertyKey, "Alias") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_STRING))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "s");
		else
			bluetooth_device_property_update_alias(path,g_variant_get_string(propertyValue,NULL));
	}
	else if(strcmp(propertyKey, "RSSI") == 0)
	{
		if(!g_variant_is_of_type(propertyValue, G_VARIANT_TYPE_INT16))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "n");
		else
			bluetooth_device_property_update_RSSI(path,g_variant_get_int16(propertyValue));
	}
	else if(strcmp(propertyKey, "UUIDs") == 0)
	{
		if(!g_variant_is_of_type(propertyValue,G_VARIANT_TYPE_STRING_ARRAY))
            g_print("Invalid argument type for %s: %s != %s", propertyKey,g_variant_get_type_string(propertyValue), "as");
		else
		{
			g_variant_iter_init(&iter, propertyValue);
			while((uuid = g_variant_iter_next_value(&iter)))
				bluetooth_device_property_add_service_UUID(path,g_variant_get_string(uuid,NULL));
		}
	}
}

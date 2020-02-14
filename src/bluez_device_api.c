/**
* Author: Kyle Van Cleae
*/
#include <stdio.h>

#include "bluez_device_api.h"
#include "bluez_dbus_names.h"

/*
* Private Function Declerations
*/

/*
*	Private Variables
*/
GDBusConnection * mCon;

/*
* Accessors
*/


/*
* Modifiers
*
*/

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

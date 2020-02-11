/*
 * 	bluez_agent_pair_device_.c - Pair with a device found after discovering it. 
 * 	- This file pairs with a device after it had scaned for new devices after 
 *	  powering the adapter, if any devices
 * 	  appeared in /org/hciX/dev_XX_YY_ZZ_AA_BB_CC, it is monitered using "InterfaceAdded"
 *	  signal and all the properties of the device is printed
 *	- Scanning continues to run until any device is disappered, this happens after 180 seconds
 *	  automatically if the device is not used.
 * gcc `pkg-config --cflags glib-2.0 gio-2.0` -Wall -Wextra -o ./bin/bluez_adapter_scan ./bluez_adapter_scan.c `pkg-config --libs glib-2.0 gio-2.0`
 */

#include "bluez_agent_api.h"
#include "bluez_dbus_names.h"

#define AGENT_PATH "/org/bluez/AutoPinAgent"		// freely definable according to agent-api.txt

static GDBusConnection *mCon;

/*
 * Private Functions
 *
*/
static int bluez_agent_call_method(const gchar *method, GVariant *param);
static int bluez_register_autopair_agent(void);


static int bluez_agent_call_method(const gchar *method, GVariant *param)
{
        GVariant *result;
        GError *error = NULL;

        result = g_dbus_connection_call_sync(mCon,
                                             "org.bluez",
                                             "/org/bluez",
                                             "org.bluez.AgentManager1",
                                             method,
                                             param,
                                             NULL,
                                             G_DBUS_CALL_FLAGS_NONE,
                                             -1,
                                             NULL,
                                             &error);
        if(error != NULL) {
		g_print("Register %s: %s\n", AGENT_PATH, error->message);
                return 1;
	}

        g_variant_unref(result);
        return 0;
}

static int bluez_register_autopair_agent(void)
{
	int rc;
	
	 // According to the agent-api.txt, AGENT_PATH is freely definable. This could be anything as far as I know
	rc = bluez_agent_call_method("RegisterAgent", g_variant_new("(os)", AGENT_PATH, "NoInputNoOutput"));
	if(rc)
		return 1;

	rc = bluez_agent_call_method("RequestDefaultAgent", g_variant_new("(o)", AGENT_PATH));
	if(rc) {
		bluez_agent_call_method("UnregisterAgent", g_variant_new("(o)", AGENT_PATH));
		return 1;
	}

	return 0;
}

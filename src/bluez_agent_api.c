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
#include <stdio.h>
#include "bluez_agent_api.h"
#include "bluez_dbus_names.h"

#define AGENT_PATH "/org/bluez/AutoPinAgent"		// freely definable according to agent-api.txt

static GDBusConnection *mCon;

/*
 * Private Functions
 *
*/
static int bluez_agent_call_method(const gchar *method, GVariant *param);
static void bluez_agent_method_call(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *method,
                    GVariant *params,
                    GDBusMethodInvocation *invocation,
                    void *userdata);

/*
* Private Variables
*/
/*
 * typedef struct {
 * GDBusInterfaceMethodCallFunc  method_call;
 * GDBusInterfaceGetPropertyFunc get_property;
 * GDBusInterfaceSetPropertyFunc set_property;
 * } GDBusInterfaceVTable;
 * 
 * Virual table to hanle properties and method calls for D-Bus interface
 */
static const GDBusInterfaceVTable agent_method_table = {
    .method_call = bluez_agent_method_call,
};

int bluez_agent_init(GDBusConnection *conn)
{
	printf("Initializing Agent...\n");
	
	mCon = conn;
	
	if(mCon == NULL) {
		g_printerr("Not able to get connection to system bus\n");
		/**TODO Need to organize Error Codes */
		return -3;
	}
	
	return 0;
}

/* This method uses the default agent 
* 	It can be used to pair with a device using 'NoInputNoOutput'
* 	the 'NoInputNoOutput' is similear to like headphones where
*	there is no screen or keyboard on the device
*/
int bluez_register_autopair_agent(void)
{
	int rc;
	
	 // According to the agent-api.txt, AGENT_PATH is freely definable. This could be anything as far as I know
	rc = bluez_agent_call_method("RegisterAgent", g_variant_new("(os)", AGENT_PATH, "KeyboardDisplay"));
	if(rc)
		return - 1;

	rc = bluez_agent_call_method("RequestDefaultAgent", g_variant_new("(o)", AGENT_PATH));
	if(rc) {
		bluez_agent_call_method("UnregisterAgent", g_variant_new("(o)", AGENT_PATH));
		return  -2;
	}

	return 0;
}

int bluez_register_agent()
{
    GError *error = NULL;
    guint id = 0;
    GDBusNodeInfo *info = NULL;
	
	// this simulates the agent-api API found in agent-api.txt
    static const gchar bluez_agent_introspection_xml[] =
        "<node name='/org/bluez/SampleAgent'>"
        "   <interface name='org.bluez.Agent1'>"
        "       <method name='Release'>"
        "       </method>"
        "       <method name='RequestPinCode'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='pincode' direction='out' />"
        "       </method>"
        "       <method name='DisplayPinCode'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='pincode' direction='in' />"
        "       </method>"
        "       <method name='RequestPasskey'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='out' />"
        "       </method>"
        "       <method name='DisplayPasskey'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='in' />"
        "           <arg type='q' name='entered' direction='in' />"
        "       </method>"
        "       <method name='RequestConfirmation'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='u' name='passkey' direction='in' />"
        "       </method>"
        "       <method name='RequestAuthorization'>"
        "           <arg type='o' name='device' direction='in' />"
        "       </method>"
        "       <method name='AuthorizeService'>"
        "           <arg type='o' name='device' direction='in' />"
        "           <arg type='s' name='uuid' direction='in' />"
        "       </method>"
        "       <method name='Cancel'>"
        "       </method>"
        "   </interface>"
        "</node>";

    info = g_dbus_node_info_new_for_xml(bluez_agent_introspection_xml, &error);
    if(error) {
        g_printerr("Unable to create node: %s\n", error->message);
        g_clear_error(&error);
        return -2;
    }

    id = g_dbus_connection_register_object(mCon, 
            AGENT_PATH,
            info->interfaces[0],
            &agent_method_table,
            NULL, NULL, &error);
    g_dbus_node_info_unref(info);
    //g_dbus_connection_unregister_object(con, id);
    /* call register method in AgentManager1 interface */
	
	bluez_register_autopair_agent();
	
    return id;
}

/*
* Private Functions
*/

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
                return  1;
	}

        g_variant_unref(result);
        return 0;
}

/* This method is called when a mobile deivce attempts to pair with the Raspberry Pi 
*
*/
static void bluez_agent_method_call(GDBusConnection *conn,
                    const gchar *sender,
                    const gchar *path,
                    const gchar *interface,
                    const gchar *method,
                    GVariant *params,
                    GDBusMethodInvocation *invocation,
                    void *userdata)
{
    int pass;
    int entered;
    char *opath;
	char * uuid;
   
	//GVariant *p = g_dbus_method_invocation_get_parameters(invocation);

    g_print("Agent method call: %s.%s()\n", interface, method);
    if(!strcmp(method, "RequestPinCode")) {
        ;
    }
    else if(!strcmp(method, "DisplayPinCode")) {
        ;
    }
    else if(!strcmp(method, "RequestPasskey")) {
        g_print("Getting the Pin from user: ");
        if(fscanf(stdin, "%d", &pass) > 0)
			g_dbus_method_invocation_return_value(invocation, g_variant_new("(u)", pass));
    }
    else if(!strcmp(method, "DisplayPasskey")) {
        g_variant_get(params, "(ouq)", &opath, &pass, &entered);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else if(!strcmp(method, "RequestConfirmation")) {
        g_variant_get(params, "(ou)", &opath, &pass);
		g_print("Pin Code: %d\n",pass);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else if(!strcmp(method, "RequestAuthorization")) {
        g_print("Inside Request Authorization\n");
    }
    else if(!strcmp(method, "AuthorizeService")) {
		g_variant_get(params, "(os)", &opath, &uuid);
		g_print("Authorize Service UUID: %s\t YES/NO\n", uuid);
		g_dbus_method_invocation_return_value(invocation, NULL);
        ;
    }
    else if(!strcmp(method, "Cancel")) {
        ;
    }
    else
        g_print("We should not come here, unknown method\n");
}


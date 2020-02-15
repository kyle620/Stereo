#include "bluez_mediaplayer_api.h"
#include "bluez_dbus_names.h"

/*
 * Private Function Declerations
*/
static int bluez_mediaplayer_call_method( const char *method, GVariant *param);
static int bluez_mediaplayer_set_property(const char *prop, GVariant *value);
static void bluez_mediaplayer_appeared(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
/*
 * Private Variables
*/
static GDBusConnection *mCon;
static MediaPlayer mDefaultPlayer;
static guint iface_added;

/*
 * Accessors
 */
 
 /*
  * Modifiers
 */ 
 int bluez_mediaplayer_init(GDBusConnection * conn)
 {
	g_print("Initializing MediaPlayer...\n");
	
	mCon = conn;
	
	if(mCon == NULL) {
		g_printerr("Not able to get connection to system bus\n");
		/**TODO Need to organize Error Codes */
		return -3;
	}
	
	bluez_mediaplayer_init_signals();
	
	return 0;
 }
 
void bluez_mediaplayer_init_signals(void)
{
	iface_added = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.Properties",
							"PropertiesChanged",
							NULL,									// NULL so we can listen for all object paths
							BLUEZ_MediaPlayer_INTERFACE,			// defined in bluez_dbus_names.h
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_mediaplayer_appeared,
							NULL,
							NULL);
}

void bluez_mediaplayer_mute_signals(void)
{
	g_dbus_connection_signal_unsubscribe(mCon, iface_added);
}
 
 void bluez_media_player_update_remote_device_property(const char * prop_name, const char * value)
 {

	 //update MediaPlayer
	 if(strcmp(prop_name, "Equalizer"))
		 strcpy(mDefaultPlayer.EQUALIZER, value);
	 else if(strcmp(prop_name, "Repeat"))
		 strcpy(mDefaultPlayer.REPEAT, value);
	 else if(strcmp(prop_name, "Shuffle"))
		 strcpy(mDefaultPlayer.SHUFFLE, value);
	 
	 // Send command to remote device
	 bluez_mediaplayer_set_property(prop_name, g_variant_new("s", value));
 }
 
 /*
  * Control Methods
 */
void bluez_mediaplayer_play()
{
	bluez_mediaplayer_call_method("Play",NULL);
}

void bluez_mediaplayer_pause()
{
	bluez_mediaplayer_call_method("Pause",NULL);
}

void bluez_mediaplayer_stop()
{
	bluez_mediaplayer_call_method("Stop",NULL);
}

void bluez_mediaplayer_next()
{
	bluez_mediaplayer_call_method("Next",NULL);
}

void bluez_mediaplayer_previous()
{
	bluez_mediaplayer_call_method("Previous",NULL);
}

/*
 * Private Method
*/

static int bluez_mediaplayer_call_method( const char *method, GVariant *param)
{
	GVariant *result;
	GError *error = NULL;

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     mDefaultPlayer.OBJECT_PATH,
					     BLUEZ_MediaPlayer_INTERFACE,			// defined in bluez_dbus_names.h
					     method,
					     param,
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
	if(error != NULL)
	{
		g_print("Method Call Error: %s",error->message);
		return -1;
	}

	g_variant_unref(result);
	return 0;
}

static int bluez_mediaplayer_set_property(const char *prop, GVariant *value)
{
	GVariant *result;
	GError *error = NULL;

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     mDefaultPlayer.OBJECT_PATH,
					     "org.freedesktop.DBus.Properties",
					     "Set",
					     g_variant_new("(ssv)", BLUEZ_MediaPlayer_INTERFACE, prop, value),
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

static void bluez_mediaplayer_appeared(GDBusConnection *sig,
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
	//const gchar *interface_name;
	//GVariant *properties;
	
	g_print("\n****\t MediaPlayer Appeared \t****\n");
	g_print ("\t- Object Path: %s\n", object_path);
	g_variant_get(parameters, "(&sa{sv}as)", &object, &interfaces);
	
	
	/*
	while(g_variant_iter_next(interfaces, "{&s@a{sv}}", &interface_name, &properties)) 
	{
		if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) 
		{
			g_print("[ %s ]\n", object);
			const gchar *property_name;
			GVariantIter i;
			GVariant *prop_val;
			g_variant_iter_init(&i, properties);
			while(g_variant_iter_next(&i, "{&sv}", &property_name, &prop_val))
				bluez_property_value(property_name, prop_val,&newDevice);
			g_variant_unref(prop_val);
			
			
		}
		g_variant_unref(properties);
	}
	*/
	return;
}

#include "bluez_mediaplayer_api.h"
#include "bluez_dbus_names.h"

/*
 * Private Function Declerations
*/
static int bluez_mediaplayer_call_method( const char *method, GVariant *param);
static int bluez_mediaplayer_set_property(const char *prop, GVariant *value);
static void bluez_mediaplayer_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static void bluez_mediaplayer_mediacontrol_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static void bluez_property_value(const gchar *key, GVariant *value);
static void bluez_mediaplayer_print_track_data(GVariant * mediadata);
/*
 * Private Variables
*/
static GDBusConnection *mCon;
static MediaPlayer mDefaultPlayer;
static guint iface_added;
static guint prop_changed;
static guint prop_changed_control;

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
	prop_changed = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.Properties",
							"PropertiesChanged",
							NULL,									// NULL so we can listen for all object paths
							BLUEZ_MediaPlayer_INTERFACE,			// defined in bluez_dbus_names.h
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_mediaplayer_properties_changed,
							NULL,
							NULL);
							
	prop_changed_control = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.Properties",
							"PropertiesChanged",
							NULL,									// NULL so we can listen for all object paths
							BLUEZ_MediaController_INTERFACE,			// defined in bluez_dbus_names.h
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_mediaplayer_mediacontrol_properties_changed,
							NULL,
							NULL);
}

void bluez_mediaplayer_mute_signals(void)
{
	g_dbus_connection_signal_unsubscribe(mCon, iface_added);
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed);
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed_control);
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
		g_print("***\tMediaPlayer Method Call Error: %s",error->message);
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

static void bluez_mediaplayer_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data)
{
	//(void)sig;
	//(void)sender_name;
	(void)object_path;
	//(void)interface;
	//(void)signal_name;
	//(void)user_data;

	GVariantIter *intr;
	const char *object;
	const char * key;
	GVariant * value;
	GVariant * unknown;
	
	g_print("\n****\t MediaPlayer Properties Changed \t****\n");
	
	g_print ("\t- Object Path: %s\n", object_path);
	g_variant_get(parameters, "(&sa{sv}as)", &object, &intr, &unknown);

	while(g_variant_iter_next(intr, "{sv}", &key, &value)) 
		bluez_property_value(key,value);
	
	/* Do not unref, it gets cleaned up on its own */
	//g_variant_unref(intr);
	//g_variant_unref(value);
}

static void bluez_mediaplayer_mediacontrol_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data)
{
	//(void)sig;
	//(void)sender_name;
	(void)object_path;
	//(void)interface;
	//(void)signal_name;
	//(void)user_data;

	GVariantIter *intr;
	const char *object;
	const char * key;
	GVariant * value;
	GVariant * unknown;
	
	g_print("\n****\t MediaPlayer\tMedia Controller Properties Changed \t****\n");
	//g_print("Type: %s\n", g_variant_get_type_string(parameters));
	
	g_print ("\t- Object Path: %s\n", object_path);
	g_variant_get(parameters, "(&sa{sv}as)", &object, &intr, &unknown);

	while(g_variant_iter_next(intr, "{sv}", &key, &value)) 
		bluez_property_value(key,value);
	
	/* Do not unref, it gets cleaned up on its own */
	//g_variant_unref(intr);
	//g_variant_unref(value);
}

static void bluez_property_value(const gchar *key, GVariant *value)
{
	const gchar *type = g_variant_get_type_string(value);
	
	g_print("\t- %s: ", key);
	switch(*type) {
		case 'o':
		case 's':
					
			g_print("%s\n", g_variant_get_string(value, NULL));
			
			break;
		case 'b':		
			g_print("%d\n", g_variant_get_boolean(value));
			
			break;
		case 'u':
			
			g_print("%d\n", g_variant_get_uint32(value));
			
			break;
			
		case 'n':
			g_print("%d\n", g_variant_get_int16(value));
			
			break;
		case 'a':
		
			bluez_mediaplayer_print_track_data(value);
			break;
		
		default:
			g_print("Other\n");
			break;
	}
}

static void bluez_mediaplayer_print_track_data(GVariant * mediadata)
{
	GVariantIter *intr;
	GVariant * trackInfo;
	const gchar * key;
	
	g_variant_get(mediadata, "a{sv}", &intr);
	
	while(g_variant_iter_next(intr, "{sv}", &key, &trackInfo))
	{
		g_print("\n\t");
		bluez_property_value(key,trackInfo);
	}
}



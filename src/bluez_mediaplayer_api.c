#include "bluez_mediaplayer_api.h"
#include "bluez_dbus_names.h"

/*
 * Private Function Declerations
*/
static int bluez_media_player_call_method( const char *method, GVariant *param);
static int bluez_media_player_set_property(const char *prop, GVariant *value);
static int bluez_media_player_read_property(const char * path, const char *property);
static void bluez_media_player_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static void bluez_media_control_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);
static void bluez_media_player_read_property_cb(GObject *con,GAsyncResult *res,gpointer data);
static void bluez_media_player_parse_property_value(const gchar *key, GVariant *value);
static void bluez_media_control_parse_property_value(const gchar *key, GVariant *value);
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
 void bluez_media_player_print_current_player(void)
 {
	 if(!mDefaultPlayer.CONNECTED)
		 g_print("No Player exists!\n");
	 else
	 {
		 g_print("***\tCurrent Player Info\t***\n");
		 g_print("\t- Object Path:\t%s\n",mDefaultPlayer.OBJECT_PATH);
		 g_print("\t- Player Path:\t%s\n",mDefaultPlayer.PLAYER_PATH);
		 g_print("\t- Repeat:\t%s\n",mDefaultPlayer.REPEAT);
		 g_print("\t- Shuffle:\t%s\n",mDefaultPlayer.SHUFFLE);
		 g_print("\t- Status:\t%s\n",mDefaultPlayer.STATUS);
		 g_print("\t- Player Name:\t%s\n",mDefaultPlayer.PLAYER_NAME);
		 g_print("\t- Type:\t\t%s\n",mDefaultPlayer.TYPE);
	 }
	g_print("\n");
 }
 
 /*
  * Modifiers
 */ 
 int bluez_media_player_init(GDBusConnection * conn)
 {
	g_print("Initializing MediaPlayer...\n");
	
	mCon = conn;
	
	if(mCon == NULL) {
		g_printerr("Not able to get connection to system bus\n");
		/**TODO Need to organize Error Codes */
		return -3;
	}
	
	// initialize our mediaplayer object with values
	bluez_media_player_set_default_properties();
	
	bluez_media_player_init_signals();
	
	return 0;
 }
 
 void bluez_media_player_set_default_properties()
 {
	 // initialize our mediaplayer object with values
	strcpy(mDefaultPlayer.OBJECT_PATH,"NULL");
	strcpy(mDefaultPlayer.PLAYER_PATH,"NULL");
	strcpy(mDefaultPlayer.REPEAT,"off");
	strcpy(mDefaultPlayer.STATUS,"stopped");
	strcpy(mDefaultPlayer.PLAYER_NAME,"NULL");
	strcpy(mDefaultPlayer.TYPE,"Audio");
	strcpy(mDefaultPlayer.TRACK_TITLE,"--.--");
	strcpy(mDefaultPlayer.TRACK_ARTIST,"--.--");
	strcpy(mDefaultPlayer.TRACK_ALBUM,"--.--");
	strcpy(mDefaultPlayer.TRACK_GENRE,"NULL");
	mDefaultPlayer.CONNECTED = false;
	mDefaultPlayer.TRACK_NUMBER = 0;
	mDefaultPlayer.TRACK_DURATION = 0;
	mDefaultPlayer.TRACK_POSITION = 0;
 }
 
void bluez_media_player_init_signals(void)
{
	prop_changed = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.Properties",
							"PropertiesChanged",
							NULL,									// NULL so we can listen for all object paths
							BLUEZ_MediaPlayer_INTERFACE,			// defined in bluez_dbus_names.h
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_media_player_properties_changed,
							NULL,
							NULL);
							
	prop_changed_control = g_dbus_connection_signal_subscribe(mCon,
							BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
							"org.freedesktop.DBus.Properties",
							"PropertiesChanged",
							NULL,									// NULL so we can listen for all object paths
							BLUEZ_MediaController_INTERFACE,			// defined in bluez_dbus_names.h
							G_DBUS_SIGNAL_FLAGS_NONE,
							bluez_media_control_properties_changed,
							NULL,
							NULL);
}

void bluez_media_player_mute_signals(void)
{
	g_dbus_connection_signal_unsubscribe(mCon, iface_added);
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed);
	g_dbus_connection_signal_unsubscribe(mCon, prop_changed_control);
}
 
 
 void bluez_media_player_read_remote_player_properties()
 {
	 bluez_media_player_read_property(mDefaultPlayer.PLAYER_PATH,PROPERTY_REPEAT);
	 bluez_media_player_read_property(mDefaultPlayer.PLAYER_PATH,PROPERTY_SHUFFLE);
	 bluez_media_player_read_property(mDefaultPlayer.PLAYER_PATH,PROPERTY_STATUS);
	 bluez_media_player_read_property(mDefaultPlayer.PLAYER_PATH,PROPERTY_NAME);
	 bluez_media_player_read_property(mDefaultPlayer.PLAYER_PATH,PROPERTY_TYPE);
 }
 
 /*
  * Control Methods
 */
void bluez_mediaplayer_play()
{
	bluez_media_player_call_method("Play",NULL);
}

void bluez_mediaplayer_pause()
{
	bluez_media_player_call_method("Pause",NULL);
}

void bluez_mediaplayer_stop()
{
	bluez_media_player_call_method("Stop",NULL);
}

void bluez_mediaplayer_next()
{
	bluez_media_player_call_method("Next",NULL);
}

void bluez_mediaplayer_previous()
{
	bluez_media_player_call_method("Previous",NULL);
}

void bluez_mediaplayer_shuffle_on()
{
	// Send command to remote device
	 bluez_media_player_set_property(PROPERTY_SHUFFLE, g_variant_new("s", "alltracks"));
}

void bluez_mediaplayer_shuffle_off()
{
	// Send command to remote device
	 bluez_media_player_set_property(PROPERTY_SHUFFLE, g_variant_new("s", "off"));
}

void bluez_mediaplayer_repeat_singletrack()
{
	// Send command to remote device
	 bluez_media_player_set_property(PROPERTY_REPEAT, g_variant_new("s", "singletrack"));
}

void bluez_mediaplayer_repeat_alltracks()
{
	// Send command to remote device
	 bluez_media_player_set_property(PROPERTY_REPEAT, g_variant_new("s", "alltracks"));
}

void bluez_mediaplayer_repeat_off()
{
	// Send command to remote device
	 bluez_media_player_set_property(PROPERTY_REPEAT, g_variant_new("s", "off"));
}

/*
 * Private Method
*/

static int bluez_media_player_call_method( const char *method, GVariant *param)
{
	GVariant *result;
	GError *error = NULL;
	
	// only call the method if we have a valid path, meaning we are connected to a phone
	if(!mDefaultPlayer.CONNECTED)
	{
		g_print("Error: No valid player\n");
		g_print("Cannot execute command: %s\n", method);
		return -2;
	}

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     mDefaultPlayer.PLAYER_PATH,
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

static int bluez_media_player_set_property(const char *prop, GVariant *value)
{
	GVariant *result;
	GError *error = NULL;
	
	// only call the method if we have a valid path, meaning we are connected to a phone
	if(!mDefaultPlayer.CONNECTED)
	{
		g_print("Error: No valid player\n");
		g_print("Cannot update property: %s\n", prop);
		return -2;
	}

	result = g_dbus_connection_call_sync(mCon,
					     BLUEZ_BUS_NAME,						// defined in bluez_dbus_names.h
					     mDefaultPlayer.PLAYER_PATH,
					     "org.freedesktop.DBus.Properties",
					     "Set",
					     g_variant_new("(ssv)", BLUEZ_MediaPlayer_INTERFACE, prop, value),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
					     NULL,
					     &error);
	if(error != NULL)
	{
		g_print("Error: Unbale to set Property %s\tReason: %s\n",prop,error->message);
		g_error_free(error);
		return -1;
	}

	g_variant_unref(result);
	return 0;
}


static int bluez_media_player_read_property(const char * path, const char *property)
{

	if(strcmp(mDefaultPlayer.PLAYER_PATH, "NULL") == 0)
	{
		g_print("***\tMedia Player Read Propery Error: Player Path is NULL\n");
		return -3;
	}
	
	g_dbus_connection_call(mCon,
					     BLUEZ_BUS_NAME,							// defined in bluez_dbus_names.h
					     mDefaultPlayer.PLAYER_PATH,				// defined in bluez_dbus_names.h
					     "org.freedesktop.DBus.Properties",
					     "Get",
					     g_variant_new("(ss)", BLUEZ_MediaPlayer_INTERFACE, property),
					     NULL,
					     G_DBUS_CALL_FLAGS_NONE,
					     -1,
						 NULL,
					     bluez_media_player_read_property_cb,
					     property);

	return 0;
}

static void bluez_media_player_properties_changed(GDBusConnection *sig,
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
		bluez_media_player_parse_property_value(key,value);
	
	// lets grab all properites to be safe
	bluez_media_player_read_remote_player_properties();
	
	/* Do not unref, it gets cleaned up on its own */
	//g_variant_unref(intr);
	//g_variant_unref(value);
}

static void bluez_media_control_properties_changed(GDBusConnection *sig,
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
	
	g_print("\n****\tMedia Controller Properties Changed \t****\n");
	g_print ("\t- Object Path: %s\n", object_path);
	
	strcpy(mDefaultPlayer.OBJECT_PATH,object_path);
	
	g_variant_get(parameters, "(&sa{sv}as)", &object, &intr, &unknown);

	while(g_variant_iter_next(intr, "{sv}", &key, &value)) 
		bluez_media_control_parse_property_value(key,value);
	
	/* Do not unref, it gets cleaned up on its own */
	//g_variant_unref(intr);
	//g_variant_unref(value);
}

static void bluez_media_player_parse_property_value(const gchar *key, GVariant *value)
{
	const gchar *type = g_variant_get_type_string(value);
	
	g_print("\t- %s: ", key);
	switch(*type) {
		case 'o':
		case 's':
					
			g_print("%s\n", g_variant_get_string(value, NULL));
			if(strcmp(key,PROPERTY_REPEAT) == 0)
				strcpy(mDefaultPlayer.REPEAT, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_SHUFFLE) == 0)
				strcpy(mDefaultPlayer.SHUFFLE, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_STATUS) == 0)
				strcpy(mDefaultPlayer.STATUS, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_NAME) == 0)
				strcpy(mDefaultPlayer.PLAYER_NAME, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_TYPE) == 0)
				strcpy(mDefaultPlayer.TYPE, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_TRACK_TITLE) == 0)
				strcpy(mDefaultPlayer.TRACK_TITLE, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_TRACK_ARTIST) == 0)
				strcpy(mDefaultPlayer.TRACK_ARTIST, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_TRACK_ALBUM) == 0)
				strcpy(mDefaultPlayer.TRACK_ALBUM, g_variant_get_string(value,NULL));
			else if(strcmp(key,PROPERTY_TRACK_GENRE) == 0)
				strcpy(mDefaultPlayer.TRACK_GENRE, g_variant_get_string(value,NULL));
			
			break;
		case 'b':		
			g_print("%d\n", g_variant_get_boolean(value));
			
			break;
		case 'u':
			
			g_print("%d\n", g_variant_get_uint32(value));
			if(strcmp(key,PROPERTY_POSITION) == 0)
				mDefaultPlayer.TRACK_POSITION = g_variant_get_uint32(value);
			else if(strcmp(key,PROPERTY_DURATION) == 0)
				mDefaultPlayer.TRACK_DURATION = g_variant_get_uint32(value);
			else if(strcmp(key,PROPERTY_TRACK_NUMBER) == 0)
				mDefaultPlayer.TRACK_NUMBER = g_variant_get_uint32(value);
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

static void bluez_media_control_parse_property_value(const gchar *key, GVariant *value)
{
	const gchar *type = g_variant_get_type_string(value);
	
	g_print("\t- %s: ", key);
	switch(*type) {
		case 'o':
		case 's':
					
			g_print("%s\n", g_variant_get_string(value, NULL));
			if(strcmp(key,"Player") == 0)
				strcpy(mDefaultPlayer.PLAYER_PATH, g_variant_get_string(value,NULL));
			
			break;
		case 'b':		
			g_print("%d\n", g_variant_get_boolean(value));
			
			if(strcmp(key, "Connected") == 0)
			{
				if(g_variant_get_boolean(value))
					mDefaultPlayer.CONNECTED = true;		// media player connected!
				
				else
					bluez_media_player_set_default_properties();
			}
				
			
			break;
		case 'u':
			
			g_print("%d\n", g_variant_get_uint32(value));
			
			break;
			
		case 'n':
			g_print("%d\n", g_variant_get_int16(value));
			
			break;
		case 'a':
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
		bluez_media_player_parse_property_value(key,trackInfo);
	}
}

static void bluez_media_player_read_property_cb(GObject *con,GAsyncResult *res,gpointer userData)
{
	GVariant *result = NULL;
	GVariant * propertyValue = NULL;
	char propertyName[100];
	GError *error = NULL;

	g_print("***\t Media Player Inside Property Callback\t***\n");
	
	// copy the name of the property we asked for
	strcpy(propertyName,userData);
	
	// was the call successful?
	result = g_dbus_connection_call_finish((GDBusConnection *)con, res, &error);
	if(result == NULL)
	{
		g_print("\t- Unable to get result for Property: %s\n", propertyName);
		
		if(error != NULL)
		{
			g_print("\t- Error: %s\n",error->message);
			g_error_free(error);
		}
		return;
	}
	
	// okay lets update the property we asked for
	if(result) {
		result = g_variant_get_child_value(result, 0);
		if(g_variant_n_children(result) > 0)
			propertyValue = g_variant_get_child_value(result,0);
		
		if(propertyValue)
		{
			g_print("\t- Updating <%s>, Value <%s>\n",propertyName,g_variant_print(propertyValue,FALSE));
			bluez_media_player_parse_property_value(propertyName, propertyValue);
		}
	}
	g_variant_unref(result);
	g_variant_unref(propertyValue);	
}


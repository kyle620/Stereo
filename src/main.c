#include <stdlib.h>				// used for system
#include <stdio.h>				// for printf
#include <stdbool.h> 

#include "file_reader.h"
#include "bluez_adapter_api.h"

// includes to use glib and dbus
#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus.h>

#define BLUEZ_BUS_NAME 						(gchar*)"org.bluez"
#define BLUEZ_OBJ_PATH 						(gchar*)"/org/bluez"
#define BLUEZ_OBJ_ADAPTER PATH 				(gchar*) "/org/bluez/hci0"
#define BLUEZ_ADAPTER_INTERFACE 			(gchar*) "/org.bluez.Adapter1"
#define BLUEZ_AGENT_INTERFACE 				(gchar*) "org.bluez.AgentManager1"
#define OBJ_MANANAGER_INTERFACE 			(gchar*)"org.freedesktop.DBus.ObjectManager"
#define OBJ_PROPERTY_INTERFACE 				(gchar*)"org.freedesktop.DBus.Properties.Set"

static GDBusConnection * connection;
static GDBusProxy *deviceProxy;		/* Must be freed with g_object_unref when done with it */
static GError *error;

int main( int argc, char** argv )
{
	printf ("Hello!\n");
	
	char c = '1';
	char path[100];
	char fileArray[MAX_NUMBER_FILES][MAX_STRING_LEN];		// defined in file_reader.h
	
	sprintf(path,"%s","/var/lib/bluetooth/DC:A6:32:36:5C:D2/cache");
	
	listFiles(path, fileArray,false);
	
	//char devices[MAX_NUMBER_DEVICES][MAX_DEVICE_STRING_LEN];
	//bluez_start_scan(devices);
	
	bluez_adapter_set_filter_default();
	
	
	//pthread_t gdbusThread;
	//pthread_attr_t gdbusThreadAttr;
	
	//connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM,NULL,&error);
	//g_assert(connection);
	 
	//createDeviceProxy();
	  
	// create thread for gdbus 
	//pthread_attr_init( &gdbusThreadAttr );
	//pthread_attr_setdetachstate( &gdbusThreadAttr, PTHREAD_CREATE_JOINABLE );
	//pthread_create( &gdbusThread, &gdbusThreadAttr, gdbusMainLoopThread, NULL );
	//pthread_attr_destroy( &gdbusThreadAttr );
	//sleep( 1 );
	  
	while(c != 'q')
	  {
		  c = getchar();
		  usleep(200);
		  
		  if(c == 's')
		  {
			 
		  }
		  else if(c == 'l')
		  {
			 
		  }
		  else if(c == 'c')
		  {
			 
		  }
	  }
	  
	 
	 
	
	return 0;
	  
} 



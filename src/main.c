/**
* Author Kyle Van Cleave
*/

#include <stdlib.h>				// used for system
#include <stdio.h>				// for printf
#include <stdbool.h> 
#include <pthread.h>

#include "file_reader.h"
#include "bluez_adapter_api.h"
#include "bluetooth_device.h"
#include "bluez_agent_api.h"
#include "bluez_device_api.h"

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


/*
* Private Function Decleartions
*/
static void printOptions(void);
static void* gdbusMainLoopThread(void* aArg);

/* 
* Private Variables
*/
static GDBusConnection * connection;
static GDBusProxy *deviceProxy;		/* Must be freed with g_object_unref when done with it */
static GError *error;
static bool printFlag = true;

int main( int argc, char** argv )
{
	
	char c = '1';
	char userInput[100];
	char path[100];
	char fileArray[MAX_NUMBER_FILES][MAX_STRING_LEN];		// defined in file_reader.h
	
	//sprintf(path,"%s","/var/lib/bluetooth/DC:A6:32:36:5C:D2/cache");
	//listFiles(path, fileArray,false);

	pthread_t gdbusThread;
	pthread_attr_t gdbusThreadAttr;
	
	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM,NULL,&error);
	g_assert(connection);
	 
	  
	//create thread for g_main_loop
	pthread_attr_init( &gdbusThreadAttr );
	pthread_attr_setdetachstate( &gdbusThreadAttr, PTHREAD_CREATE_JOINABLE );
	pthread_create( &gdbusThread, &gdbusThreadAttr, gdbusMainLoopThread, NULL );
	pthread_attr_destroy( &gdbusThreadAttr );
	sleep( 1 );
	
	  
	while(c != 'q')
	  {
		  
		  if(printFlag)
			  printOptions();
		  
		  c = getchar();
		  usleep(200);
		  
		  if(c == '1')
		  {
			 bluez_adapter_init(connection);
			 if(bluez_adapter_power_on(true))
				 bluez_adapter_scan_on();
		  }
		  else if(c == '2')
		  {
			 bluez_adapter_scan_off();
	
		  }
		  else if(c == '3')
		  {
			bluetooth_device_print_all();
		  }
		  else if(c == '4')
		  {
			g_print("Enter the device number you want to pair with...\n");
			  bluetooth_device_print_all();
			 scanf("%s",userInput);
			 
			int x;
			
			sscanf(userInput, "%d", &x); // Using sscanf
			 
			 bluez_agent_init(connection);
			 bluez_register_autopair_agent();
			 bluez_device_init(connection);
			 
			 bluez_device_trust_device(bluetooth_get_device_path_at_index(x));
			 bluez_device_pair_device(bluetooth_get_device_path_at_index(x));
	
		  }
		  else if(c == '5')
		  {
			bluez_adapter_power_on(true);
		  }
		  else if(c == '6')
		  {
			bluez_adapter_power_off();
		  }
	  }
	  
	  bluez_adapter_deinit();
	  
	 // clean up thread
	pthread_cancel(gdbusThread);
	int returnValue = pthread_join(gdbusThread,NULL);
	g_print("Return Value: %d\n",returnValue);
	
	// clean up GDBusConnection
	g_object_unref(connection);
	
	return 0;
	  
} 


static void printOptions()
{
	g_print("***\t Options \t***\n");
	g_print(" 1:\tStart scan\n");
	g_print(" 2:\tStop scan\n");
	g_print(" 3:\tList Devices\n");
	g_print(" 4:\tPair Device\n");
	g_print(" 5:\tPower Adapter on\n");
	g_print(" 6:\tPower Adapter off\n");
	printFlag = false;
}

static void* gdbusMainLoopThread(void* aArg)
{
	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (loop);
	return 0;
}


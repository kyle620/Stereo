#ifndef DEVICE_H
#define DEVICE_H

/*
 * Author Kyle Van Cleave
*/

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#define MAX_DEVICE_STRING_LEN 	100
#define MAX_NUMBER_DEVICES 		100
#define MAX_NUMBER_UUIDS		100
#define BT_ADDRESS_STRING_SIZE 	18

struct _BluetoothDevice{
	char	SERVICE_UUIDs[MAX_NUMBER_UUIDS][37];	// List of 128-Bit UUIDs represented on device
	char 	MAC_ADDRESS[BT_ADDRESS_STRING_SIZE];	// XX:XX:XX:XX:XX:XX
	char 	ALIAS[MAX_DEVICE_STRING_LEN];			// name of device
	bool	PAIRED;									// Indicates the remove device is paired
	bool	CONNECTED;								// Indicates if remote device is currently connected
	bool	TRUSTED;								// Indicates if remote device is seen as trusted
	gint16 	RSSI;									// Receievd signal strength of remote device
	gint16  TXPOWER;								// Advertised transmitted power level
	guint32	Class;									// The Bluetooth class of device of the remote device
};

typedef struct _BluetoothDevice BluetoothDevice;

/* 
*	Accessors
*/

int bluetooth_device_get_number_devices(void);


/*
*	Modifiers
*/
int bluetooth_device_add_device(GVariant * params);						// returns 0 on success, -1 device already exists, and -2 if device list is full
bool bluetooth_device_remove_device(const char * address);			// returns false if device not found

#endif

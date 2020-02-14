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
	char	PATH[MAX_DEVICE_STRING_LEN];																				// Path according to bluez, example: /org/bluez/hci0/XX_XX_XX_XX_XX_XX
	char	SERVICE_UUIDS[MAX_NUMBER_UUIDS][37];																// List of 128-Bit UUIDs represented on device
	char 	MAC_ADDRESS[BT_ADDRESS_STRING_SIZE];																// XX:XX:XX:XX:XX:XX
	char 	ALIAS[MAX_DEVICE_STRING_LEN];																				// name of device
	bool	PAIRED;																													// Indicates the remove device is paired
	bool	CONNECTED;																											// Indicates if remote device is currently connected
	bool	TRUSTED;																													// Indicates if remote device is seen as trusted
	gint16 	RSSI;																														// Receievd signal strength of remote device
	int		NUMBER_OF_UUIDS;																								// keeps track of number of UUIDs
};

typedef struct _BluetoothDevice BluetoothDevice;

/* 
*	Accessors
*/

int bluetooth_device_get_number_devices(void);
bool bluetooth_device_already_exists(const char * address);
void bluetooth_device_print(BluetoothDevice * device);																// Prints the information out
void bluetooth_device_print_all(void);																							// prints the entire list out
/*
*	Modifiers
*/
int bluetooth_device_add_device(BluetoothDevice * newDevice);													// returns 0 on success, -1 device already exists, and -2 if device list is full
bool bluetooth_device_remove_device(const char * address);														// returns false if device not found
bool bluetooth_get_device_address_at_index(int index, char * addrContainer, bool deleteFlag);		// returns true success, deletetFlag == true, removes it from list
char * bluetooth_get_device_path_at_index(int index);		
#endif

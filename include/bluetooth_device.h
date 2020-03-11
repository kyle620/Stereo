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

typedef struct _BluetoothDevice BluetoothDevice;

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

/* 
*	Accessors
*/

int bluetooth_device_get_number_devices(void);
void bluetooth_device_print_properties(BluetoothDevice * device);																// Prints the information out
void bluetooth_device_print_all(void);
bool bluetooth_get_device_address_at_index(int index, char * addrContainer, bool deleteFlag);		// returns true success, deletetFlag == true, removes it from list
char * bluetooth_get_device_path_at_index(int index);
//int bluetooth_device_get_index(const char * path);													// returns -1 if device does not exist, ohterwise returns the array index of device
BluetoothDevice * bluetooth_get_device_at_index(int index);

// functions to get properties of the device
bool bluetooth_device_get_property_paired(BluetoothDevice * device);

// access to properties of device																						// prints the entire list out
/*
*	Modifiers
*/
bool bluetooth_device_add_device(BluetoothDevice * newDevice);													// returns 0 on success, -1 device already exists, and -2 if device list is full
bool bluetooth_device_remove_device_by_index(int index);
bool bluetooth_device_remove_device_by_path(const char * path);														// returns false if device not found
bool bluetooth_device_remove_all_devices();

// functions to update the properites of the device
bool bluetooth_device_property_add_service_UUID(const char * path, const char * uuid);			// adds the UUID to SERVICE_UUIDS array for the device located at the specified index, reutrns false if UUID already exists, or array is full
bool bluetooth_device_property_update_connection(const char * path, bool isConnected);			// updates the connection status of the device located at the index, returns false if device cannot be found
bool bluetooth_device_property_update_paired(const char * path, bool isPaired);					// updates the paired status of the device located at the index, true = is paired, false = not paired, returns false if device canot be found
bool bluetooth_device_property_update_trusted(const char * path, bool isTrusted);					// updates the trusted status of the device located at the index, true = is trusted, fasle, not trusted, returns false if device cannot be found
bool bluetooth_device_property_update_RSSI(const char * path, gint16 rssi);							// updates the rssi value of the device located at the index, returns false if device cannot be found
bool bluetooth_device_property_update_alias(const char * path, const char * name);					// updates the alias name of the device located at the index, returns false if device cannot be found (NOTE: name is truncated if it exceeds MAX_DEVICE_STRING_LEN
#endif

/**
	* @file bluetooth_device.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file holds information about remote bluetooth devices.
	* 
	* This file keeps track of devices found during a bluetooth scan,
	* and any device that connects to the raspberry pi. It can update 
	* the Bluetooth device properties
	* Files that use this file are
	* - bluez_adapter_api.h (adds devices found during scan)
	* - bluez_device_api.h  (adds devices when connections are made)
**/
#ifndef DEVICE_H
#define DEVICE_H

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#define MAX_DEVICE_STRING_LEN 	100		/**< MAX Buffer size for Strings. */
#define MAX_NUMBER_UUIDS		100		/**< MAX Buffer size for number of UUIDs to hold. */
#define BT_ADDRESS_STRING_SIZE 	18		/**< Buffer size for deivce MAC ADDRESS 'XX:XX:XX:XX:XX:XX' includes room for NULL termintating character. */

typedef struct _BluetoothDevice BluetoothDevice;

struct _BluetoothDevice{
	char	PATH[MAX_DEVICE_STRING_LEN];				/**< Path according to bluez, example: /org/bluez/hci0/XX_XX_XX_XX_XX_XX. */
	char	SERVICE_UUIDS[MAX_NUMBER_UUIDS][37];		/**< List of 128-Bit UUIDs represented on device. */
	char 	MAC_ADDRESS[BT_ADDRESS_STRING_SIZE];		/**< XX:XX:XX:XX:XX:XX. */
	char 	ALIAS[MAX_DEVICE_STRING_LEN];				/**< Name of device. */
	bool	PAIRED;										/**< Indicates the remove device is paired. */
	bool	CONNECTED;									/**< Indicates if remote device is currently connected. */
	bool	TRUSTED;									/**< Indicates if remote device is seen as trusted. */
	gint16 	RSSI;										/**< Receievd signal strength of remote device. */
	int		NUMBER_OF_UUIDS;							/**< keeps track of number of UUIDs. */
};

/** 
*	Accessors
*/

/**
       * @brief Returns the number of devices in the LinkedList
       * @return int number of devices
       */
int bluetooth_device_get_number_devices(void);

/**
       * @brief Print all members inside the struct BluetoothDevice
       * @param BluetoothDevice the device properties we want to print
       */
void bluetooth_device_print_properties(BluetoothDevice * device);

/**
       * @brief Prints out every BluetoothDevice and their properties inside the linkedlist
       */
void bluetooth_device_print_all(void);

/**
       * @brief  Gets the BluetoothDevice path at specified index from linkedlist  
       * @param index int that is a reference to head of list
       * @return string path of the BluetoothDevice
       */
char * bluetooth_get_device_path_at_index(int index);

/**
       * @brief  Gets the BluetoothDevice path at specified index from linkedlist  
       * @param index int that is a reference to head of list
       * @return string path of the BluetoothDevice
       */
BluetoothDevice * bluetooth_get_device_at_index(int index);

/**
       * @brief  Retrieves the BluetoothDevice with matching path from linkedlist  
       * @param string specified path to match with
       * @return BluetoothDevice if found, NULL othewise
       */
BluetoothDevice * bluetooth_get_device_by_path(const char * path);

// functions to get properties of the device
/**
       * @brief Returns attribute 'paired' of BluetoothDevice  
       * @param BluetoothDevice
       * @return boolean value of attribute paired
       */
bool bluetooth_device_get_property_paired(BluetoothDevice * device);

/**
       * @brief Returns attribute 'trusted' of BluetoothDevice  
       * @param BluetoothDevice
       * @return boolean value of attribute tusted
       */
bool bluetooth_device_get_property_trusted(BluetoothDevice * device);

/**
       * @brief Returns attribute 'alias' of BluetoothDevice  
       * @param BluetoothDevice
       * @return string Alias of device
       */
const char * bluetooth_device_get_property_alias(BluetoothDevice * device);

/**
       * @brief Returns attribute 'mac address' of BluetoothDevice  
       * @param BluetoothDevice
       * @return string MAC Address of device
       */
const char * bluetooth_device_get_property_address(BluetoothDevice * device);

/*
*	Modifiers
*/
/**
       * @brief Adds a BluetoothDevice . Increments Number of devices
	   * It uses BluetoothDevice.PATH to determine if device already exists.
       * @param BluetoothDevice
       * @return boolean True if succeed, false if device already exists
       */
bool bluetooth_device_add_device(BluetoothDevice * newDevice);

/**
       * @brief Removes BluetoothDevice at specified index
       * @param index integer
       * @return boolean True if succeed, false if device does not exist
       */
bool bluetooth_device_remove_device_by_index(int index);

/**
       * @brief Removes a BluetoothDevice with specified path
	   * It uses BluetoothDevice.PATH to compare against
       * @param string path 
       * @return boolean True if succeed, false if device does not exist
       */
bool bluetooth_device_remove_device_by_path(const char * path);

/**
       * @brief Removes BluetoothDevices in linkedlist
       * @return boolean True if succeed, false if list is empty
       */
bool bluetooth_device_remove_all_devices();

// functions to update the properites of the device

/**
       * @brief Adds the specified UUID to the BluetoothDevice that has matching path
	   * adds the UUID to SERVICE_UUIDS array for the device located at the specified path, reutrns false if UUID already exists, or array is full
       * @param string path
	   * @param string uuid
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_add_service_UUID(const char * path, const char * uuid);			// adds the UUID to SERVICE_UUIDS array for the device located at the specified path, reutrns false if UUID already exists, or array is full

/**
       * @brief Updates the attribute connected of the BluetoothDevice that has matching path
	   * Updates the connection status of the device located at the path, returns false if device cannot be found
       * @param string path
	   * @param boolean isConnected
       * @return boolean True if succeed, false if device not found
       */
bool bluetooth_device_property_update_connection(const char * path, bool isConnected);			// updates the connection status of the device located at the path, returns false if device cannot be found

/**
       * @brief Updates the attribute paired of the BluetoothDevice that has matching path
	   * Updates the paired status of the device located at the path, true = is paired, false = not paired, returns false if device canot be found
       * @param string path
	   * @param boolean isPaired
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_update_paired(const char * path, bool isPaired);					// updates the paired status of the device located at the path, true = is paired, false = not paired, returns false if device canot be found

/**
       * @brief Updates the attribute trusted of the BluetoothDevice that has matching path
	   * Updates the trusted status of the device located at the path, true = is trusted, fasle, not trusted, returns false if device cannot be found
       * @param string path
	   * @param string isTrusted
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_update_trusted(const char * path, bool isTrusted);				// updates the trusted status of the device located at the path, true = is trusted, fasle, not trusted, returns false if device cannot be found

/**
       * @brief Adds the atrribtue RSSI of the BluetoothDevice that has matching path
	   * Updates the rssi value of the device located at the path, returns false if device cannot be found
       * @param string path
	   * @param int16 rssi value
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_update_RSSI(const char * path, gint16 rssi);						// updates the rssi value of the device located at the path, returns false if device cannot be found

/**
       * @brief Updates the attribute alias of the BluetoothDevice that has matching path
	   * Updates the alias name of the device located at the path, returns false if device cannot be found (NOTE: name is truncated if it exceeds MAX_DEVICE_STRING_LEN
       * @param string path
	   * @param string alias name
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_update_alias(const char * path, const char * name);				// updates the alias name of the device located at the path, returns false if device cannot be found (NOTE: name is truncated if it exceeds MAX_DEVICE_STRING_LEN

/**
       * @brief Updates the attribute address of the BluetoothDevice that has matching path
	   * Updates the mac address of the device located at the path, returns false if device cannot be found
       * @param string path
	   * @param string mac address
       * @return boolean True if succeed, false if device not found, or uuid array is full
       */
bool bluetooth_device_property_update_address(const char *path, const char * address);			// updates the mac address of the device located at the path, returns false if device cannot be found

#endif

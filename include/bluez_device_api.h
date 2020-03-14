#ifndef BLUEZDEVICEAPI_H
#define BLUEZDEVICEAPI_H


/**
	* @file bluez_device_api.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file defines the methods and properties of Bluez's device-api.txt.
	* To find out more visit https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc/device-api.txt
**/

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>


// Properties of org.bluez.Device1
#define PROPERTY_ADDRESS	"Address"		// Bluetooth device address of remote device
#define PROPERTY_NAME		"Name"			// Remote device name
#define PROPERTY_ICON		"Icon"			// Proposed icon name according to the freedesktop.org
#define PROPERTY_PAIRED		"Paired"		// Indicates if the remote device is paired
#define PROPERTY_TRUSTED	"Trusted"		// Indicates if the remote device has been trusted
#define PROPERTY_ALIAS		"Alias"			// ALIAS of remote device

/*
* Accessors
*/
void bluez_device_read_remote_device_properties(const char * devicePath);		// reads the properties available in the interface org.bluez.Device1

/*
* Modifiers
*
*/
void bluez_device_init_signals(void);
void bluez_device_mute_signals(void);

/*
* Other
*/
int  bluez_device_init(GDBusConnection * conn);
bool bluez_device_trust_device(const char * path);
bool bluez_device_pair_device(const char *path);

#endif


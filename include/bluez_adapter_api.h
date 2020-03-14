#ifndef BLUEZADAPTERAPI_H
#define BLUEZADAPTERAPI_H

/**
	* @file bluez_adapter_api.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file defines the methods and properties of Bluez's adapter-api.txt.
	* 
	* For more information please refer to https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc/adapter-api.txt
**/

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#define MAX_DEVICE_STRING_LEN 	100
#define MAX_NUMBER_DEVICES 		100

/**
 * 	Helpful UUID's used to filter which devices we scan for
 *	- UUID's are 128 bits (32 Bytes) More info here https://www.bluetooth.com/specifications/assigned-numbers/service-discovery/
 *	- Base 	00000000-0000-1000-8000-00805F9B34FB
*/


#define UUID_AUDIO_SOURCE 	"0000110a-0000-1000-8000-00805f9b34fb" /**< Advanced Autio Distribution Profile Sink Defined by Bluetooth SIG*/
#define UUID_HEADSET		"00001108-0000-1000-8000-00805f9b34fb" /**< Headset Profile HSP Defined by Bluetooth Sig*/

/**
 * array{string} UUIDs
 *
 *				Filter by service UUIDs, empty means match
 *				_any_ UUID.
 *
 *				When a remote device is found that advertises
 *				any UUID from UUIDs, it will be reported if:
 *				- Pathloss and RSSI are both empty.
 *				- only Pathloss param is set, device advertise
 *				  TX pwer, and computed pathloss is less than
 *				  Pathloss param.
 *				- only RSSI param is set, and received RSSI is
 *				  higher than RSSI param.
 *
 *			int16 RSSI
 *
 *				RSSI threshold value.
 *
 *				PropertiesChanged signals will be emitted
 *				for already existing Device objects, with
 *				updated RSSI value. If one or more discovery
 *				filters have been set, the RSSI delta-threshold,
 *				that is imposed by StartDiscovery by default,
 *				will not be applied.
 *
 *			uint16 Pathloss
 *
 *				Pathloss threshold value.
 *
 *				PropertiesChanged signals will be emitted
 *				for already existing Device objects, with
 *				updated Pathloss value.
 *
 *			string Transport (Default "auto")
 *
 *				Transport parameter determines the type of
 *				scan.
 *
 *				Possible values:
 *					"auto"	- interleaved scan
 *					"bredr"	- BR/EDR inquiry
 *					"le"	- LE scan only
 *
 *				If "le" or "bredr" Transport is requested,
 *				and the controller doesn't support it,
 *				org.bluez.Error.Failed error will be returned.
 *				If "auto" transport is requested, scan will use
 *				LE, BREDR, or both, depending on what's
 *				currently enabled on the controller.
 *
 *			bool DuplicateData (Default: true)
 *
 *				Disables duplicate detection of advertisement
 *				data.
 *
 *				When enabled PropertiesChanged signals will be
 *				generated for either ManufacturerData and
 *				ServiceData everytime they are discovered.
 *
 *			bool Discoverable (Default: false)
 *
 *				Make adapter discoverable while discovering,
 *				if the adapter is already discoverable setting
 *				this filter won't do anything.
*/
struct _DiscoveryFilter{
	char 	UUID_ARRAY[10][37]; /**< Array of UUIDs to use to filter by */
	char 	TRANSPORT[10];		/**< Default 'auto' */
	bool	DUPLICATE_DATA;		/**< Default 'true' */
	bool	DISCOVERABLE;		/**< Default 'true' */
	gint16 	RSSI;				/**< RSSI Threshold value */
	int		NUM_OF_UUIDS;		/**< Keeps track of number of uuids we set */
};

typedef struct _DiscoveryFilter DiscoveryFilter;
typedef void (*bluez_call_method_callback)(GObject *, GAsyncResult *, gpointer);

/*
 * Modifiers
*/
int bluez_adapter_init(GDBusConnection * conn);
void bluez_adapter_deinit();
bool bluez_adapter_power_on(bool setPairable);		// powers on the adapter, user has option to make it pairable or not
bool bluez_adapter_power_off(void);							// powers off the adapter
bool bluez_adapter_scan_on(void);							// start scanning for devices, also makes adapter discoverable
bool bluez_adapter_scan_off(void);
bool bluez_adapter_pairable(bool value);
void bluez_adapter_init_signals(void);
void bluez_adapter_mute_signals(void);
void bluez_adapter_remove_device_found(const char * address);	// bluez caches BT devices found during a scan, this removes them, also bluez auto removes 180seconds if device is connected to

// for setting up filter for discovery settings
bool bluez_adapter_set_filter(DiscoveryFilter * filterSettings);
bool bluez_adapter_set_filter_default(void);


/*
 * Accessors
*/ 
bool bluez_is_adapter_on(void);					// returns true if adapter is powered on
bool bluez_adapter_print_filter_settings(void);
#endif

#ifndef BLUEZADAPTERAPI_H
#define BLUEZADAPTERAPI_H

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#define MAX_DEVICE_STRING_LEN 100
#define MAX_NUMBER_DEVICES 100

/*
 * 	Helpful UUID's used to filter which devices we scan for
 *	- UUID's are 128 bits (32 Bytes) More info here https://www.bluetooth.com/specifications/assigned-numbers/service-discovery/
 *	- Base 	00000000-0000-1000-8000-00805F9B34FB
*/
#define UUID_AUDIO_SOURCE 	"0000110a-0000-1000-8000-00805f9b34fb"
#define UUID_HEADSET		"00001108-0000-1000-8000-00805f9b34fb"

/*
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
	char 	UUID_ARRAY[10][37];
	char 	TRANSPORT[10];		// Default 'auto'
	bool	DUPLICATE_DATA;		// Default 'true'
	bool	DISCOVERABLE;
	gint16 	RSSI;
	guint16 PATHLOSS;
	int		UUID_ARRAY_INDEX;
};

typedef struct _DiscoveryFilter DiscoveryFilter;

/*
 * Modifiers
*/
bool bluez_adapter_power_on(void);				// powers on the adapter
bool bluez_adapter_power_off(void);				// powers off the adapter
bool bluez_adapter_scan_on(void);				// start scanning for devices, also makes adapter discoverable
bool bluez_adapter_scan_off(void);
bool bluez_adapter_init_signals(void);
bool bluez_adapter_mute_signals(void);

// for setting up filter for discovery settings
bool bluez_adapter_set_filter(DiscoveryFilter * filterSettings);
void bluez_adapter_set_filter_default(void);
/*
 * Accessors
*/ 
bool bluez_is_adapter_on(void);					// returns true if adapter is powered on

int bluez_adapter_init();
void bluez_adapter_deinit();
int bluez_start_scan();
int bluez_stop_scan();
int bluez_connect(const char * objectPath);


int bluez_get_number_devices_found();
char * bluez_get_device(int index);
void bluez_print_devices_found();
#endif

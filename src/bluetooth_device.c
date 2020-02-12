/*
 * Author Kyle Van Cleave
*/

#include "bluetooth_device.h"

/** 
* Private Variables
**/
static BluetoothDevice mBluetoothDeviceArray[MAX_NUMBER_DEVICES];

int mNumberOfDevices = 0;

/* 
*	Accessors
*/

int bluetooth_device_get_number_devices(void)
{
	return mNumberOfDevices;
}


/*
*	Modifiers
*/
int	bluetooth_device_add_device(GVariant * params)
{
	// TODO implement
	return 0;
}
bool bluetooth_device_remove_device(const char * address)
{
		return true;
}


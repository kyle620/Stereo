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
	g_print("Number of Devices: %d\n", mNumberOfDevices);
	return mNumberOfDevices;
}

bool bluetooth_device_already_exists(const gchar * address)
{
	int i;
	bool result = false;
	for(i = 0; i < mNumberOfDevices; i++)
	{
		if(strcmp(mBluetoothDeviceArray[i].MAC_ADDRESS, address) == 0)
		{
			result = true;
			break;
		}
	}
	
	return result;
}

/*
*	Modifiers
*/
int	bluetooth_device_add_device(GVariant * params, const char * address)
{
	// TODO implement
	
	
	// check if we have room to add another device
	if(mNumberOfDevices > MAX_NUMBER_DEVICES)
		return -2;
	else if(bluetooth_device_already_exists(address))
		return -1;
	else
	{
		g_print("\nDevice:	\t Adding Device %s\n",address);
		strcpy(mBluetoothDeviceArray[mNumberOfDevices++].MAC_ADDRESS,address);
	}
	
	return 0;
}
bool bluetooth_device_remove_device(const char * address)
{
		return true;
}

bool bluetooth_get_device_address_at_index(int index, char * addrContainer, bool deleteFlag)
{
	/** TODO need to implement a delete operation
	* 	Costly operation due to nature of arrays
	*	- dynamically need to readjust the size of the arrays
	*		- copy all devices but the one we are removing into a temp array 
	*		- move back over to array
	*	- Investigate using a different storage container that isn't an array
	**/
	if(index < MAX_NUMBER_DEVICES)
	{
		strcpy(addrContainer, mBluetoothDeviceArray[index].MAC_ADDRESS);
		return true;
	}
	else
		return false;
	
}	



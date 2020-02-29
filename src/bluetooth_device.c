/*
 * Author Kyle Van Cleave
*/

#include "bluetooth_device.h"
#include "double_link_list.h"
/**
* Private Function Declerations
*/
void bluetooth_device_create_device(BluetoothDevice *device);

/** 
* Private Variables
**/
static BluetoothDevice mBluetoothDeviceArray[MAX_NUMBER_DEVICES];
static Node * mHead = NULL;

int mNumberOfDevices = 0;

/* 
*	Accessors
*/

int bluetooth_device_get_number_devices(void)
{
	g_print("Number of Devices: %d\n", mNumberOfDevices);
	return mNumberOfDevices;
}

bool bluetooth_device_already_exists(const gchar * path)
{
	int i;
	bool result = false;
	for(i = 0; i < mNumberOfDevices; i++)
	{
		if(strcmp(mBluetoothDeviceArray[i].PATH, path) == 0)
		{
			result = true;
			break;
		}
	}
	
	return result;
}

void bluetooth_device_print(BluetoothDevice * device)
{
	int i;
	g_print("\n***\t\t Device Properties \t\t***\n\n");
	g_print("\t-Path:\t %s\n",device->PATH);
	g_print("\t-Alias:\t %s\n",device->ALIAS);
	g_print("\t-RSSI:\t %d\n",device->RSSI);
	g_print("\t-Address:\t %s\n",device->MAC_ADDRESS);
	g_print("\t-Paired:\t \"%s\"\n", device->PAIRED ? "True" : "False");
	g_print("\t-Trusted:\t \"%s\"\n", device->TRUSTED ? "True" : "False");
	g_print("\t-Connected:\t \"%s\"\n", device->CONNECTED ? "True" : "False");
	g_print("\t-UUIDs: {\n");
	
	for(i = 0; i < device->NUMBER_OF_UUIDS; i ++)
		g_print("\t\t%s\n",device->SERVICE_UUIDS[i]);
	
	g_print("\t\t}\n");
	g_print("\n***\t\t Device Properites \t\t***\n");
	
}

void bluetooth_device_print_all(void)
{
	int i;
	
	for(i = 0; i < mNumberOfDevices; i++)
	{
		g_print("Device %d:\n",i);
		bluetooth_device_print(&mBluetoothDeviceArray[i]);
	}

}

char * bluetooth_get_device_path_at_index(int index)
{
	if(index < MAX_NUMBER_DEVICES)
	{
		return &mBluetoothDeviceArray[index].PATH;
	}
	else
		return NULL;
}

/*
*	Modifiers
*/
int	bluetooth_device_add_device(BluetoothDevice * newDevice)
{
	// TODO implement
	
	// check if we have room to add another device
	if(mNumberOfDevices > MAX_NUMBER_DEVICES)
		return -2;
	else if(bluetooth_device_already_exists(newDevice->PATH))
		return -1;
	else
	{
		g_print("\nDevice:	\t Adding Device %s\n",newDevice->PATH);
		//bluetooth_device_create_device(newDevice);
		if(append(&mHead, newDevice))
			mNumberOfDevices++;
	}
	
	return 0;
}

bool bluetooth_device_remove_device(const char * path)
{
		return deleteNode(mHead,path);
}

bool bluetooth_device_remove_all_devices()
{
		return deleteNode(mHead,path);
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
		strcpy(addrContainer, mBluetoothDeviceArray[index].PATH);
		return true;
	}
	else
		return false;
	
}	

// functions to update the properties of a device
bool bluetooth_device_property_add_service_UUID(int * index, const char * uuid)
{
	if(&index < 0 || &index > MAX_NUMBER_DEVICES)
		return false;
}

bool bluetooth_device_property_update_connection(int * index, bool isConnected)
{
	return true;
}

bool bluetooth_device_property_update_paired(int * index, bool isPaired)
{
	return true;
}

bool bluetooth_device_property_update_trusted(int index, bool isTrusted)
{
	return true;
}

bool bluetooth_device_property_update_RSSI(int index, gint16 rssi)
{
	return true;
}

bool bluetooth_device_property_update_alias(int index, const char * name)
{
	return true;
}

/*
* Private Functions
*/

/**
*	This function will store the newley created device into this classes device array
*	The reason we store this in this file is so that the program does not have
* 	to create multiple device arrays. This is the one place that holds them
**/
void bluetooth_device_create_device(BluetoothDevice *device)
{
	int i;
	// need to copy over all contents into device array
	strcpy(mBluetoothDeviceArray[mNumberOfDevices].PATH,device->PATH);
	strcpy(mBluetoothDeviceArray[mNumberOfDevices].MAC_ADDRESS,device->MAC_ADDRESS);
	strcpy(mBluetoothDeviceArray[mNumberOfDevices].ALIAS,device->ALIAS);
	
	mBluetoothDeviceArray[mNumberOfDevices].PAIRED = device->PAIRED;
	mBluetoothDeviceArray[mNumberOfDevices].TRUSTED = device->TRUSTED;
	mBluetoothDeviceArray[mNumberOfDevices].CONNECTED = device->CONNECTED;
	mBluetoothDeviceArray[mNumberOfDevices].RSSI = device->RSSI;
	mBluetoothDeviceArray[mNumberOfDevices].NUMBER_OF_UUIDS = device->NUMBER_OF_UUIDS;
	
	for(i = 0; i < mBluetoothDeviceArray[mNumberOfDevices].NUMBER_OF_UUIDS; i++)
		strcpy(mBluetoothDeviceArray[mNumberOfDevices].SERVICE_UUIDS[i],device->SERVICE_UUIDS[i]);
	
	bluetooth_device_print(&mBluetoothDeviceArray[mNumberOfDevices]);
	
	mNumberOfDevices++;
}

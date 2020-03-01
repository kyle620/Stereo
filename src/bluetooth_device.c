/*
 * Author Kyle Van Cleave
*/

#include "bluetooth_device.h"
#include "double_link_list.h"
/**
* Private Function Declerations
*/


/** 
* Private Variables
**/
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

void bluetooth_device_print_properties(BluetoothDevice * device)
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
	printList(mHead);
}

char * bluetooth_get_device_path_at_index(int index)
{
	Node * dev = scanList(mHead,index);
	
	if(dev != NULL)
	{
		return dev->device.PATH;
	}
	else
		return NULL;
}

/*
*	Modifiers
*/
int	bluetooth_device_add_device(BluetoothDevice * newDevice)
{
	if(append(&mHead, newDevice))
	{
		g_print("\nDevice:\t Adding Device %s\n",newDevice->PATH);
		mNumberOfDevices++;
		return 0;
	}
	
	return -1;
}

bool bluetooth_device_remove_device_by_index(int index)
{
	Node * nodeToDelete = scanList(mHead,index);
	
	if(nodeToDelete != NULL)
	{
		if(deleteNode(&mHead,nodeToDelete->device.PATH))
		{
			mNumberOfDevices--;		// decrement the number of devices
			return true;
		}
	}

	return false;
}

bool bluetooth_device_remove_device_by_path(const char * path)
{
	if(deleteNode(&mHead,path))
	{
		mNumberOfDevices--;			// Decrement the number of devices
		return true;
	}
	else
		return false;
}

bool bluetooth_device_remove_all_devices()
{
	if(clearList(&mHead))
	{
		mNumberOfDevices = 0;
		return true;
	}
	
	return false;
}

bool bluetooth_get_device_address_at_index(int index, char * addrContainer, bool deleteFlag)
{
	bool result = true;
	
	/*1. Grab the node with the device we want */
	Node * dev = scanList(mHead,index);
	
	if(dev != NULL)
	{
		strcpy(addrContainer, dev->device.PATH);
		
		if(deleteFlag)
			if(deleteNode(&mHead, dev->device.PATH))
				mNumberOfDevices--;
	}
	else
		result = false;
	
	return result;
	
}	

// functions to update the properties of a device
bool bluetooth_device_property_add_service_UUID(int * index, const char * uuid)
{
	return true;
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

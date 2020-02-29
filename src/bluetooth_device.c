/*
 * Author Kyle Van Cleave
*/

#include "bluetooth_device.h"

/**
* Private Function Declerations
*/
void bluetooth_device_create_device(BluetoothDevice *device);

// functions for double linked list
void push(Node ** headRef, BluetoothDevice * newDevice);
void append(Node ** headRef, BluetoothDevice * newDevice);
void insertBefore(Node ** headRef, Node* nextNode, BluetoothDevice * newDevice);
void printList(Node * node);
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
		push(&mHead, newDevice);
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

/* Given a reference (pointer to pointer) to the head of a list  
and an int, inserts a new node on the front of the list. */
void push(Node** head_ref, BluetoothDevice * newDevice)  
{  
	g_print("Inside Push\n");
	int i;

    Node* new_node = (Node*)malloc(sizeof(Node));  

	// need to copy over all contents into device array
	strcpy(new_node->device.PATH,newDevice->PATH);
	strcpy(new_node->device.MAC_ADDRESS,newDevice->MAC_ADDRESS);
	strcpy(new_node->device.ALIAS,newDevice->ALIAS);
	
	new_node->device.PAIRED = newDevice->PAIRED;
	new_node->device.TRUSTED = newDevice->TRUSTED;
	new_node->device.CONNECTED = newDevice->CONNECTED;
	new_node->device.RSSI = newDevice->RSSI;
	new_node->device.NUMBER_OF_UUIDS = newDevice->NUMBER_OF_UUIDS;
	
	for(i = 0; i < new_node->device.NUMBER_OF_UUIDS; i++)
		strcpy(new_node->device.SERVICE_UUIDS[i],newDevice->SERVICE_UUIDS[i]);
  
    new_node->next = (*head_ref);  
    new_node->prev = NULL;  
  
    if ((*head_ref) != NULL)  
        (*head_ref)->prev = new_node;  
  
    (*head_ref) = new_node;  
	
	mNumberOfDevices++;
} 

/* Given a reference (pointer to pointer) to the head 
   of a DLL and an int, appends a new node at the end  */
void append(Node** head_ref, BluetoothDevice * newDevice) 
{ 
    /* 1. allocate node */
	Node* new_node = (Node*)malloc(sizeof(Node)); 
  
    Node* last = *head_ref; /* used in step 5*/
  
    /* 2. put in the data  */
    //new_node->data = new_data; 
  
    /* 3. This new node is going to be the last node, so 
          make next of it as NULL*/
    new_node->next = NULL; 
  
    /* 4. If the Linked List is empty, then make the new 
          node as head */
    if (*head_ref == NULL) { 
        new_node->prev = NULL; 
        *head_ref = new_node; 
        return; 
    } 
  
    /* 5. Else traverse till the last node */
    while (last->next != NULL) 
        last = last->next; 
  
    /* 6. Change the next of last node */
    last->next = new_node; 
  
    /* 7. Make last node as previous of new node */
    new_node->prev = last; 
  
    return; 
} 

/* Given a node as next_node, insert a new node before the given node */
void insertBefore(Node** head_ref, Node* next_node, BluetoothDevice * newDevice)  
{  
    /*1. check if the given next_node is NULL */
    if (next_node == NULL) {  
        g_print("The given next node cannot be NULL");  
        return;  
    }  
  
    /* 2. allocate new node */
    Node* new_node = (Node*)malloc(sizeof(Node));  
  
    /* 3. put in the data */
   // new_node->data = new_data;  
  
    /* 4. Make prev of new node as prev of next_node */
    new_node->prev = next_node->prev;  
  
    /* 5. Make the prev of next_node as new_node */
    next_node->prev = new_node;  
  
    /* 6. Make next_node as next of new_node */
    new_node->next = next_node;  
  
    /* 7. Change next of new_node's previous node */
    if (new_node->prev != NULL)  
        new_node->prev->next = new_node;  
    /* 8. If the prev of new_node is NULL, it will be 
       the new head node */
    else
        (*head_ref) = new_node; 
      
}  
  
// This function prints contents of linked list starting from the given node  
void printList(Node* node)  
{  
    Node* last;  
	g_print("\nTraversal in forward direction \n");  
    while (node != NULL) {  
        //printf(" %d ", node->data);  
        last = node;  
        node = node->next;  
    }  
  
    g_print("\nTraversal in reverse direction \n");  
    while (last != NULL) {  
       // printf(" %d ", last->data);  
        last = last->prev;  
    }  
}  

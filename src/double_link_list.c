/* 
 *Author: Kyle Van Cleave
*/
#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

#include "double_link_list.h"

/*
 * Private Helpers
*/
// transfers the BluetoothDevice data into the new node
void static transfer_bluetooth_device_data(Node * new_node, BluetoothDevice * newDevice);

/* Given a reference (pointer to pointer) to the head of a list  
and an int, inserts a new node on the front of the list. */
bool push(Node** head_ref, BluetoothDevice * newDevice)  
{  
	g_print("Double Linked List Push Method\n");
	
	/* 1. Check if node already exists */
	if(doesNodeExist((*head_ref), newDevice->PATH) != NULL)
		return false;	// Node already exists
	
	/* 2. allocate node */
    Node* new_node = (Node*)malloc(sizeof(Node));  
	
	/* 3. transfer the new device contents into Node->device */
	transfer_bluetooth_device_data(new_node,newDevice);
	
    new_node->next = (*head_ref);  
    new_node->prev = NULL;  
  
    if ((*head_ref) != NULL)  
        (*head_ref)->prev = new_node;  
  
    (*head_ref) = new_node; 
	
	return true;
} 

/* Given a reference (pointer to pointer) to the head 
   of a DLL and an int, appends a new node at the end  */
bool append(Node** head_ref, BluetoothDevice * newDevice) 
{ 
	g_print("Double Linked List Append Method\n");
	/* 1. Check if node already exists */
	if(doesNodeExist((*head_ref), newDevice->PATH) != NULL)
		return false;	// Node already exists

    /* 2. allocate node */
	Node* new_node = (Node*)malloc(sizeof(Node)); 
  
    Node* last = *head_ref; /* used in step 5*/
  
    /* 3. put in the data  */
    transfer_bluetooth_device_data(new_node, newDevice);
  
    /* 4. This new node is going to be the last node, so 
          make next of it as NULL*/
    new_node->next = NULL; 
  
    /* 5. If the Linked List is empty, then make the new 
          node as head */
    if (*head_ref == NULL) { 
        new_node->prev = NULL; 
        *head_ref = new_node; 
        return true; 
    } 
  
    /* 6. Else traverse till the last node */
    while (last->next != NULL) 
        last = last->next; 
  
    /* 7. Change the next of last node */
    last->next = new_node; 
  
    /* 8. Make last node as previous of new node */
    new_node->prev = last; 
  
    return true; 
} 

/* Given a node as next_node, insert a new node before the given node */
bool insertBefore(Node** head_ref, Node* next_node, BluetoothDevice * newDevice)  
{  
    /*1. check if the given next_node is NULL */
    if (next_node == NULL) {  
        g_print("The given next node cannot be NULL");  
        return false;  
    } 
	
	/* 2. Check if node already exists */
	if(doesNodeExist((*head_ref), newDevice->PATH) != NULL)
		return false;	// Node already exists
  
    /* 3. allocate new node */
    Node* new_node = (Node*)malloc(sizeof(Node));  
  
    /* 4. put in the data */
   transfer_bluetooth_device_data(new_node, newDevice);  
  
    /* 5. Make prev of new node as prev of next_node */
    new_node->prev = next_node->prev;  
  
    /* 6. Make the prev of next_node as new_node */
    next_node->prev = new_node;  
  
    /* 7. Make next_node as next of new_node */
    new_node->next = next_node;  
  
    /* 8. Change next of new_node's previous node */
    if (new_node->prev != NULL)  
        new_node->prev->next = new_node;  
    
	/* 9. If the prev of new_node is NULL, it will be 
       the new head node */
	else
		(*head_ref) = new_node; 
	
	return true; 
}  

 bool deleteNode(Node ** start, const char * path)
 {
	 // removes the Node that the device->Path matches the path given as parameter
	 g_print("Double Linked List Delete Method %s\n",path);
	 
	 /*1. The list is empty */
	 if(*start == NULL)
		 return false;
	 
	 /*2. The node we want to remove is the head node */
	 if(strcmp((*start)->device.PATH,path) == 0)
	 {
		 // Move head pointer to next node
		 Node * temp = *start;
		 *start = (*start)->next;
		 
		 // set the new head prev to NULL if not NULL
		 if(*start != NULL)
			(*start)->prev = NULL;
		 
		 // unallocate the memory
		 free(temp);
		 
		 return true;
	 }
	 
	 /* 3. The node we want to remove is not at the front */
	 Node * current = (*start)->next;
	 Node * previous = *start;
	 
	 while(current != NULL)
	 {
		 if(strcmp(current->device.PATH,path) == 0)
		 {
			 // found the node to delete
			previous->next = current->next;
			
			// change the next only if node to be deleted is not the last node
			if(previous->next != NULL)
				current->next->prev = previous->next;
			
			free(current);
			
			return true;
		 }
		 previous = current;
		 current = current->next;
	 }
	 
	 return false;
 }

/* unallocates all the memory of the list */
bool clearList(Node **start)
 {
	  g_print("Double Linked List Clear List\n");
	  
	 if(*start == NULL)
		 return false;
	 
	 Node * current = (*start)->next;
	 Node * temp = NULL;
	 
	 g_print("***\tRemoving %s\n",(*start)->device.PATH);
	 
	 free(*start);
	 
	 while(current != NULL)
	 {
		temp = current->next;
		g_print("***\tRemoving %s\n",current->device.PATH);
		free(current);
		current = temp;
	 }
	
	*start = NULL;
	
	 return true;
	 
 }
 
 /*
  * Accessors
*/
// This function prints contents of linked list starting from the given node  
void printList(Node* node)  
{  
	int index = 1; 
	g_print("\nList of devices\n");  
    while (node != NULL) 
	{  
        g_print("%d. %s\n", index, node->device.PATH);  
        node = node->next;
		index++;
    }  
}  

Node * scanList(Node *start, int index)
{
	if(start == NULL)
		return NULL;
	
	int count = 1;
	
	Node *previous, *current;
	current = start->next;
	previous = start;
	
	while(current != NULL && count != index)
	{
		previous = current;
		current = current->next;
		count++;
	}
	
	return previous;
}

Node * scanListByPath(Node *start, const char * path)
{
	if(start == NULL)
		return NULL;
	
	Node *current = start;
	
	while(current != NULL && strcmp(current->device.PATH, path) != 0)
		current = current->next;
	
	return current;
}

/*
 * Private Helpers
*/
void static transfer_bluetooth_device_data(Node * new_node, BluetoothDevice * newDevice)
{
	int i;
	
	// need to copy over all contents of newDevie into Nodes->device
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
}

Node * doesNodeExist(Node *head, const char * path)
{
	
	g_print("Double Linked List doesNodeExist Method\n");
	
	if(head == NULL)
	{
		g_print("%s Not found in Linked List\n",path);
		return NULL;
	}
	
	Node *current;
	current = head->next;
	
	if(strcmp(head->device.PATH,path) == 0)
		return head;

	while(current != NULL)
	{
		if(strcmp(current->device.PATH,path) == 0)
		{
			g_print("%s Found!\n", path);
			return current;
		}
		current = current->next;
	}
	
	g_print("%s Not found in Linked List\n",path);
	
	return NULL;
}

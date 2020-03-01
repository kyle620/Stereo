#ifndef DOUBLELINKLIST_H
#define DOUBLELINKLIST_H

/*
* Authour: Kyle Van Cleave
*/

#include "bluetooth_device.h"

typedef struct _Node Node;							// structure for Double Linked List

struct _Node
{
	BluetoothDevice device;
	Node * next;
	Node * prev;
};

/*
 * Modifiers
*/


bool push(Node ** headRef, BluetoothDevice * newDevice);
bool append(Node ** headRef, BluetoothDevice * newDevice);
bool insertBefore(Node ** headRef, Node* nextNode, BluetoothDevice * newDevice);
bool deleteNode(Node **start, const char * path);												// removes a node at the given char address, returns false if node does not exist
bool clearList(Node **start);

/*
* Accessors
*/
void printList(Node * node);
Node * doesNodeExist(Node *head, const char * path);				// scans the list starting from given node, returns the Node whose path matches the one we are looking for, or NULL
Node * scanList(Node *start, int index);							// traverses the list from start to end, returns the node whose index in the list matches the specified parameter
#endif


#ifndef DOUBLELINKLIST_H
#define DOUBLELINKLIST_H

/**
	* @file double_link_list.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file implements a double link list of Bluetooth Devices.
**/

#include "bluetooth_device.h"

typedef struct _Node Node;							// structure for Double Linked List

struct _Node
{
	BluetoothDevice device;		/**< BluetoothDevice struct, holds information of the bluetooth device. */
	Node * next;				/**< Pointer to next Node in the list. */
	Node * prev;				/**< Pointer to Node behind current Node. */
};

/**
 * Modifiers.
**/

/**
       * @brief Inserts a Node at beginning of list
       * @param Node that is a reference to head of list
       * @return Returns true on success, false if node already exists
       */
bool push(Node ** headRef, BluetoothDevice * newDevice);

/**
       * @brief Inserts a new Node at end of list
       * @param Node that is a reference to head of list
	   * @param BluetoothDevice 
       * @return Returns true on success, false if node already exists
       */
bool append(Node ** headRef, BluetoothDevice * newDevice);

/**
       * @brief Inserts a new Node in the list before Node specified in second paramater
       * @param head an Node that is a reference to head of list
	   * @param next an Node that we insert our new Node in front of
       * @return Returns true on success, false if node already exists
       */
bool insertBefore(Node ** headRef, Node* nextNode, BluetoothDevice * newDevice);

/**
       * @brief Deletes the Node that contains the BluetoothDevice with path from list
       * @param start an Node that is a reference to head of list
	   * @param path
       * @return Returns true on success, false if start is NULL, or BluetoothDevice with path is not found
       */
bool deleteNode(Node **start, const char * path);												// removes a node at the given char address, returns false if node does not exist

/**
       * @brief Removes all nodes from list, unallocates all the memory
       * @param start an Node that is a reference to head of list
       * @return Returns true on success, false start is NULL
       */
bool clearList(Node **start);

/**
	* Accessors.
**/
/**
       * @brief Prints contents of the linked list staring from the given node
       * @param start Node that is a reference to where begin printing list from
       */
void printList(Node * start);

/**
       * @brief Searches for a node contains a BluetoothDevice with given path
       * @param start an Node that is a reference to head of list
	   * @param path used to compare a BluetoothDevice path to
       * @return Returns a reference to the Node with the BluetoothDevice with given path, NULL otherwise
       */
Node * doesNodeExist(Node *head, const char * path);

/**
       * @brief Searches for a node at a given index
       * @param start an Node that is a reference to head of list
	   * @param index an integer that is the index we want
       * @return Returns a reference to the Node at index, or NULL otherwise
       */				
Node * scanList(Node *start, int index);							

/**
       * @brief Searches for a Node that contains a BluetoothDevice with given path
       * @param start an Node that is a reference to head of list
	   * param path a string that is the path we are looking for
       * @return Returns a reference to the Node that contains a Bluetooth Device with given path, or NULL if not found
       */
Node * scanListByPath(Node *start, const char * path);				
#endif


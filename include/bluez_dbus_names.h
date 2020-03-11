#ifndef BLUEZDBUSNAMES_H
#define BLUEZDBUSNAMES_H

/* 
 * To find out more about Bluez and the why the defines 
 * below are used refer to http://www.bluez.org/bluez-5-api-introduction-and-porting-guide/
 *
 */
#define BLUEZ_BUS_NAME		"org.bluez"
#define BLUEZ_ROOT_PATH		"/"
#define BLUEZ_BASE_PATH		"/org/bluez"

/*
 * Bluez Interfaces
 *	- These interfaces are descriped inside their respectful api.txt file
 *		The different api.txt files can be found in the Bluez.git repo
 *		https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc
*/ 
#define BLUEZ_ADAPTER_INTERFACE			"org.bluez.Adapter1"
#define BLUEZ_AGENT_INTERFACE			"org.bluez.Agent1"
#define BLUEZ_DEVICE_INTERFACE			"org.bluez.Device1"
#define BLUEZ_AgentManager_INTERFACE	"org.bluez.AgentManager1"
#define BLUEZ_MediaPlayer_INTERFACE		"org.bluez.MediaPlayer1"
#define BLUEZ_MediaController_INTERFACE "org.bluez.MediaControl1"
#define BLUEZ_MediaTrasnport_INTERFACE	"org.bluez.MediaTransport1"

/*
 * Object Paths
 *		- NOTE: For raspberry pi device on board bluetooth device is hci0
 *		If you use another bluetooth adapter like a usb dongle plugged in 
 *		back of the raspberry pi then it could be hci1,hci2,..etc
 *		Use command 'hcitool dev' to list avaible devices.
 *		Example Output
 *		- hcitool dev
 *			Devices:
 *					hci0 DC:A6:32:36:5C:D2
 */
#define BLUEZ_HCI0_PATH "/org/bluez/hci0"

#endif

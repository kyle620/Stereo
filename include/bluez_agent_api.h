#ifndef BLUEZAGENTAPI_H
#define BLUEZAGENTAPI_H

/**
	* @file bluez_agent_api.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file defines the methods and properties of Bluez's agent-api.txt.
	* For more information please refer to https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc/agent-api.txt
**/

#include <glib.h>
#include <gio/gio.h>

#define AGENT_PATH "/org/bluez/AutoPinAgent" /**< Freely definable */

int bluez_agent_init(GDBusConnection *conn);
int bluez_register_autopair_agent(void);
int bluez_register_agent(void);

#endif

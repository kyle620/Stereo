#ifndef BLUEZDEVICEAPI_H
#define BLUEZDEVICEAPI_H
/**
* Author: Kyle Van Cleae
*/

#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

/*
* Accessors
*/


/*
* Modifiers
*
*/

/*
* Other
*/
int  bluez_device_init(GDBusConnection * conn);
bool bluez_device_trust_device(const char * path);
bool bluez_device_pair_device(const char *path);

#endif


#ifndef BLUEZAGENTAPI_H
#define BLUEZAGENTAPI_H

#include <glib.h>
#include <gio/gio.h>

#define AGENT_PATH "/org/bluez/AutoPinAgent"

int bluez_agent_init(GDBusConnection *conn);
int bluez_register_autopair_agent(void);

#endif

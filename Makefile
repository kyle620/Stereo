#	$(CC) is a built-in variable already containing what you need when compiling and linking in C,
#	To avoid linker errors, you should put $(LDFLAGS) before your object files and $(LDLIBS) after.
#	$(CPPFLAGS) and $(CFLAGS) are useless here, the compilation phase is already over, it is the linking phase here.

EXE := Stereo

SRC_DIR := src
INCLUDE_DIR := include
GLIB_INCLUDE_DIR := /usr/include/glib-2.0/
GLIB_CONFIG_DIR := /usr/lib/arm-linux-gnueabihf/glib-2.0/include/		# needed to find glibconfig.h
DBUS_INCLUDE_DIR := /usr/include/dbus-1.0/
DBUS_ARCH_DIR := /usr/lib/arm-linux-gnueabihf/dbus-1.0/include/			# needed to find dbus-arch-deps.h
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -I$(INCLUDE_DIR) 		\
			-I$(GLIB_CONFIG_DIR) 	\
			-I$(GLIB_INCLUDE_DIR) 	\
			-I$(DBUS_INCLUDE_DIR)	\
			-I$(DBUS_ARCH_DIR)		
			
CFLAGS   := -O2 -Wall -Wextra -g -pipe -fstack-protector -fexceptions -D_FORTIFY_SOURCE=2

LDFLAGS  := -L$(GLIB_CONFIG_DIR)

LDLIBS   := -lglib-2.0 			\
			-lgio-2.0 			\
			-lgobject-2.0 		\
			-lbluetooth 		\
			-ldbus-1 			\
			-pthread			

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ)
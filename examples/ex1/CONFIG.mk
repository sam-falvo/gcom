INSTALLPATH	= /home/kc5tja/Projects/gcom-0.4
LIBPATH		= $(INSTALLPATH)/libraries
REGPATH		= $(INSTALLPATH)/registry
INCLUDEPATH	= $(INSTALLPATH)/include

LD		= gcc
LDFLAGS		= -shared
LDLIBS		= -L$(LIBPATH) -lcom -ldl

CC		= gcc
CCFLAGS		= -g -c -D__LINUX__

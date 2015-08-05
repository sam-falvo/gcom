include ../CONFIG.mk

MODULELIST	= alloc dll lists misc unicode init constants class
DEFINES		= -DMAX_PATH_LEN=$(LONGESTPATHSIZE)	\
		  -DREGPATH=\"$(REGPATH)/\"		\
		  -DMAX_REGKEY_LEN=$(LONGESTKEYSIZE)

all: libcom.so

clean:
	rm -rf *.o
	
distclean: clean
	rm -rf libcom.so*

.c.o:
	$(CC) $(CCFLAGS) $(DEFINES) -I$(INCLUDEPATH) -o $@ $<


libcom.so: Makefile $(MODULELIST:%=%.o)
	$(LD) $(LDFLAGS) -o libcom.so $(MODULELIST:%=%.o) $(LDLIBS)

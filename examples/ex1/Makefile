include CONFIG.mk

SRCS =	CLSID_Example1 IClassFactory_impl IDisplayText IDisplayText_impl

.c.o:
	$(CC) $(CCFLAGS) -I$(INCLUDEPATH) -o $@ $<


example1.so: $(SRCS:%=%.o)
	$(LD) $(LDFLAGS) -o example1.so $(SRCS:%=%.o) $(LDLIBS)

client: client.o CLSID_Example1.o IDisplayText.o
	$(LD) -o client client.o CLSID_Example1.o IDisplayText.o $(LDLIBS)

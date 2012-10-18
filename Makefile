SOURCES=serial_cockpit.c
OUTPUT=serial_cockpit.dylib

all: $(SOURCES)
	gcc -Wall -DAPL -ISDK/CHeaders/XPLM -m32 -shared -rdynamic -undefined dynamic_lookup -nodefaultlibs -lc -o $(OUTPUT) $(SOURCES)

install: serial_cockpit.dylib
	cp serial_cockpit.dylib /Applications/X-Plane\ 10/Resources/plugins/serial_cockpit.xpl

clean::
	rm -f $(OUTPUT)

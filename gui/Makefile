# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := nin10kitgui
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I../shared $(shell pkg-config --cflags Magick++) $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall -L../shared
LDLIBS := -lshared $(WXCPPLIBS) $(shell pkg-config --libs Magick++)
CC := g++
CXX := $(CC)
RM := rm
INSTALL := install
FILES := exporter.o \
	imageutil.o \
	nin10kitapp.o \
	nin10kit.o \
	nin10kitapp.o \
	nin10kitgui.o \
	nin10kitviewer.o \
	nin10kitviewergui.o \
	wxlogger.o \


all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean :
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUT)

# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := nin10kit
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I../shared -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall -L../shared
LDLIBS := -lshared $(WXCPPLIBS) -lMagick++ -lMagickWand -lMagickCore
CC := g++
CXX := $(CC)
RM := rm
INSTALL := install
FILES := main.o

all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean :
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUT)

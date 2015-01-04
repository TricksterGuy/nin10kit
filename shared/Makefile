# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := libshared.a
WXCPPFLAGS := $(shell wx-config --cppflags)
CPPFLAGS := $(MASTER_CPP_FLAGS) -I. -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS)
CC := g++
CXX := $(CC)
RM := rm
AR := ar
INSTALL := install

FILES := 3ds-exporter.o \
	cmd-line-parser-helper.o \
	color.o \
	cpercep.o \
	dither.o \
	ds-exporter.o \
	exportfile.o \
	fileutils.o \
	gba-exporter.o \
	gettimeofday.o \
	headerfile.o \
	image16.o \
	image32.o \
	image8.o \
	image.o \
	implementationfile.o \
	logger.o \
	lutgen.o \
	lut-exporter.o \
	map.o \
	mediancut.o \
	palette.o \
	scanner.o \
	scene.o \
	shared.o \
	sprite.o \
	tile.o \
	tileset.o \

all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(AR) rcs $(OUTPUT) $^ 

clean :
	$(RM) -rf *.o $(OUTPUT)


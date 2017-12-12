####################################################################################################
# Capture Makefile
#
# I've got no idea how to do all this, but this file works for me. Anyone interested in sorting
# it out, please do!
#
# This project depends on wxMac, libgig and portaudio. 
#
####################################################################################################

# Here's a few things you need to change

# for the include paths
LIBGIG_HOME=/usr/local/Cellar/libgig/3.3.0
PORTAUDIO_HOME=/usr/local/Cellar/portaudio/19.6.0

# static libraries
LIBGIG_A=/usr/local/Cellar/libgig/3.3.0/lib/libgig.a
LIBPORTAUDIO_A=$(PORTAUDIO_HOME)/lib/libportaudio.a

# the rest should be ok

INCLUDES=-Isrc/ -I$(LIBGIG_HOME) -I$(PORTAUDIO_HOME)/include/ $(shell /usr/local/Cellar/wxmac/3.0.3.1_1/bin/wx-config --cxxflags)

## for some reason these flags cause ui events to stop working
LANGFLAGS=-Wall -Wundef -Wno-ctor-dtor-privacy -O2 -g -fno-strict-aliasing -fno-common


ARCHFLAGS= -D__MACOSX_CORE__
OSFLAGS=
FRAMEWORKS= -framework CoreAudio -framework CoreFoundation -framework AudioToolbox -framework CoreServices -framework AudioUnit -framework CoreMIDI
LIBS = $(shell /usr/local/Cellar/wxmac/3.0.3.1_1/bin/wx-config --libs)  $(LIBGIG_A) $(LIBPORTAUDIO_A) $(FRAMEWORKS)

SOURCES := $(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all: $(OBJECTS)
	g++ $(OBJECTS) -o Capture $(LIBS) $(ARCHFLAGS) $(OSFLAGS)

# this is the old one
#	Rez -t APPL Carbon.r res/sample.r -o Capture
	
	#Rez -d __DARWIN__ -t APPL -d __WXMAC__ -i src  -i /usr/local/etc/wxMac-2.8.9/include -i ../lib/src/portaudio/include/ -i ../lib/src/libgig -o Capture Carbon.r res/sample.r
	SetFile -a C Capture
	mkdir Capture.app
	mkdir Capture.app/Contents
	mkdir Capture.app/Contents/Resources
	mv Capture Capture.app/Contents
	cp res/Info.plist Capture.app/Contents/
	echo -n "APPL????" >Capture.app/Contents/PkgInfo
	#ln -f Capture bombCapture.app/Contents/MacOS/Capture
	cp -f res/icon.icns Capture.app/Contents/Resources/wxmac.icns
	
## builds the object files out of the cpp files ##
%.o: %.cpp
	g++ $(INCLUDES) -c $(LANGFLAGS)  $(ARCHFLAGS) $(OSFLAGS) $+ -o $@
clean:
	rm $(OBJECTS)
	rm -Rf Capture.app



#g++-L/usr/local/etc/wxMac-2.8.9/osx-build/lib     -lwx_macu_core-2.8  -lwx_base_carbonu-2.8   -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8 -framework WebKit 
#g++ -L/usr/local/etc/wxMac-2.8.9/osx-build/lib  -lwxregexu-2.8 -lwxexpat-2.8 -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8  -L/Developer/SDKs/MacOSX10.4u.sdk/usr/lib 




	
#/usr/local/etc/wxMac-2.8.9/osx-build/bk-deps g++ -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -c -o bombs_bombs1.o -D__WXMAC__     -I../../../demos/bombs  -I../../../demos/bombs/../../samples -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I/usr/local/etc/wxMac-2.8.9/osx-build/lib/wx/include/mac-unicode-release-static-2.8 -I../../../include -fpascal-strings -I../../../src/mac/carbon/morefilex -I/Developer/Headers/FlatCarbon -Wall -Wundef -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing -arch ppc -arch i386 -fno-common ../../../demos/bombs/bombs1.cpp
#/usr/local/etc/wxMac-2.8.9/osx-build/bk-deps g++ -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -c -o bombs_game.o -D__WXMAC__     -I../../../demos/bombs  -I../../../demos/bombs/../../samples -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I/usr/local/etc/wxMac-2.8.9/osx-build/lib/wx/include/mac-unicode-release-static-2.8 -I../../../include -fpascal-strings -I../../../src/mac/carbon/morefilex -I/Developer/Headers/FlatCarbon -Wall -Wundef -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing -arch ppc -arch i386 -fno-common ../../../demos/bombs/game.cpp
#g++ -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -o bombs  bombs_bombs.o bombs_bombs1.o bombs_game.o  -arch ppc -arch i386  -framework IOKit -framework Carbon -framework Cocoa -framework System -framework QuickTime -framework OpenGL -framework AGL    -L/usr/local/etc/wxMac-2.8.9/osx-build/lib    -lz -lpthread -liconv -lwx_macu_core-2.8  -lwx_base_carbonu-2.8   -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8 -framework WebKit   -lwxregexu-2.8 -lwxexpat-2.8 -arch ppc -arch i386  -framework IOKit -framework Carbon -framework Cocoa -framework System -framework QuickTime -framework OpenGL -framework AGL  -lz -lpthread -liconv 
#Rez -d __DARWIN__ -t APPL -d __WXMAC__     -i ../../../demos/bombs  -i ../../../demos/bombs/../../samples  -i ../../../include -o bombs Carbon.r sample.r
#SetFile -a C bombs
#mkdir -p bombs.app/Contents
#mkdir -p bombs.app/Contents/MacOS
#mkdir -p bombs.app/Contents/Resources
#sed -e "s/IDENTIFIER/`echo ../../../demos/bombs | sed -e 's,\.\./,,g' | sed -e 's,/,.,g'`/" \
#	-e "s/EXECUTABLE/bombs/" \
#	-e "s/VERSION/2.8.9/" \
#	../../../src/mac/carbon/Info.plist.in >bombs.app/Contents/Info.plist
#echo -n "APPL????" >bombs.app/Contents/PkgInfo
#ln -f bombs bombs.app/Contents/MacOS/bombs
#cp -f ../../../src/mac/carbon/wxmac.icns bombs.app/Contents/Resources/wxmac.icns

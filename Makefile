PLUGIN_NAME = wave_maker

RTXI_INCLUDES =

HEADERS = wave-maker.h\

SOURCES = wave-maker.cpp\
         moc_wave-maker.cpp
		
LIBS = -lrtplot

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile

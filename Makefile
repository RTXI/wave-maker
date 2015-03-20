
PLUGIN_NAME = wave_maker

HEADERS = wave_maker.h\
          include/plotdialog.h\
          include/basicplot.h\
          include/scrollzoomer.h\
          include/scrollbar.h

SOURCES = wave_maker.cpp\
          moc_wave_maker.cpp\
          include/plotdialog.cpp\
          include/basicplot.cpp\
          include/scrollzoomer.cpp\
          include/scrollbar.cpp\
          include/moc_scrollbar.cpp\
          include/moc_scrollzoomer.cpp\
          include/moc_basicplot.cpp\
          include/moc_plotdialog.cpp
		
LIBS = -lqwt

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile

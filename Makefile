PLUGIN_NAME = wave_maker

RTXI_INCLUDES = /usr/local/lib/rtxi_includes

HEADERS = wave-maker.h\
			 $(RTXI_INCLUDES)/plotdialog.h\
			 $(RTXI_INCLUDES)/basicplot.h\
			 $(RTXI_INCLUDES)/scrollzoomer.h\
			 $(RTXI_INCLUDES)/scrollbar.h\

SOURCES = wave-maker.cpp\
          moc_wave-maker.cpp\
			 $(RTXI_INCLUDES)/plotdialog.cpp\
			 $(RTXI_INCLUDES)/basicplot.cpp\
			 $(RTXI_INCLUDES)/scrollzoomer.cpp\
			 $(RTXI_INCLUDES)/scrollbar.cpp\
			 $(RTXI_INCLUDES)/moc_plotdialog.cpp\
			 $(RTXI_INCLUDES)/moc_basicplot.cpp\
			 $(RTXI_INCLUDES)/moc_scrollzoomer.cpp\
			 $(RTXI_INCLUDES)/moc_scrollbar.cpp\
		
LIBS = -lqwt

### Do not edit below this line ###

include $(shell rtxi_plugin_config --pkgdata-dir)/Makefile.plugin_compile

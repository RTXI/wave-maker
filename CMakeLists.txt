cmake_minimum_required(VERSION 3.14)

project(
    wave-maker
    VERSION 0.1.0
    DESCRIPTION "Generate output waveform from input file"
    HOMEPAGE_URL "https://rtxi.org/"
    LANGUAGES CXX
)

# These lines help with third-party tooling integration
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RTXI_CMAKE_SCRIPTS "" CACHE PATH "In development mode, RTXI uses this to expose conan dependencies")
set(CMAKE_PREFIX_PATH "${RTXI_CMAKE_SCRIPTS}")
set(RTXI_PACKAGE_PATH "/usr/local/" CACHE PATH "Path hint to RTXI package information")
set(CMAKE_INSTALL_RPATH "${RTXI_PACKAGE_PATH}/lib")
list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ---- find libraries ----
find_package(rtxi REQUIRED HINTS ${RTXI_PACKAGE_PATH})
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets HINTS ${RTXI_CMAKE_SCRIPTS})
find_package(fmt REQUIRED)
find_package(qwt REQUIRED)
find_package(Boost REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


#################################################################################################
### You can modify within this region for finding additional libraries and linking them to your #
### custom plugin. Make sure to install them prior to configuration or else build will fail     #
### with linking and include errors!                                                            # 
#################################################################################################
add_library(
    wave-maker MODULE
    widget.cpp
    widget.hpp
)

# Consult library website for how to link them to your plugin using cmake
target_link_libraries(wave-maker PUBLIC 
    rtxi::rtxi rtxi::rtxidsp rtxi::rtxigen rtxi::rtxififo rtxi::rtxiplot Qt5::Core Qt5::Gui Qt5::Widgets 
    Boost::headers dl fmt::fmt qwt::qwt
)

################################################################################################ 

# We need to tell cmake to use the c++ version used to compile the dependent library or else...
get_target_property(REQUIRED_COMPILE_FEATURE rtxi::rtxi INTERFACE_COMPILE_FEATURES)
target_compile_features(wave-maker PRIVATE ${REQUIRED_COMPILE_FEATURE})

install(
    TARGETS wave-maker
    DESTINATION ${RTXI_PACKAGE_PATH}/bin/rtxi_modules
)


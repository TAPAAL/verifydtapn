cmake_minimum_required(VERSION 3.16)

## Default to release builds, must be set before project
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Project Definition
set(VERIFYDTAPN_NAME "verifydtapn")
set(VERIFYDTAPN_VERSION 3.4.0)
project(${VERIFYDTAPN_NAME} VERSION ${VERIFYDTAPN_VERSION} LANGUAGES CXX C)


# C++ Definition
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Default Values
## Build Flags
option(VERIFYDTAPN_Static "Link libraries statically" ON)
option(VERIFYDTAPN_GetDependencies "Fetch external dependencies from web." ON)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external CACHE PATH "Install location for external dependencies")
set(VERIFYDTAPN_TARGETDIR "${CMAKE_BINARY_DIR}/${VERIFYDTAPN_NAME}" CACHE PATH "Traget directory for build files")

# Configure Project

## Configure Static
if (VERIFYDTAPN_Static)
    set(BUILD_SHARED_LIBS OFF)
    if (NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    else () 
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif ()

## Configure Traget Platform 
if (UNIX AND NOT APPLE)
    set(ARCH_TYPE "linux64")
elseif (APPLE)
    set(ARCH_TYPE "osx64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET ${VERIFYPN_OSX_DEPLOYMENT_TARGET})
else ()
    set(ARCH_TYPE "win64")
endif ()

## Enabled INTERPROCEDURAL_OPTIMIZATION for non-macos
if (CMAKE_BUILD_TYPE MATCHES Release)
    if (NOT APPLE)
    	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif ()
endif ()

## Target File location
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VERIFYDTAPN_TARGETDIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VERIFYDTAPN_TARGETDIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VERIFYDTAPN_TARGETDIR}/bin)

# Required Packages and Tools

find_package(FLEX 2.6.0 REQUIRED)
find_package(BISON 3.5.0 REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.66 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

if (VERIFYDTAPN_GetDependencies)
    include(ExternalProject)
    # Install location should be ${EXTERNAL_INSTALL_LOCATION}

    ExternalProject_add(rapidxml-ext
        URL https://downloads.sourceforge.net/project/rapidxml/rapidxml/rapidxml%201.13/rapidxml-1.13.zip
        URL_HASH SHA512=6c10583e6631ccdb0217d0a5381172cb4c1046226de6ef1acf398d85e81d145228e14c3016aefcd7b70a1db8631505b048d8b4f5d4b0dbf1811d2482eefdd265
        BUILD_COMMAND ""
        CONFIGURE_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/external/include
        INSTALL_COMMAND cd ../rapidxml-ext && ${CMAKE_COMMAND} -E copy rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp ${EXTERNAL_INSTALL_LOCATION}/include
    )

    ExternalProject_add(unfoldtacpn-ext 
        GIT_REPOSITORY https://github.com/TAPAAL/unfoldTACPN.git
        GIT_TAG v0.0.1
	    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}  -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DFLEX_EXECUTABLE=${FLEX_EXECUTABLE} -DBISON_EXECUTABLE=${BISON_EXECUTABLE}
    )
    include_directories(${EXTERNAL_INSTALL_LOCATION}/include)

endif ()

# Set Macros
add_compile_definitions(VERIFYDTAPN_VERSION=\"${VERIFYDTAPN_VERSION}\")


# Source
include_directories(include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/)

# Testing
#if (VERIFYTAPN_TEST)
#    enable_testing()
#    add_subdirectory(${CMAKE_SOURCE_DIR}/test)
#endif()
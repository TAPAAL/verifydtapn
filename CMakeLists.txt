cmake_minimum_required(VERSION 3.31)

set(DEFAULT_GCC_VERSION 16)

## Default to release builds, must be set before project
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Project Definition
set(VERIFYDTAPN_NAME "verifydtapn")
set(VERIFYDTAPN_VERSION 3.6.2)


# Select GCC compiler
set(DESIRED_GCC_VERSION "$ENV{GCC_VERSION}")
if (DESIRED_GCC_VERSION STREQUAL "")
    set(DESIRED_GCC_VERSION ${DEFAULT_GCC_VERSION})
endif()

if (NOT CMAKE_C_COMPILER)
    find_program(GCC_REQ_C NAMES "gcc-${DESIRED_GCC_VERSION}" gcc REQUIRED)
    set(CMAKE_C_COMPILER "${GCC_REQ_C}" CACHE STRING "C compiler")
endif()

if (NOT CMAKE_CXX_COMPILER)
    find_program(GCC_REQ_CXX NAMES "g++-${DESIRED_GCC_VERSION}" g++ REQUIRED)
    set(CMAKE_CXX_COMPILER "${GCC_REQ_CXX}" CACHE STRING "CXX compiler")
endif()

project(${VERIFYDTAPN_NAME} VERSION ${VERIFYDTAPN_VERSION} LANGUAGES CXX C)

if (NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "GCC is required, but found ${CMAKE_C_COMPILER_ID} and ${CMAKE_CXX_COMPILER_ID}")
endif()

if (NOT CMAKE_CROSSCOMPILING)
    if (NOT CMAKE_C_COMPILER_VERSION MATCHES "^${DESIRED_GCC_VERSION}([.].*)?$")
        message(FATAL_ERROR "GCC ${DESIRED_GCC_VERSION} is required, but found GCC ${CMAKE_C_COMPILER_VERSION}")
    endif()
    if (NOT CMAKE_CXX_COMPILER_VERSION MATCHES "^${DESIRED_GCC_VERSION}([.].*)?$")
        message(FATAL_ERROR "G++ ${DESIRED_GCC_VERSION} is required, but found G++ ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
endif()


# C++ Definition
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Default Values
## Build Flags
option(VERIFYDTAPN_Static "Link libraries statically" ON)
option(VERIFYDTAPN_GetDependencies "Fetch external dependencies from web." ON)
option(VERIFYDTAPN_TEST "Build tests" OFF)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external CACHE PATH "Install location for external dependencies")
set(VERIFYDTAPN_TARGETDIR "${CMAKE_BINARY_DIR}/${VERIFYDTAPN_NAME}" CACHE PATH "Target directory for build files")
set(VERIFYDTAPN_OSX_DEPLOYMENT_TARGET 10.8 CACHE STRING "Specify the minimum version of the target platform for MacOS on which the target binaries are to be deployed ")

# Configure Project

## Configure Static
if (VERIFYDTAPN_Static AND NOT BUILD_TESTING)
    set(BUILD_SHARED_LIBS OFF)
    if (NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    else ()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()

## Configure Target Platform
if (UNIX AND NOT APPLE)
    set(ARCH_TYPE "linux64")
elseif (APPLE)
    set(ARCH_TYPE "osx64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET ${VERIFYDTAPN_OSX_DEPLOYMENT_TARGET})
else ()
    set(ARCH_TYPE "win64")
endif ()

## Enabled INTERPROCEDURAL_OPTIMIZATION for non-macos
if (CMAKE_BUILD_TYPE MATCHES Release)
    if (NOT APPLE AND UNIX)
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

if (VERIFYDTAPN_GetDependencies)
    include(ExternalProject)
    include(ProcessorCount)
    ProcessorCount(DEPENDENCY_JOBS)
    if (NOT DEPENDENCY_JOBS)
        set(DEPENDENCY_JOBS 1)
    endif()
    # Install location should be ${EXTERNAL_INSTALL_LOCATION}

    set(BOOST_PROGRAM_OPTIONS_LIBRARY
        ${EXTERNAL_INSTALL_LOCATION}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_program_options${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(BOOST_UNIT_TEST_FRAMEWORK_LIBRARY
        ${EXTERNAL_INSTALL_LOCATION}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_unit_test_framework${CMAKE_STATIC_LIBRARY_SUFFIX})

    ExternalProject_add(rapidxml-ext
        URL https://downloads.sourceforge.net/project/rapidxml/rapidxml/rapidxml%201.13/rapidxml-1.13.zip
        URL_HASH SHA512=6c10583e6631ccdb0217d0a5381172cb4c1046226de6ef1acf398d85e81d145228e14c3016aefcd7b70a1db8631505b048d8b4f5d4b0dbf1811d2482eefdd265
        BUILD_COMMAND ""
        CONFIGURE_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/external/include
        INSTALL_COMMAND cd ../rapidxml-ext && ${CMAKE_COMMAND} -E copy rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp ${EXTERNAL_INSTALL_LOCATION}/include
    )

    ExternalProject_add(unfoldtacpn-ext
        GIT_REPOSITORY https://github.com/TAPAAL/unfoldTACPN
        GIT_TAG main #5948ffff5a084eccef7c2758b8df371b170fa68d 
	    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}  -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DCMAKE_BUILD_TYPE=Release -DFLEX_EXECUTABLE=${FLEX_EXECUTABLE} -DBISON_EXECUTABLE=${BISON_EXECUTABLE}
    )

    set(BOOST_TOOLSET gcc)
    set(BOOST_CONFIG_JAM "using ${BOOST_TOOLSET} : : ${CMAKE_CXX_COMPILER}")
    set(BOOST_B2_OPTS "")

    if (ARCH_TYPE MATCHES "win64")
        set(BOOST_B2_OPTS target-os=windows binary-format=pe abi=ms threading=multi threadapi=win32)
        set(SPARSEHASH_HOST --host x86_64-w64-mingw32 )
    endif ()

    if (CMAKE_CROSSCOMPILING)
        set(BOOST_CONFIGURE_COMMAND ./bootstrap.sh --prefix=${CMAKE_BINARY_DIR}/external --without-icu)
    else ()
        set(BOOST_CONFIGURE_COMMAND
            ./tools/build/src/engine/build.sh --cxx=${CMAKE_CXX_COMPILER} gcc
            COMMAND ${CMAKE_COMMAND} -E copy tools/build/src/engine/b2 b2
            COMMAND ./bootstrap.sh --with-bjam=./b2 --with-toolset=gcc --prefix=${CMAKE_BINARY_DIR}/external --without-icu)
    endif ()

    ExternalProject_Add(
        boost-ext
        URL https://archives.boost.io/release/1.92.0/source/boost_1_92_0.zip
        URL_HASH SHA256=c46baee75ed4b558cf6553630d0615a147ed582812917b57e6f0839d1954549f
        PATCH_COMMAND echo "${BOOST_CONFIG_JAM}" $<SEMICOLON>  > config.jam
        CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND}
        BUILD_COMMAND ./b2 -j${DEPENDENCY_JOBS} --user-config=config.jam --prefix=${CMAKE_BINARY_DIR}/external --with-program_options --with-test --layout=system toolset=${BOOST_TOOLSET} cxxstd=${CMAKE_CXX_STANDARD} cflags="-fPIC" cxxflags="-fPIC" address-model=64 ${BOOST_B2_OPTS} variant=release link=static runtime-link=static install
        BUILD_BYPRODUCTS ${BOOST_PROGRAM_OPTIONS_LIBRARY} ${BOOST_UNIT_TEST_FRAMEWORK_LIBRARY}
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
    )


    ExternalProject_Add( sparsehash-ext
	    URL https://github.com/sparsehash/sparsehash/archive/sparsehash-2.0.4.zip
	    CONFIGURE_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ./configure --prefix=${CMAKE_BINARY_DIR}/external CXXFLAGS=-std=c++11 ${SPARSEHASH_HOST}
	    BUILD_COMMAND ""
	    INSTALL_COMMAND make install-data
            BUILD_IN_SOURCE 1
    )

    set(CMAKE_PREFIX_PATH ${EXTERNAL_INSTALL_LOCATION};${CMAKE_PREFIX_PATH})
    link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
    include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
endif ()


if(VERIFYDTAPN_GetDependencies)
    add_library(Boost::program_options STATIC IMPORTED)
    set_target_properties(Boost::program_options PROPERTIES
        IMPORTED_LOCATION ${BOOST_PROGRAM_OPTIONS_LIBRARY})
else()
    set(Boost_USE_RELEASE_LIBS ON)
    set(Boost_USE_MULTITHREADED OFF)
    set(Boost_USE_STATIC_LIBS ${VERIFYDTAPN_Static})
    find_package(Boost 1.66 REQUIRED COMPONENTS program_options)
    include_directories(${Boost_INCLUDE_DIR})
endif()

# Set Macros
add_compile_definitions(VERIFYDTAPN_VERSION=\"${VERIFYDTAPN_VERSION}\")
add_compile_definitions(VERIFYDTAPN_NAME=\"verifydtapn-${ARCH_TYPE}\")



# Source
include_directories(include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/)


if(BUILD_TESTING AND VERIFYDTAPN_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0069 NEW)

set(CMAKE_CXX_STANDARD 17)
if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(VERIFYPN_VERSION 3.3)
add_compile_definitions(VERIFYDTAPN_VERSION=\"${VERIFYDTAPN_VERSION}\")

option(VERIFYDTAPN_Static "Link libraries statically" ON)
option(VERIFYDTAPN_GetDependencies "Fetch external dependencies from web." ON)

project(verifydtapn VERSION ${VERIFYPN_VERSION} LANGUAGES CXX C)

if (VERIFYDTAPN_Static)
    set(BUILD_SHARED_LIBS OFF)
    if (NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif(NOT APPLE)
else(VERIFYDTAPN_Static)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif (VERIFYDTAPN_Static)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -O3 -DNDEBUG")
if (VERIFYPN_Static AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (UNIX AND NOT APPLE)
    set(ARCH_TYPE "linux64")
elseif(APPLE)
    set(ARCH_TYPE "osx64")
else()
    set(ARCH_TYPE "win64")
endif ()

if (VERIFYDTAPN_GetDependencies)
    # setup for external imports
    include(ExternalProject)
    set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

    ExternalProject_add(rapidxml-ext
        URL https://downloads.sourceforge.net/project/rapidxml/rapidxml/rapidxml%201.13/rapidxml-1.13.zip
        URL_HASH SHA512=6c10583e6631ccdb0217d0a5381172cb4c1046226de6ef1acf398d85e81d145228e14c3016aefcd7b70a1db8631505b048d8b4f5d4b0dbf1811d2482eefdd265
        BUILD_COMMAND ""
        CONFIGURE_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/external/include
        INSTALL_COMMAND cd ../rapidxml-ext && ${CMAKE_COMMAND} -E copy rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp ${EXTERNAL_INSTALL_LOCATION}/include
    )
    include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
endif(VERIFYDTAPN_GetDependencies)

include_directories(include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/)

#
# Gnoproj - gnomonic projection software
#
# Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
# Please read <http://foxel.ch/license> for more information.
#
#
# Author(s):
#
# Stéphane Flotron <s.flotron@foxel.ch>
#
#
# This file is part of the FOXEL project <http://foxel.ch>.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
#
# Additional Terms:
#
# You are required to preserve legal notices and author attributions in
# that material or in the Appropriate Legal Notices displayed by works
# containing it.
#
# You are required to attribute the work as explained in the "Usage and
# Attribution" section of <http://foxel.ch/license>.
#

# ==============================================================================
#  CMake base
# ==============================================================================

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(gnoproj C CXX)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BUILD_TYPE Release)

set(GNOPROJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ==============================================================================
#  C / C++ flags
# ==============================================================================
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    set(cxx_flags "-std=c++11")
endif()

if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings} ${cxx_flags}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# ==============================================================================
# OpenMP detection (needed by libgnomonic)
# ==============================================================================
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 ENDIF (OPENMP_FOUND)

# ==============================================================================
# OpenCV / pkgconfig  detection
# ==============================================================================
find_package(PkgConfig)
find_package(OpenCV REQUIRED)

# ==============================================================================
# libtiff / libxml2  detection (needed by elphelphg)
# ==============================================================================
pkg_check_modules(PC_LIBTIFF QUIET libtiff)
set(LIBTIFF_DEFINITIONS ${PC_LIBTIFF_CFLAGS_OTHER})

find_path(LIBTIFF_INCLUDE_DIR tiff.h
  HINTS ${PC_LIBTIFF_INCLUDEDIR} ${PC_LIBTIFF_INCLUDE_DIRS}
  PATH_SUFFIXES libtiff )
find_library(LIBTIFF_LIBRARY NAMES tiff libtiff
  HINTS ${PC_LIBTIFF_LIBDIR} ${PC_LIBTIFF_LIBRARY_DIRS} )

# libxml2
pkg_check_modules(PC_LIBXML QUIET libxml-2.0)
set(LIBXML2_DEFINITIONS ${PC_LIBXML_CFLAGS_OTHER})

find_path(LIBXML2_INCLUDE_DIR libxml/xpath.h
          HINTS ${PC_LIBXML_INCLUDEDIR} ${PC_LIBXML_INCLUDE_DIRS}
          PATH_SUFFIXES libxml2 )

find_library(LIBXML2_LIBRARY NAMES xml2 libxml2
             HINTS ${PC_LIBXML_LIBDIR} ${PC_LIBXML_LIBRARY_DIRS} )

# ==============================================================================
# Boost  detection (needed by elphelphg)
# ==============================================================================
pkg_check_modules(PC_LIBBOOST QUIET libboost-filesystem)
set(LIBBOOST_DEFINITIONS ${PC_LIBBOOST_CFLAGS_OTHER})

find_path(LIBBOOST_INCLUDE_DIR boost/filesystem.hpp
  HINTS ${PC_LIBBOOST_INCLUDEDIR} ${PC_LIBBOOST_INCLUDE_DIRS}
          PATH_SUFFIXES boost )

find_library(LIBBOOST_FILESYSTEM_LIBRARY NAMES boost_filesystem libboost_filesystem
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )
find_library(LIBBOOST_SYSTEM_LIBRARY NAMES boost_system libboost_system
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )
find_library(LIBBOOST_REGEX_LIBRARY NAMES boost_regex libboost_regex
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )

# ==============================================================================
#  submodules Lignomonic and libinter
# ==============================================================================
add_custom_target(
  libgnomonic
  COMMAND make
  WORKING_DIRECTORY ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic
)
set(LIBGNOMONIC_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/src)
set(LIBGNOMONIC_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/bin/libgnomonic.a)

# libinter
set(LIBINTER_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/lib/libinter/src)
set(LIBINTER_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/lib/libinter/bin/libinter.a)

# ==============================================================================
# submodules elphelphg
# ==============================================================================
add_custom_target(
  elphelphg
  WORKING_DIRECTORY ${GNOPROJ_SOURCE_DIR}/../lib/elphelphg
  COMMAND cmake ./src/
  COMMAND make
)

set(LIBELPHELPHG_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/elphelphg/src)
set(LIBELPHELPHG_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/elphelphg/elphelphg/libelphelphg.so)

# ==============================================================================
# Include directories
# ==============================================================================
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${LIBELPHELPHG_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
  ${LIBGNOMONIC_INCLUDE_DIR}
  ${LIBINTER_INCLUDE_DIR}
  ${LIBBOOST_INCLUDE_DIR}
  ${LIBTIFF_INCLUDE_DIR}
)

# ==============================================================================
# Library linkage
# ==============================================================================
set(GNOPROJ_LIBRARY_LIST
  ${LIBELPHELPHG_LIBS}
  ${OpenCV_LIBS}
  ${LIBXML2_LIBRARY}
  ${LIBGNOMONIC_LIBS}
  ${LIBBOOST_SYSTEM_LIBRARY}
  ${LIBBOOST_FILESYSTEM_LIBRARY}
  ${LIBBOOST_REGEX_LIBRARY}
  ${LIBINTER_LIBS}
  ${LIBTIFF_LIBRARY}
 #  ${CMAKE_THREAD_LIBS_INIT}
)

# ==============================================================================
# Build executable
# ==============================================================================
add_executable(gnoproj gnoproj.cpp)
target_link_libraries(gnoproj
  ${GNOPROJ_LIBRARY_LIST}
)

add_dependencies(gnoproj libgnomonic elphelphg)

install(TARGETS gnoproj DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")

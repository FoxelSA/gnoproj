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
# along with this program. if not, see <http://www.gnu.org/licenses/>.
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
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BUILD_TYPE Release)

project(gnoproj C CXX)

# ==============================================================================
#  C / C++ flags
# ==============================================================================
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra ")
    set(cxx_flags "-std=c++11")
endif()

if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings} ${cxx_flags}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# ==============================================================================
# add a target to generate API documentation with Doxygen
# ===============================================================================
find_package(Doxygen)
if(DOXYGEN_FOUND)
  add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

# ==============================================================================
# OpenMP detection (needed by libgnomonic)
# ==============================================================================
find_package(OpenMP)
if (OPENMP_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 endif (OPENMP_FOUND)

# ==============================================================================
# OpenCV / pkgconfig  detection
# ==============================================================================
find_package(PkgConfig)
find_package(OpenCV REQUIRED)

# ------------------------------------------------------------------------------
# stlplus
# ------------------------------------------------------------------------------
add_subdirectory(lib)
set(LIBSTL_LIBS ${PROJECT_BINARY_DIR}/lib/stlplus3/libstlplus.a)

# ==============================================================================
#  submodules Lignomonic and libinter
# ==============================================================================
add_custom_target(
  libgnomonic
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgnomonic
)
set(LIBGNOMONIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgnomonic/src)
set(LIBGNOMONIC_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgnomonic/bin/libgnomonic.a)

# libinter
set(LIBINTER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgnomonic/lib/libinter/src)
set(LIBINTER_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgnomonic/lib/libinter/bin/libinter.a)

# ==============================================================================
#  submodules libfastcal
# ==============================================================================
add_custom_target(
  libfastcal
  COMMAND make clean-all && make [OPENMP=false]
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfastcal
)
set(LIBFASTCAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfastcal/src)
set(LIBFASTCAL_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfastcal/bin/libfastcal.a)

set(GNOPROJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ==============================================================================
# Include directories
# ==============================================================================
include_directories(
  ${GNOPROJ_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${LIBGNOMONIC_INCLUDE_DIR}
  ${LIBINTER_INCLUDE_DIR}
  ${LIBFASTCAL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/cmdLine
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/stlAddition
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/stlplus3/filesystemSimplified/
)

# ==============================================================================
# Library linkage
# ==============================================================================
set(GNOPROJ_LIBRARY_LIST
  ${OpenCV_LIBS}
  ${LIBGNOMONIC_LIBS}
  ${LIBINTER_LIBS}
  ${LIBFASTCAL_LIBS}
  ${LIBSTL_LIBS}
)

# ==============================================================================
# Build executable
# ==============================================================================
add_subdirectory(src)

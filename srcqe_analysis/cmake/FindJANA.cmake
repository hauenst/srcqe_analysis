
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindJANA
--------

Finds the JANA library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``JANA::jana``
  The JANA library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``JANA_FOUND``
  True if the system has the JANA library.
``JANA_VERSION``
  The version of the JANA library which was found.
``JANA_INCLUDE_DIRS``
  Include directories needed to use JANA.
``JANA_LIBRARIES``
  Libraries needed to link to JANA.

#]=======================================================================]

if (DEFINED JANA_HOME)
    set(JANA_ROOT_DIR ${JANA_HOME})
    message(STATUS "Using JANA_HOME = ${JANA_ROOT_DIR} (From CMake JANA_HOME variable)")

elseif (DEFINED ENV{JANA_HOME})
    set(JANA_ROOT_DIR $ENV{JANA_HOME})
    message(STATUS "Using JANA_HOME = ${JANA_ROOT_DIR} (From JANA_HOME environment variable)")

else()
    message(FATAL_ERROR "Missing $JANA_HOME")
endif()

set(JANA_VERSION 2)

find_path(JANA_INCLUDE_DIR
        NAMES "JANA/JApplication.h"
        PATHS ${JANA_ROOT_DIR}/include
        )

find_library(JANA_LIBRARY
        NAMES "JANA"
        PATHS ${JANA_ROOT_DIR}/lib
        )

set(JANA_LIBRARIES ${JANA_LIBRARY})
set(JANA_INCLUDE_DIRS ${JANA_ROOT_DIR}/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JANA
        FOUND_VAR JANA_FOUND
        VERSION_VAR JANA_VERSION
        REQUIRED_VARS JANA_ROOT_DIR JANA_INCLUDE_DIR JANA_LIBRARY
        )

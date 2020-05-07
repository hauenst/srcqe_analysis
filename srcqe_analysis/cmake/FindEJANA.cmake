
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindEJANA
--------

Finds the EJANA library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``EJANA::EJANA``
  The EJANA library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``EJANA_FOUND``
  True if the system has the EJANA library.
``EJANA_VERSION``
  The version of the EJANA library which was found.
``EJANA_INCLUDE_DIRS``
  Include directories needed to use EJANA.
``EJANA_LIBRARIES``
  Libraries needed to link to EJANA.

#]=======================================================================]

if (DEFINED EJANA_HOME)
    set(EJANA_ROOT_DIR ${EJANA_HOME})
    message(STATUS "Using EJANA_HOME = ${EJANA_ROOT_DIR} (From CMake EJANA_HOME variable)")

elseif (DEFINED ENV{EJANA_HOME})
    set(EJANA_ROOT_DIR $ENV{EJANA_HOME})
    message(STATUS "Using EJANA_HOME = ${EJANA_ROOT_DIR} (From EJANA_HOME environment variable)")

else()
    message(FATAL_ERROR "Missing $EJANA_HOME")

endif()

set(EJANA_VERSION 1)

find_path(EJANA_INCLUDE_DIR NAMES "ejana/EJanaRootApplication.h" PATHS ${EJANA_ROOT_DIR}/include)
find_library(EJANA_LIBRARY NAMES "ejana" PATHS ${EJANA_ROOT_DIR}/lib)
message(STATUS "EJANA_LIBRARY ${EJANA_LIBRARY}")

find_library(EJANA_FMT_LIBRARY NAMES "ejana_fmt" PATHS ${EJANA_ROOT_DIR}/lib)

set(EJANA_LIBRARIES ${EJANA_LIBRARY})
set(EJANA_INCLUDE_DIRS ${EJANA_ROOT_DIR}/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EJANA
        FOUND_VAR EJANA_FOUND
        VERSION_VAR EJANA_VERSION
        REQUIRED_VARS EJANA_ROOT_DIR EJANA_INCLUDE_DIR EJANA_LIBRARY
        )

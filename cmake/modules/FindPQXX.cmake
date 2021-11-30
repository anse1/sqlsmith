# Distributed under the GPL-3.0 as part of SQLSmith.
#[=======================================================================[.rst:
FindPQXX
-------

Finds the libpqxx library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``PQXX::PQXX``
  The libpqxx library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``PQXX_FOUND``
  True if the system has the libpqxx library.
``PQXX_VERSION``
  The version of the libpqxx library which was found.
``PQXX_INCLUDE_DIRS``
  Include directories needed to use libpqxx.
``PQXX_LIBRARIES``
  Libraries needed to link to libpqxx.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``PQXX_INCLUDE_DIR``
  The directory containing ``foo.h``.
``PQXX_LIBRARY``
  The path to the Foo library.

#]=======================================================================]

find_package(PkgConfig)
pkg_check_modules(PC_PQXX QUIET libpqxx)

find_path(PQXX_INCLUDE_DIR
        NAMES pqxx
        PATHS ${PC_PQXX_INCLUDE_DIRS}
        )

find_library(PQXX_LIBRARY
        NAMES pqxx
        PATHS ${PC_PQXX_LIBRARY_DIRS}
        )

set(PQXX_VERSION ${PC_PQXX_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PQXX
        FOUND_VAR PQXX_FOUND
        REQUIRED_VARS
            PQXX_LIBRARY
            PQXX_INCLUDE_DIR
        VERSION_VAR PQXX_VERSION
        )

if (PQXX_FOUND)
    set(PQXX_LIBRARIES ${PQXX_LIBRARY})
    set(PQXX_INCLUDE_DIRS ${PQXX_INCLUDE_DIR})
    set(PQXX_DEFINITIONS ${PC_PQXX_CFLAGS_OTHER})

    add_library(PQXX::PQXX UNKNOWN IMPORTED)
    set_target_properties(PQXX::PQXX PROPERTIES
            IMPORTED_LOCATION "${PQXX_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${PC_PQXX_CFLAGS_OTHER}"
            INTERFACE_INCLUDE_DIRECTORIES "${PQXX_INCLUDE_DIR}"
            )
endif ()


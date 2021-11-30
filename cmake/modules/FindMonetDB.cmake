# Distributed under the GPL-3.0 as part of SQLSmith.
#[=======================================================================[.rst:
FindMonetDB
-------

Finds the monetdb-mapi library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``MonetDB::mapi``
  The monetdb-mapi library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``MonetDB_FOUND``
  True if the system has the monetdb-mapi library.
``MonetDB_VERSION``
  The version of the monetdb-mapi library which was found.
``MonetDB_INCLUDE_DIRS``
  Include directories needed to use monetdb-mapi.
``MonetDB_LIBRARIES``
  Libraries needed to link to monetdb-mapi.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``MonetDB_INCLUDE_DIR``
  The directory containing ``foo.h``.
``MonetDB_LIBRARY``
  The path to the Foo library.

#]=======================================================================]

find_package(PkgConfig)
pkg_check_modules(PC_MonetDB QUIET monetdb-mapi)

find_path(MonetDB_INCLUDE_DIR
        NAMES mapi.h
        PATHS ${PC_MonetDB_INCLUDE_DIRS}
        PATH_SUFFIXES monetdb
        )

find_library(MonetDB_LIBRARY
        NAMES mapi
        PATHS ${PC_MonetDB_LIBRARY_DIRS}
        )

set(MonetDB_VERSION ${PC_MonetDB_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MonetDB
        FOUND_VAR MonetDB_FOUND
        REQUIRED_VARS
            MonetDB_LIBRARY
            MonetDB_INCLUDE_DIR
        VERSION_VAR MonetDB_VERSION
        )

if (MonetDB_FOUND)
    set(MonetDB_LIBRARIES ${MonetDB_LIBRARY})
    set(MonetDB_INCLUDE_DIRS ${MonetDB_INCLUDE_DIR})
    set(MonetDB_DEFINITIONS ${PC_MonetDB_CFLAGS_OTHER})

    add_library(MonetDB::mapi UNKNOWN IMPORTED)
    set_target_properties(MonetDB::mapi PROPERTIES
            IMPORTED_LOCATION "${MonetDB_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${PC_MonetDB_CFLAGS_OTHER}"
            INTERFACE_INCLUDE_DIRECTORIES "${MonetDB_INCLUDE_DIR}"
            )
endif ()


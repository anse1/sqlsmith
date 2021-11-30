cmake_minimum_required(VERSION 3.14)
project(SQLsmith LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(FeatureTests)

Option(USE_UPSTREAM_LIBPQXX "Fetch and use libpqxx from Github.")



# define our executable early, so we can set the properties, based on our dependencies
add_executable(sqlsmith)
target_sources(sqlsmith PRIVATE
        relmodel.cc schema.cc random.cc prod.cc expr.cc grammar.cc log.cc dump.cc impedance.cc sqlsmith.cc postgres.cc)

target_link_libraries(sqlsmith PostgreSQL::PostgreSQL)

# Workaround for outdated cmake packages on debian
set(PostgreSQL_ADDITIONAL_VERSIONS 11 12 13 14)
find_package(PostgreSQL REQUIRED)

if (NOT USE_UPSTREAM_LIBPQXX)
    # If we use the system pqxx library, we have to check for libpq.
    # This check is done by the CMakeLists.txt of the libpqxx project otherwise
    find_package(PQXX REQUIRED)

    if (PQXX_VERSION GREATER 7.0)
        set(HAVE_LIBPQXX7 TRUE)
    endif (PQXX_VERSION GREATER 7.0)

    target_link_libraries(sqlsmith PQXX::PQXX)
else (NOT USE_UPSTREAM_LIBPQXX)
    set(SKIP_BUILD_TEST TRUE)
    include(FetchContent)
    FetchContent_Declare(
            libpqxx
            GIT_REPOSITORY https://github.com/jtv/libpqxx.git
            GIT_TAG 7.6.0
    )
    FetchContent_MakeAvailable(libpqxx)
    FetchContent_GetProperties(libpqxx)
    unset(SKIP_BUILD_TEST)

    set(HAVE_LIBPQXX7 TRUE)

    target_include_directories(sqlsmith PRIVATE ${libpqxx_SOURCE_DIR}/include)
    target_link_libraries(sqlsmith pqxx)
endif (NOT USE_UPSTREAM_LIBPQXX)

if(NOT std_regex_ok)
    find_package(Boost REQUIRED COMPONENTS regex)
    set(REGEX_LIBRARY Boost::regex)
    set(HAVE_BOOST TRUE)
    set(HAVE_BOOST_REGEX TRUE)
endif(NOT std_regex_ok)

## optional dependencies
# sqlite
find_package(SQLite3)
if (SQLite3_FOUND)
    set(HAVE_LIBSQLITE3 TRUE)
    target_sources(sqlsmith PRIVATE sqlite.cc)
    target_link_libraries(sqlsmith SQLite::SQLite3)
endif ()

# monetdb
find_package(MonetDB)
if (MonetDB_FOUND)
    set(HAVE_MONETDB 1)
    target_sources(sqlsmith PRIVATE monetdb.cc)
    target_link_libraries(sqlsmith MonetDB::mapi)
endif ()


## target configuration
string(TOLOWER ${PROJECT_NAME} PACKAGE)
set(PACKAGE_NAME ${PROJECT_NAME})
configure_file(config_h.cmake.in config.h)


# get our git tag
set(GIT_TAG UNRELEASED)
execute_process(
        COMMAND git describe --dirty --tags --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# our gitrev.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gitrev.h "#define GITREV \"${GIT_TAG}\"")

# make sure our target finds gitrev.h and config.h
target_include_directories(sqlsmith PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

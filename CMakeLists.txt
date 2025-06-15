# Set default build type
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release) # options are: Release, Debug, RelWithDebInfo, MinSizeRel
ENDIF()

SET(BUILD_TYPE ${CMAKE_BUILD_TYPE})

CMAKE_MINIMUM_REQUIRED (VERSION 3.10)
PROJECT (vscode-ncproj CXX)

# Application info
SET (APPLICATION_NAME "NC Project")
SET (APPLICATION_CODENAME "${PROJECT_NAME}")
SET (APPLICATION_COPYRIGHT_YEARS "2016-2025")
SET (APPLICATION_VERSION_MAJOR 0)
SET (APPLICATION_VERSION_MINOR 1)
SET (APPLICATION_VERSION_PATCH 0)
SET (APPLICATION_VERSION_TYPE "${BUILD_TYPE}")

SET (APPLICATION_VENDOR_ID "JRMC")
SET (APPLICATION_VENDOR_NAME "Janusz Rupar, Marcin Cieślik")
SET (APPLICATION_VENDOR_URL "https://github.com/jmr1/vscode-ncproj.git")
SET (APPLICATION_ID "${APPLICATION_VENDOR_ID}.${PROJECT_NAME}")

# Policies
IF(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
ENDIF()

INCLUDE (${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/DetectDistro.cmake)
detect_distro()

IF (CMAKE_CROSSCOMPILING)
  SET (AUTORUN_TESTS FALSE)
ENDIF()

MESSAGE(STATUS "Running on ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "Build type: ${BUILD_TYPE}")
MESSAGE(STATUS "CMake Build type: ${CMAKE_BUILD_TYPE}")

SET (SHOW_STATS FALSE)

# Library names
set(LIBPARSER_NAME parser)
set(LIBPARSER_NAME_SHARED ${LIBPARSER_NAME})
set(LIBPARSER_NAME_STATIC ${LIBPARSER_NAME})
SET (LIBPARSER_NAME_UT ${LIBPARSER_NAME}-ut)
SET (NCLANGSRV_NAME nclangsrv)

# Build options: Enable both static and shared libs as needed
SET(project_build_static OFF)    # Set to 1 to build static library
SET(project_build_shared ON)    # Set to 1 to build shared library

# Build verbose mode
SET (CMAKE_VERBOSE_MAKEFILE 0)

# Output paths
SET (MAINFOLDER ${PROJECT_SOURCE_DIR})
SET (PLUGIN_OUT_FOLDER "${MAINFOLDER}/vscode-client/out")
SET (EXECUTABLE_OUTPUT_PATH "${PLUGIN_OUT_FOLDER}")
SET (LIBRARY_OUTPUT_PATH "${PLUGIN_OUT_FOLDER}")

# Install paths
SET (EXECUTABLE_INSTALL_PATH "bin")
SET (LIBRARY_INSTALL_PATH "bin")

# Search paths
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/tools")
LIST (APPEND CMAKE_MODULE_PATH "${MAINFOLDER}/tools/cmake")
INCLUDE_DIRECTORIES("${MAINFOLDER}/parser" "${MAINFOLDER}/parser/fanuc")

# Locate prerequisites
FIND_PACKAGE (GCCVersion 10.0.0 REQUIRED)
FIND_PACKAGE (Threads REQUIRED)
FIND_PACKAGE (Boost 1.56 REQUIRED)

IF (NOT project_build_static)
  ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  FIND_PACKAGE(VALGRIND REQUIRED)
  SET(VALGRIND_CONFIG --leak-check=full)
ENDIF()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

# Verify C++20 support
INCLUDE (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
IF (COMPILER_SUPPORTS_CXX20)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
ELSE()
  MESSAGE(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++20 support. Please use a different compiler.")
ENDIF()

# Use ccache on Linux
IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
ENDIF()

# Compiler flags
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fmessage-length=0 -rdynamic -Wno-multichar -Wfatal-errors -Wno-write-strings")
ELSE()
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fmessage-length=0 -Wno-multichar -Wfatal-errors -Wno-write-strings -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-class-memaccess")
ENDIF()

IF (project_build_static)
  # Usually don't force -static globally for static libs, but keep if needed
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -ffunction-sections -fdata-sections -Wl,--gc-sections")
ENDIF()

IF (SHOW_STATS)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report")
ENDIF()

SET (CMAKE_CXX_OPTIMIZATION_FLAGS "-mavx2")

IF (NOT BUILD_COVERAGE)
  SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_OPTIMIZATION_FLAGS}")
  SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_OPTIMIZATION_FLAGS}")
  SET (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${CMAKE_CXX_OPTIMIZATION_FLAGS}")
ENDIF()

SET (CMAKE_BUILD_TYPE "${BUILD_TYPE}")

# Coverage targets (if GNU compiler and coverage enabled)
IF (CMAKE_COMPILER_IS_GNUCXX AND BUILD_COVERAGE)
  include(CodeCoverage)
  APPEND_COVERAGE_COMPILER_FLAGS()
  SETUP_TARGET_FOR_COVERAGE_LCOV(NAME ${LIBPARSER_NAME_UT}-coverage-lcov
                                EXECUTABLE ${PLUGIN_OUT_FOLDER}/${LIBPARSER_NAME_UT} 
                                DEPENDENCIES ${PLUGIN_OUT_FOLDER}/${LIBPARSER_NAME_UT}
                                EXCLUDE "/usr/include/*" "${PROJECT_SOURCE_DIR}/parser_ut/*")

  setup_target_for_coverage_gcovr_html(NAME ${LIBPARSER_NAME_UT}-coverage-gcovr 
                                EXECUTABLE ${PLUGIN_OUT_FOLDER}/${LIBPARSER_NAME_UT} 
                                DEPENDENCIES ${PLUGIN_OUT_FOLDER}/${LIBPARSER_NAME_UT}
                                EXCLUDE "/usr/include/*" "${PROJECT_SOURCE_DIR}/parser_ut/*")
ENDIF()

enable_testing()

# Add subdirectories
ADD_SUBDIRECTORY(parser)
ADD_SUBDIRECTORY(nclangsrv)
ADD_SUBDIRECTORY(parser_ut)

# Clean extra files on make clean
SET_PROPERTY(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${PLUGIN_OUT_FOLDER}/.debug")

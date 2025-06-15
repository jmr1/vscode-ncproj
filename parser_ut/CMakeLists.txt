include(FetchContent)

# Disable install for GTest, use it as a subproject
set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# Collect test sources
FILE(GLOB_RECURSE test_SRCS *.cpp *.cc *.cxx *.C)
LIST(REMOVE_ITEM test_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp)

# Define test binary and required libraries
SET(test_BIN ${LIBPARSER_NAME_UT})
SET(test_LIBS ${LIBPARSER_NAME} ${Boost_LIBRARIES} gmock gmock_main gtest gtest_main)

ADD_EXECUTABLE(${test_BIN} ${test_SRCS})

# Link to either shared or static parser
IF(project_build_shared)
  ADD_DEPENDENCIES(${test_BIN} ${LIBPARSER_NAME_SHARED})
ELSE()
  ADD_DEPENDENCIES(${test_BIN} ${LIBPARSER_NAME_STATIC})
ENDIF()

TARGET_COMPILE_OPTIONS(${test_BIN} PRIVATE -Wno-restrict -Wno-error)
TARGET_LINK_LIBRARIES(${test_BIN} PRIVATE ${test_LIBS})

# Leak check command setup
IF(LEAKCHECK)
  IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
  ENDIF()
  SET(TEST_EXEC_CMD ${VALGRIND_PROGRAM} ${VALGRIND_CONFIG}
    --xml=yes
    --xml-file=${PROJECT_SOURCE_DIR}/output/${LIBPARSER_NAME}-ut-leakcheck.xml
    ${PLUGIN_OUT_FOLDER}/${test_BIN})
ELSE()
  SET(TEST_EXEC_CMD ${ENV} ${PLUGIN_OUT_FOLDER}/${test_BIN})
ENDIF()

# Register test with CTest
ADD_TEST(NAME ${LIBPARSER_NAME}-ut-test COMMAND ${PLUGIN_OUT_FOLDER}/${test_BIN})

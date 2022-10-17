find_package(GMock REQUIRED)

FILE (GLOB_RECURSE test_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
LIST(REMOVE_ITEM test_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp ${CMAKE_CURRENT_SOURCE_DIR}/targetver.h ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)
SET (test_LIBS ${LIBPARSER_NAME} ${Boost_LIBRARIES} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
SET (test_BIN ${LIBPARSER_NAME_UT})
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

include (CTest)
ADD_EXECUTABLE(${test_BIN} ${test_SRCS})
IF (project_build_shared)
  ADD_DEPENDENCIES(${test_BIN} ${LIBPARSER_NAME_SHARED})
ELSE (project_build_shared)
  ADD_DEPENDENCIES(${test_BIN} ${LIBPARSER_NAME_STATIC})
ENDIF (project_build_shared)
TARGET_LINK_LIBRARIES(${test_BIN} ${test_LIBS})

IF (LEAKCHECK)
  IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
    FILE (MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
  ENDIF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
  SET (TEST_EXEC_CMD ${VALGRIND_PROGRAM} ${VALGRIND_CONFIG} --xml=yes --xml-file=${PROJECT_SOURCE_DIR}/output/${LIBPARSER_NAME}-ut-leakcheck.xml  ${PLUGIN_OUT_FOLDER}/${test_BIN})
ELSE (LEAKCHECK)
    SET (TEST_EXEC_CMD ${ENV} ${PLUGIN_OUT_FOLDER}/${test_BIN})
ENDIF (LEAKCHECK)

ADD_TEST(${LIBPARSER_NAME}-ut-test ${PLUGIN_OUT_FOLDER}/${test_BIN})
#ADD_CUSTOM_TARGET(${LIBPARSER_NAME}-ut-test
#                    ${AUTORUN_TESTS}
#                    ${TEST_EXEC_CMD}
#                    DEPENDS ${test_BIN}
#                    WORKING_DIRECTORY "${MAINFOLDER}"
#                    COMMENT "Executing ${LIBPARSER_NAME}-ut-test unit tests..."
#                    VERBATIM
#                    SOURCES ${test_SRCS})

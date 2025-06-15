FIND_PACKAGE(RapidJSON REQUIRED)
FIND_PACKAGE(Boost 1.56 COMPONENTS program_options REQUIRED)

# Collect sources
FILE(GLOB_RECURSE nclangsrv_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
LIST(REMOVE_ITEM nclangsrv_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/targetver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

# Define executable and its libraries
SET(nclangsrv_BIN ${NCLANGSRV_NAME})
SET(nclangsrv_LIBS ${LIBPARSER_NAME} ${Boost_LIBRARIES})

ADD_EXECUTABLE(${nclangsrv_BIN} ${nclangsrv_SRCS})

# Link against the correct parser library
IF (project_build_shared)
  ADD_DEPENDENCIES(${nclangsrv_BIN} ${LIBPARSER_NAME_SHARED})
ELSE()
  ADD_DEPENDENCIES(${nclangsrv_BIN} ${LIBPARSER_NAME_STATIC})
ENDIF()

TARGET_LINK_LIBRARIES(${nclangsrv_BIN} PRIVATE ${nclangsrv_LIBS})

# Copy config directory to output path before build
ADD_CUSTOM_COMMAND(TARGET ${nclangsrv_BIN} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/conf ${PLUGIN_OUT_FOLDER}/conf/)

# Leak check command setup (used for dev or test convenience)
IF (LEAKCHECK)
  IF (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
  ENDIF()
  SET(TEST_EXEC_CMD ${VALGRIND_PROGRAM} ${VALGRIND_CONFIG}
    --xml=yes
    --xml-file=${PROJECT_SOURCE_DIR}/output/${LIBPARSER_NAME}-leakcheck.xml
    ${MAINFOLDER}/bin/${nclangsrv_BIN})
ELSE()
  SET(TEST_EXEC_CMD ${ENV} ${MAINFOLDER}/bin/${nclangsrv_BIN})
ENDIF()

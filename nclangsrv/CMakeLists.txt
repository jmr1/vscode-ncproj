FIND_PACKAGE (RapidJSON REQUIRED)

FILE (GLOB_RECURSE nclangsrv_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
LIST(REMOVE_ITEM nclangsrv_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp ${CMAKE_CURRENT_SOURCE_DIR}/targetver.h ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)
SET (nclangsrv_LIBS ${LIBPARSER_NAME} ${Boost_LIBRARIES})
SET (nclangsrv_BIN ${NCLANGSRV_NAME})

ADD_EXECUTABLE(${nclangsrv_BIN} ${nclangsrv_SRCS})
IF (project_build_shared)
  ADD_DEPENDENCIES(${nclangsrv_BIN} ${LIBPARSER_NAME_SHARED})
ELSE (project_build_shared)
  ADD_DEPENDENCIES(${nclangsrv_BIN} ${LIBPARSER_NAME_STATIC})
ENDIF (project_build_shared)
TARGET_LINK_LIBRARIES(${nclangsrv_BIN} ${nclangsrv_LIBS})

add_custom_command(TARGET nclangsrv PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/conf ${PLUGIN_OUT_FOLDER}/conf/)

IF (LEAKCHECK)
  IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
    FILE (MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
  ENDIF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/output)
  SET (TEST_EXEC_CMD ${VALGRIND_PROGRAM} ${VALGRIND_CONFIG} --xml=yes --xml-file=${PROJECT_SOURCE_DIR}/output/${LIBPARSER_NAME}-leakcheck.xml  ${MAINFOLDER}/bin/${nclangsrv_BIN})
ELSE (LEAKCHECK)
  SET (TEST_EXEC_CMD ${ENV} ${MAINFOLDER}/bin/${nclangsrv_BIN})
ENDIF (LEAKCHECK)


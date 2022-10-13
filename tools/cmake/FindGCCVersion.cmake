if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${GCCVersion_FIND_VERSION})
    message(FATAL_ERROR "Insufficient gcc version, required ${GCCVersion_FIND_VERSION}")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13)
    message(FATAL_ERROR "Insufficient msvc version")
  endif()
else()
  message(WARNING "You are using an unsupported compiler!")
endif()
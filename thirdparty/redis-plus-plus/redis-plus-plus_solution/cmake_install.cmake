# Install script for directory: C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/redis++")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Debug/redis++_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Release/redis++_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/MinSizeRel/redis++_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/RelWithDebInfo/redis++_static.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Debug/redis++.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Release/redis++.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/MinSizeRel/redis++.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/RelWithDebInfo/redis++.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Debug/redis++.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/Release/redis++.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/MinSizeRel/redis++.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/RelWithDebInfo/redis++.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake"
         "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sw/redis++" TYPE FILE FILES
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/cmd_formatter.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/command.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/command_args.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/command_options.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/connection.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/connection_pool.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/cxx17/sw/redis++/cxx_utils.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/cxx_utils.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/errors.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/hiredis_features.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/no_tls/sw/redis++/tls.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/pipeline.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/queued_redis.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/queued_redis.hpp"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis++.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis.hpp"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis_cluster.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis_cluster.hpp"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/redis_uri.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/reply.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/sentinel.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/shards.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/shards_pool.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/subscriber.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/tls.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/transaction.h"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/utils.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/redis++/include/sw/redis++/patterns/redlock.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files/redis++/include/sw/redis++/patterns" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus/src/sw/redis++/patterns/redlock.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/cmake/redis++-config.cmake"
    "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/cmake/redis++-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/cmake/redis++.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/SSC/Desktop/Study/IOCP_Study/thirdparty/redis-plus-plus/redis-plus-plus_solution/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

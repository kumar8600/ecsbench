cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ecsbench)

# default to a release build if no CMAKE_BUILD_TYPE is defined
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/lib)

aux_source_directory(src SRC_LIST)
add_executable(ecsbench ${SRC_LIST})

include(ExternalProject)

ExternalProject_Add(project_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v0.1.0
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/tmp/benchmark
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  )

ExternalProject_Add(project_entityx
  GIT_REPOSITORY https://github.com/alecthomas/entityx.git
  GIT_TAG 1.1.2
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/tmp/entityx
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DENTITYX_MAX_COMPONENTS=256
  )

ExternalProject_Add(project_anax
  GIT_REPOSITORY https://github.com/miguelmartin75/anax.git
  GIT_TAG v1.1
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/tmp/anax
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  )

find_package(Git REQUIRED)

# coment is not a cmake project && header only
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/external/include/coment
  COMMAND ${GIT_EXECUTABLE} clone --depth 1 -b v2.3.1 https://github.com/Catchouli/coment.git
  COMMAND ${CMAKE_COMMAND} -E copy_directory coment/coment/include/coment ../include/coment
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external/tmp
  )
set_property(SOURCE src/coment_bench.cpp APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/external/include/coment)

add_dependencies(ecsbench
  project_benchmark
  project_entityx
  project_anax
  )

target_include_directories(ecsbench
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/external/include
  )

find_package(Threads REQUIRED)

target_link_libraries(ecsbench
  LINK_PRIVATE Threads::Threads
  LINK_PRIVATE benchmark
  LINK_PRIVATE entityx
  LINK_PRIVATE debug anax_d
  LINK_PRIVATE optimized anax
  )

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

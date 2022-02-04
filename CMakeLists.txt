cmake_minimum_required(VERSION 3.14)
project(cpcos
    VERSION 0.0.1
    LANGUAGES C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 14)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
  message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

enable_testing()

file(GLOB_RECURSE ALL_HDRS "cpcos/*.h" "cpcos/*.hpp" "cpcos/*.inc" "cpcos/*.ipp")
file(GLOB_RECURSE ALL_SRCS "cpcos/*.cc" "cpcos/*.cpp")
file(GLOB_RECURSE ALL_TEST_SRCS "cpcos/*_test.cc" "cpcos/*_test.cpp" tests/*.cc tests/*.cpp)
file(GLOB_RECURSE ALL_EXECUTABLE_SRCS "cpcos/*_main.cc" "cpcos/*_main.cpp")

# remove hidden files and folders that could potentially pollute the build.
file(GLOB_RECURSE ALL_DOTFILES ".*/*")
if (ALL_DOTFILES)
  list(REMOVE_ITEM ALL_HDRS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_SRCS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_TEST_SRCS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_EXECUTABLE_SRCS ${ALL_DOTFILES})
endif()

list(REMOVE_ITEM ALL_SRCS ${ALL_EXECUTABLE_SRCS})
list(REMOVE_ITEM ALL_SRCS ${ALL_TEST_SRCS})

# flatbuffer schema
file(GLOB_RECURSE ALL_FBS "*.fbs")
message("ALL_FBS: ${ALL_FBS}")
# TODO: find flatc programatically
set(FLATBUFFERS_FLATC_EXECUTABLE ${CONAN_BIN_DIRS_FLATBUFFERS}/flatc)
message("FLATBUFFERS_FLATC_EXECUTABLE: ${FLATBUFFERS_FLATC_EXECUTABLE}")
set(ALL_FBS_SRCS)
set(ALL_FBS_HDRS)
foreach(ABS_FIL ${ALL_FBS})
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  list(APPEND ALL_FBS_HDRS "${PROJECT_BINARY_DIR}/${DIR}/${FIL_WE}_generated.h")

  message("ABS_FIL: ${ABS_FIL}")
  
  build_flatbuffers(
    ${ABS_FIL}
    ""
    ${FIL_WE}_generated.h
    ""
    ${PROJECT_BINARY_DIR}/${DIR}
    ""
    ""
  )
endforeach()

set_source_files_properties(${ALL_FBS_SRCS} ${ALL_PROTO_HDRS} PROPERTIES GENERATED TRUE)
list(APPEND ALL_HDRS ${ALL_FBS_HDRS})

if(NOT ${ALL_SRCS} STREQUAL "")
  add_library(${PROJECT_NAME} STATIC ${ALL_HDRS} ${ALL_SRCS})
  target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
else()
  message("no source file found, make it a header-only library")
  add_library(${PROJECT_NAME} INTERFACE ${ALL_HDRS})
  target_link_libraries(${PROJECT_NAME} INTERFACE ${CONAN_LIBS})
endif()

# build tests, but exclude some of them
set(IGNORE_TEST_SRCS)
list(APPEND IGNORE_TEST_SRCS "${PROJECT_SOURCE_DIR}/tests/boost_log_test.cpp") # Boost.Log is not header-only, not supported yet
list(REMOVE_ITEM ALL_TEST_SRCS ${IGNORE_TEST_SRCS})

message("ALL_TEST_SRCS: ${ALL_TEST_SRCS}")

foreach(ABS_FIL ${ALL_TEST_SRCS})
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  # replace slashes as required for CMP0037.
  string(REPLACE "/" "." TEST_TARGET_NAME "${DIR}/${FIL_WE}")

  add_executable(${TEST_TARGET_NAME} ${ABS_FIL})
  target_link_libraries("${TEST_TARGET_NAME}" ${CONAN_LIBS_GTEST})
  add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
endforeach()


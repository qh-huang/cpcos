cmake_minimum_required(VERSION 3.14)
project(cpcos
    VERSION 0.0.1
    LANGUAGES C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 20)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#   message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

enable_testing()

file(GLOB_RECURSE ALL_HDRS "cpcos/*.h" "cpcos/*.hpp" "cpcos/*.inc" "cpcos/*.ipp")
file(GLOB_RECURSE ALL_SRCS "cpcos/*.cc" "cpcos/*.cpp")
file(GLOB_RECURSE ALL_TEST_SRCS "cpcos/*_test.cc" "cpcos/*_test.cpp" "tests/*_test.cc" "tests/*_test.cpp")
file(GLOB_RECURSE ALL_GTEST_SRCS "cpcos/*_gtest.cc" "cpcos/*_gtest.cpp" "tests/*_gtest.cc" "tests/*_gtest.cpp")
file(GLOB_RECURSE ALL_EXECUTABLE_SRCS "cpcos/*_main.cc" "cpcos/*_main.cpp")

# debug only
# message(STATUS "ALL_SRCS=${ALL_SRCS}")

# remove hidden files and folders that could potentially pollute the build.
file(GLOB_RECURSE ALL_DOTFILES ".*/*")
if (ALL_DOTFILES)
  list(REMOVE_ITEM ALL_HDRS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_SRCS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_TEST_SRCS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_GTEST_SRCS ${ALL_DOTFILES})
  list(REMOVE_ITEM ALL_EXECUTABLE_SRCS ${ALL_DOTFILES})
endif()

if (NOT "${ALL_EXECUTABLE_SRCS} " STREQUAL " ")
  list(REMOVE_ITEM ALL_SRCS ${ALL_EXECUTABLE_SRCS})
endif ()

if (NOT "${ALL_TEST_SRCS} " STREQUAL " ")
  list(REMOVE_ITEM ALL_SRCS ${ALL_TEST_SRCS})
endif ()

if (NOT "${ALL_GTEST_SRCS} " STREQUAL " ")
  list(REMOVE_ITEM ALL_SRCS ${ALL_GTEST_SRCS})
endif ()

# flatbuffers
include("${CONAN_LIB_DIRS_FLATBUFFERS}/cmake/BuildFlatBuffers.cmake")

# flatbuffer schema
file(GLOB_RECURSE ALL_FBS "*.fbs")
# message("ALL_FBS: ${ALL_FBS}")

# TODO: find flatc programatically
set(FLATBUFFERS_FLATC_EXECUTABLE ${CONAN_BIN_DIRS_FLATBUFFERS}/flatc)
# message("FLATBUFFERS_FLATC_EXECUTABLE: ${FLATBUFFERS_FLATC_EXECUTABLE}")

set(ALL_FBS_HDRS)
foreach(ABS_FIL ${ALL_FBS})
  # message("ABS_FIL: ${ABS_FIL}")
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  list(APPEND ALL_FBS_HDRS "${PROJECT_BINARY_DIR}/${DIR}/${FIL_WE}_generated.h")

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

find_package(Protobuf REQUIRED)
# message("Protobuf_PROTOC_EXECUTABLE: ${Protobuf_PROTOC_EXECUTABLE}")
file(GLOB_RECURSE ALL_PROTOS "*.proto")
# message("ALL_PROTOS: ${ALL_PROTOS}")
foreach(ABS_FIL ${ALL_PROTOS})
  # message("ABS_FIL: ${ABS_FIL}")
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ABS_FIL})
  # message("PROTO_SRCS: ${PROTO_SRCS}")
  # message("PROTO_HDRS: ${PROTO_HDRS}")
  list(APPEND ALL_PROTO_HDRS "${PROTO_HDRS}")
  list(APPEND ALL_PROTO_SRCS "${PROTO_SRCS}")
endforeach()

set_source_files_properties(${ALL_FBS_SRCS} ${ALL_PROTO_SRCS} PROPERTIES GENERATED TRUE)
list(APPEND ALL_HDRS ${ALL_FBS_HDRS} ${ALL_PROTO_HDRS})
list(APPEND ALL_SRCS ${ALL_PROTO_SRCS})
# message("ALL_HDRS: ${ALL_HDRS}")

# include PROJECT_BINARY_DIR since the .pb.h files are generated in the top level of build directory
include_directories(cpcos ${PROJECT_BINARY_DIR})
# message("CONAN_LIBS: ${CONAN_LIBS}")
if(NOT "${ALL_SRCS} " STREQUAL " ")
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
list(REMOVE_ITEM ALL_GTEST_SRCS ${IGNORE_TEST_SRCS})

# google tests
include(GoogleTest)
enable_testing()
# message("ALL_GTEST_SRCS: ${ALL_TGEST_SRCS}")
foreach(ABS_FIL ${ALL_GTEST_SRCS})
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  # replace slashes as required for CMP0037.
  string(REPLACE "/" "." TEST_TARGET_NAME "${DIR}/${FIL_WE}")

  add_executable(${TEST_TARGET_NAME} ${ABS_FIL})
  target_link_libraries("${TEST_TARGET_NAME}" ${CONAN_LIBS_GTEST} ${PROJECT_NAME})
  gtest_discover_tests(${TEST_TARGET_NAME})
endforeach()

#normal tests
foreach(ABS_FIL ${ALL_TEST_SRCS})
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${ABS_FIL})
  get_filename_component(DIR ${REL_FIL} DIRECTORY)
  get_filename_component(FIL_WE ${REL_FIL} NAME_WE)

  # replace slashes as required for CMP0037.
  string(REPLACE "/" "." TEST_TARGET_NAME "${DIR}/${FIL_WE}")

  add_executable(${TEST_TARGET_NAME} ${ABS_FIL})
  target_link_libraries("${TEST_TARGET_NAME}" ${PROJECT_NAME})
endforeach()

# docs
add_subdirectory(docs)

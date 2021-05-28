
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Project
project(cluon-nmea0183 LANGUAGES CXX)

# Fetch CPM
set(CPM_DOWNLOAD_VERSION 0.32.0) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake v${CPM_DOWNLOAD_VERSION}")
  file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})

# Add dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

CPMAddPackage("gh:CLIUtils/CLI11@1.9.1")
CPMAddPackage("gh:gabime/spdlog@1.8.2")

CPMAddPackage(
  NAME cluon
  GITHUB_REPOSITORY chrberger/libcluon
  VERSION 0.0.140
  SOURCE_SUBDIR libcluon
  EXCLUDE_FROM_ALL YES
  PATCH_COMMAND git apply ${CMAKE_SOURCE_DIR}/external/libcluon_CMakeLists_disable_tests.patch
)

# Create executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} 
  PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/src 
  ${cluon_SOURCE_DIR}/libcluon/include
  ${CMAKE_BINARY_DIR}/include # Compiled message specifications!
)
target_link_libraries(${PROJECT_NAME} Threads::Threads cluon-static CLI11 spdlog::spdlog_header_only)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# Tests
CPMAddPackage("gh:onqtam/doctest#2.4.0")

enable_testing()
add_executable(${PROJECT_NAME}-tester ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_nmea_0183_assembler.cpp)
target_include_directories(${PROJECT_NAME}-tester PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}-tester doctest)
add_test(NAME ${PROJECT_NAME}-tester COMMAND ${PROJECT_NAME}-tester)

# Install executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT ${PROJECT_NAME})

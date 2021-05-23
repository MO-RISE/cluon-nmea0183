
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

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.4.0
)

CPMAddPackage(
  NAME CLI11
  GITHUB_REPOSITORY CLIUtils/CLI11
  VERSION 1.9.1  
)

CPMAddPackage("gh:gabime/spdlog@1.8.2")

# Extract cluon-msc from cluon-complete.hpp.
set(CLUON_COMPLETE cluon-complete-v0.0.140.hpp)
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/cluon-msc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/external/${CLUON_COMPLETE} ${CMAKE_BINARY_DIR}/cluon-complete.hpp
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/cluon-complete.hpp ${CMAKE_BINARY_DIR}/cluon-complete.cpp
    COMMAND ${CMAKE_CXX_COMPILER} -o ${CMAKE_BINARY_DIR}/cluon-msc ${CMAKE_BINARY_DIR}/cluon-complete.cpp -std=c++14 -pthread -D HAVE_CLUON_MSC
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/external/${CLUON_COMPLETE})

# Add custom traget to make sure we can specify dependency
add_custom_target(generate_cluon_complete_hpp DEPENDS ${CMAKE_BINARY_DIR}/cluon-msc)

# Create executable
add_executable(${PROJECT_NAME} src/main.cpp)
add_dependencies(${PROJECT_NAME} generate_cluon_complete_hpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src SYSTEM ${CMAKE_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} Threads::Threads CLI11 spdlog::spdlog_header_only)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# Enable unit testing.
enable_testing()
add_executable(${PROJECT_NAME}-tester ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_nmea_0183_assembler.cpp)
target_include_directories(${PROJECT_NAME}-tester PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}-tester doctest)
add_test(NAME ${PROJECT_NAME}-tester COMMAND ${PROJECT_NAME}-tester)

# Install executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT ${PROJECT_NAME})

cmake_minimum_required(VERSION 3.10)
project(MyShell CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Good practice

# Add source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Find source files
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(myshell ${SOURCES})

# Add include directories if we create an include folder later
# target_include_directories(myshell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# For Windows, we might need to link specific libraries later, e.g., for networking or advanced console features
if(WIN32)
  # message(STATUS "Configuring for Windows")
  # Example: target_link_libraries(myshell PRIVATE some_windows_lib)
endif()

# Enable warnings (good practice)
if(MSVC)
  # /WX treats warnings as errors. Remove if too strict during early development.
  add_compile_options(/W4 /WX) 
else()
  # -Werror treats warnings as errors. Remove if too strict during early development.
  add_compile_options(-Wall -Wextra -Wpedantic -Werror) 
endif() 
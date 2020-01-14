cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME Pollster)
set(VERSION_NUM 1.0)
project(${PROJECT_NAME} VERSION ${VERSION_NUM})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#Set output Directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include(CheckFunctionExists)

set(CMAKE_REQUIRED_INCLUDES sys/event.h)
check_function_exists(kqueue HAVE_KQUEUE)
set(CMAKE_REQUIRED_INCLUDES )
if(HAVE_KQUEUE)
	add_library(pollster STATIC ${CMAKE_CURRENT_SOURCE_DIR}/KQueue/Pollster.cpp)
else()
	set(CMAKE_REQUIRED_INCLUDES sys/epoll.h)
	check_function_exists(epoll_create HAVE_EPOLL)
	set(CMAKE_REQUIRED_INCLUDES )
	if(HAVE_EPOLL)
		add_library(pollster STATIC ${CMAKE_CURRENT_SOURCE_DIR}/EPoll/Pollster.cpp)
	else()
		message(FATAL_ERROR "System must support either KQueue or EPoll")
	endif()
endif()

install(TARGETS pollster DESTINATION lib)
install(FILES Pollster.hpp DESTINATION include)


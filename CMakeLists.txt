cmake_minimum_required(VERSION 3.16)

project (client-core
    VERSION 0.0.1 
    DESCRIPTION "client core lib"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
#project settings
include(ProjectSettings)
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
#project warnings
add_library(project_warnings INTERFACE)
include(CompilerWarnings)
set_project_warnings(project_warnings)
#include libs
if(EXISTS ${CMAKE_MODULE_PATH}/LibsPath.cmake)
    include(LibsPath)
endif()
#boost settings
include(BoostSettings)

set(SOURCE_FILES 
    src/client.cpp 
    src/connection.cpp
    src/worker.cpp
)
set(PRIVATE_HEADER_FILES 
    src/connection.h
    src/worker.h
)
set(PUBLIC_HEADER_FILES 
    include/clientcore/client.h
)
add_library(clientcore_static ${SOURCE_FILES} ${PRIVATE_HEADER_FILES} ${PUBLIC_HEADER_FILES})
add_library(clientcore::clientcore ALIAS clientcore_static)
set_target_properties(clientcore_static PROPERTIES OUTPUT_NAME clientcore_static)
target_include_directories(clientcore_static 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#precompiled headers
target_precompile_headers(clientcore_static PRIVATE src/pch.h)

# set MT / MTd static linking flags
set_property(TARGET clientcore_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time)

target_link_libraries(clientcore_static 
    PRIVATE
        project_options
        project_warnings
        fmt::fmt 
        spdlog::spdlog 
        Boost::boost
        Boost::date_time
)
cmake_minimum_required(VERSION 3.10)
project(CS621Project2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

build_lib(
    LIBNAME CS621Project2
    SOURCE_FILES
        model/diffserv.cc
        model/traffic-class.cc
        model/filter.cc
        model/filter-element.cc
        model/spq.cc
        model/drr.cc
    HEADER_FILES
        model/diffserv.h
        model/traffic-class.h
        model/filter.h
        model/filter-element.h
        model/spq.h
        model/drr.h
    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libflow-monitor}
)
target_include_directories(CS621Project2 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/model>
)

build_exec(
    EXECNAME spq-simulation
    SOURCE_FILES model/spq-simulation.cc
    LIBRARIES_TO_LINK
        CS621Project2
        ${libcore}
        ${libnetwork}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libflow-monitor}
    EXECUTABLE_DIRECTORY_PATH
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/CS621Project2/
)

build_exec(
    EXECNAME drr-simulation
    SOURCE_FILES model/drr-simulation.cc
    LIBRARIES_TO_LINK
        CS621Project2
        ${libcore}
        ${libnetwork}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libflow-monitor}
    EXECUTABLE_DIRECTORY_PATH
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/CS621Project2/
)
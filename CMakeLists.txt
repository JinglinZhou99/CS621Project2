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
        model/schedulers/spq.cc
        model/schedulers/drr.cc
    HEADER_FILES
        model/diffserv.h
        model/traffic-class.h
        model/filter.h
        model/filter-element.h
        model/schedulers/spq.h
        model/schedulers/drr.h
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/model/schedulers>
)

build_exec(
    EXECNAME spq-simulation
    SOURCE_FILES model/simulation/spq-simulation.cc
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
    SOURCE_FILES model/simulation/drr-simulation.cc
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

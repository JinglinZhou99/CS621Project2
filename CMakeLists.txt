# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.22)

# Define the project name
project(CS621Project2)

# Define the library for CS621Project2
add_library(CS621Project2
    model/diffserv.cc
    model/traffic-class.cc
    model/filter.cc
    model/filter-element.cc
    model/spq.cc
    model/drr.cc
    model/diffserv.h
    model/traffic-class.h
    model/filter.h
    model/filter-element.h
    model/spq.h
    model/drr.h
)

# Include ns-3 directories and the model directory for CS621Project2
target_include_directories(CS621Project2 PRIVATE ${ns3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/model)

# Link the necessary ns-3 libraries to the CS621Project2 library
target_link_libraries(CS621Project2
    ns3::core
    ns3::network
    ns3::internet
    ns3::point-to-point
    ns3::applications
    ns3::flow-monitor
)

# Add simulation executables in model directory
add_executable(spq-simulation
    model/spq-simulation.cc
)

add_executable(drr-simulation
    model/drr-simulation.cc
)

# Include the model directory for spq-simulation and drr-simulation
target_include_directories(spq-simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/model ${ns3_INCLUDE_DIRS})
target_include_directories(drr-simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/model ${ns3_INCLUDE_DIRS})

# Link the simulation executables to the CS621Project2 library and ns-3 libraries
target_link_libraries(spq-simulation
    CS621Project2
    ns3::core
    ns3::network
    ns3::internet
    ns3::point-to-point
    ns3::applications
    ns3::flow-monitor
)

target_link_libraries(drr-simulation
    CS621Project2
    ns3::core
    ns3::network
    ns3::internet
    ns3::point-to-point
    ns3::applications
    ns3::flow-monitor
)

# Optional: Uncomment if you have examples
# if(NS3_EXAMPLES)
#   add_subdirectory(examples)
# endif()

# Optional: Uncomment if you have tests
# if(NS3_TESTS)
#   add_subdirectory(test)
# endif()
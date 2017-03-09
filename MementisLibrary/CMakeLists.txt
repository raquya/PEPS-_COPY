cmake_minimum_required(VERSION 2.8)
project(mc-pricer C CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

if (CXX)
    set(CMAKE_CXX_COMPILER ${CXX})
endif (CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()

# Detect MPI
set(MPI_LIBS)
find_package(MPI REQUIRED)
if (MPI_C_FOUND)
    include_directories(${MPI_C_INCLUDE_PATH})
    set (MPI_LIBS ${MPI_LIBS} ${MPI_C_LIBRARIES})
    add_definitions(-DOMPI_SKIP_MPICXX)
else (MPI_C_FOUND)
    message(STATUS "MPI not found.")
endif (MPI_C_FOUND)

# Detect Boost
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
message(STATUS "Boost include directory ${Boost_INCLUDE_DIR}")

# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF=1)
endif()

# Add math lib
set(LIBS ${LIBS} m)

# Pricing
add_executable(pricer src/BlackScholesModel.cpp src/MonteCarlo.cpp  src/parser.cpp  src/pricer.cpp src/BSParameters.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp src/BasketOption.cpp src/AsianOption.cpp src/Utils.cpp)
target_link_libraries(pricer ${LIBS})


#TestHedging
add_executable(HedgingCallTest src/BlackScholesModel.cpp src/MonteCarlo.cpp  test/HedgingCallTest.cpp src/BSParameters.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp src/BasketOption.cpp )
target_link_libraries(HedgingCallTest ${LIBS})

#TestDelta
add_executable(DeltaCallTest src/BlackScholesModel.cpp src/MonteCarlo.cpp test/DeltaCallTest.cpp src/BSParameters.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp src/BasketOption.cpp src/AsianOption.cpp)
target_link_libraries(DeltaCallTest ${LIBS})

#TestPayOff valeur minimum
add_executable(TestPayOffMin test/TestPayOffMin.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp)
target_link_libraries(TestPayOffMin ${LIBS})

#Test convergence du PnL
add_executable(TestConvergencePnL test/PnlConvergenceTest.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp src/FlowCalculator.cpp src/BSParameters.cpp src/BlackScholesModel.cpp src/MonteCarlo.cpp src/Utils.cpp)
target_link_libraries(TestConvergencePnL ${LIBS})


# Library
#add_executable(MementisLibrary src/BlackScholesModel.cpp src/MonteCarlo.cpp src/MementisLibrary.cpp src/BSParameters.cpp src/FlowCalculator.cpp src/ProduitMementis.cpp src/ConstantVolatilityModel.cpp src/ConstantRateModel.cpp src/Utils.cpp)
#target_link_libraries(MementisLibrary ${LIBS})


# Print compiler flags
get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_LINKER_EXE_FLAGS}" )
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_LINKER_EXE_FLAGS}" )
endif()

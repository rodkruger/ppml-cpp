cmake_minimum_required(VERSION 3.5.1)

### To use gcc/g++ on a Macintosh, you must set the Compilers
### here, not inside the project
##if(APPLE)
##       set(CMAKE_C_COMPILER "/usr/local/bin/gcc-7")
##       set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-7")
##endif()
### TODO: for now, we use CLang for Mac
###
### In order to create OpenFHE's static libraries you should enable
### the BUILD_STATIC option. For that, you run "cmake .. -DBUILD_STATIC=ON".
### After having your link completed you will find static libs
### with the suffix "_static" in ./build/libs/.
### Examples: OPENFHEpke_static.a, OPENFHEcore_static.a, etc.
### After you run "make install" in your build directory, you can build your custom application.
### If you need your application to be linked statically, then run "cmake .. -DBUILD_STATIC=ON"

project(hermesml CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_STATIC "Set to ON to include static versions of the library" OFF)

find_package(OpenFHE CONFIG REQUIRED)
if (OpenFHE_FOUND)
    message(STATUS "FOUND PACKAGE OpenFHE")
    message(STATUS "OpenFHE Version: ${BASE_OPENFHE_VERSION}")
    message(STATUS "OpenFHE installed as shared libraries: ${OpenFHE_SHARED}")
    message(STATUS "OpenFHE include files location: ${OpenFHE_INCLUDE}")
    message(STATUS "OpenFHE lib files location: ${OpenFHE_LIBDIR}")
    message(STATUS "OpenFHE Native Backend size: ${OpenFHE_NATIVE_SIZE}")
else ()
    message(FATAL_ERROR "PACKAGE OpenFHE NOT FOUND")
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -flto=auto -ffunction-sections -fdata-sections -fopenmp -funroll-loops -falign-functions=32 -fomit-frame-pointer -fprefetch-loop-arrays -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

include_directories(${OPENMP_INCLUDES})
include_directories(${OpenFHE_INCLUDE})
include_directories(${OpenFHE_INCLUDE}/third-party/include)
include_directories(${OpenFHE_INCLUDE}/core)
include_directories(${OpenFHE_INCLUDE}/pke)
include_directories(${OpenFHE_INCLUDE}/binfhe)
### add directories for other OpenFHE modules as needed for your project
include_directories(includes)

link_directories(${OpenFHE_LIBDIR})
link_directories(${OPENMP_LIBRARIES})
if (BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS} -static")
    link_libraries(${OpenFHE_STATIC_LIBRARIES})
else ()
    set(CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS})
    link_libraries(${OpenFHE_SHARED_LIBRARIES})
endif ()

# Fetch and link spdlog (with fmt bundled)
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.0
)
FetchContent_MakeAvailable(spdlog)

### ADD YOUR EXECUTABLE(s) HERE
### add_executable( EXECUTABLE-NAME SOURCES )
###

add_executable(CkksLogisticRegression
        src/examples/CkksLogisticRegression.cpp
        includes/client.h
        includes/context.h
        includes/core.h
        includes/datasets.h
        includes/experiments.h
        includes/hemath.h
        includes/model.h
        includes/validation.h
        src/client/Client.cpp
        src/context/HEContextFactory.cpp
        src/context/HEContext.cpp
        src/core/BootstrapableCiphertext.cpp
        src/core/Experiment.cpp
        src/core/EncryptedObject.cpp
        src/core/MinMaxScaler.cpp
        src/core/Quantizer.cpp
        src/experiments/CkksLogisticRegressionExperiment.cpp
        src/datasets/BreastCancerDataset.cpp
        src/datasets/Datasets.cpp
        src/datasets/DiabetesDataset.cpp
        src/hemath/Calculus.cpp
        src/hemath/Constants.cpp
        src/model/CkksLogisticRegression.cpp
        src/validation/Holdout.cpp
        src/datasets/GliomaGradingDataset.cpp
        src/datasets/DifferentiatedThyroidDataset.cpp
        src/datasets/CirrhosisPatientDataset.cpp
        src/datasets/LoanPredictionDataset.cpp
        src/datasets/CreditCardFraudDataset.cpp
        src/model/MlModel.cpp)
target_link_libraries(CkksLogisticRegression PRIVATE spdlog::spdlog)

add_executable(CkksNeuralNetwork
        src/examples/CkksNeuralNetwork.cpp
        includes/client.h
        includes/context.h
        includes/core.h
        includes/datasets.h
        includes/experiments.h
        includes/hemath.h
        includes/model.h
        includes/validation.h
        src/client/Client.cpp
        src/context/HEContextFactory.cpp
        src/context/HEContext.cpp
        src/core/BootstrapableCiphertext.cpp
        src/core/Experiment.cpp
        src/core/EncryptedObject.cpp
        src/core/MinMaxScaler.cpp
        src/core/Quantizer.cpp
        src/experiments/CkksNeuralNetworkExperiment.cpp
        src/datasets/BreastCancerDataset.cpp
        src/datasets/Datasets.cpp
        src/datasets/DiabetesDataset.cpp
        src/hemath/Calculus.cpp
        src/hemath/Constants.cpp
        src/model/CkksNeuralNetwork.cpp
        src/validation/Holdout.cpp
        src/datasets/GliomaGradingDataset.cpp
        src/datasets/DifferentiatedThyroidDataset.cpp
        src/datasets/CirrhosisPatientDataset.cpp
        src/datasets/LoanPredictionDataset.cpp
        src/datasets/CreditCardFraudDataset.cpp
        src/model/MlModel.cpp)
target_link_libraries(CkksNeuralNetwork PRIVATE spdlog::spdlog)

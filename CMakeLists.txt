#cmake_minimum_required(VERSION 3.17)
#project(pathtracer-sycl CXX)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
#
##set(CMAKE_CXX_COMPILER /usr/bin/syclcc)
#
#
##list(APPEND COMPUTECPP_USER_FLAGS -fPIC)
##list(APPEND COMPUTECPP_DEVICE_COMPILER_FLAGS -fPIC)
##
##option(COMPUTECPP_SDK_USE_OPENMP "Enable OpenMP support" ON)
##option(COMPUTECPP_BITCODE ptx64)
##set(COMPUTECPP_BITCODE ptx64)
##set(ComputeCpp_DIR /opt/ComputeCpp-CE)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
#list(APPEND CMAKE_PREFIX_PATH /opt/hipSYCL/CUDA/lib/cmake/hipSYCL)
##find_package(ComputeCpp REQUIRED)
#
#
##include(OpenCL)
#
#set(DPCPP_ROOT /opt/dpc++)
#option(DPCPP_ROOT /opt/dpc++)
#set(SYCLROOT /opt/dpc++)
#
##include(SYCL)
#
##find_package(SYCL REQUIRED)
#
#
#
#set(QT_VERSION 5)
#set(REQUIRED_LIBS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)
#set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)
#
#add_executable(${PROJECT_NAME} main.cpp Ray.cpp Ray.h Sphere.cpp Sphere.h Camera.cpp Camera.h Material.cpp Material.h Random.h Diffuse.cpp Diffuse.h Emissive.cpp Emissive.h Trace.h Random.cpp Triangle.cpp Triangle.h)
#target_compile_options(${PROJECT_NAME} PRIVATE -O3)
##add_sycl_to_target(TARGET ${PROJECT_NAME} SOURCES main.cpp Ray.cpp Ray.h Sphere.cpp Sphere.h Camera.cpp Camera.h)
#
#MESSAGE(${REQUIRED_LIBS_QUALIFIED})
#
#if (NOT CMAKE_PREFIX_PATH)
#    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
#            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
#endif ()
#
#set(HIPSYCL_TARGETS cuda:sm_60)
#find_package(hipSYCL REQUIRED)
#add_sycl_to_target(TARGET ${PROJECT_NAME} SOURCES main.cpp)
#
#find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})
#
#target_compile_options(${PROJECT_NAME} PRIVATE -O3)
#
##target_compile_options(${PROJECT_NAME} PRIVATE -fsycl-targets=nvptx64-nvidia-cuda-sycldevice -Xcuda-ptxas -maxrregcount=96 --cuda-gpu-arch=sm52)
##target_link_options(${PROJECT_NAME} PRIVATE -fsycl-targets=nvptx64-nvidia-cuda-sycldevice -Xcuda-ptxas -maxrregcount=96 --cuda-gpu-arch=sm52)
#
##target_compile_options(${PROJECT_NAME} PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda-sycldevice,spir64-unknown-unknown-sycldevice)
##target_link_options(${PROJECT_NAME} PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda-sycldevice,spir64-unknown-unknown-sycldevice)

cmake_minimum_required(VERSION 3.17)
project(pathtracer-sycl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH /opt/hipSYCL/CUDA/lib/cmake/hipSYCL)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)

add_executable(${PROJECT_NAME} main.cpp Ray.cpp Ray.h Sphere.cpp Sphere.h Camera.cpp Camera.h Material.cpp Material.h Random.h Diffuse.cpp Diffuse.h Emissive.cpp Emissive.h Trace.h Random.cpp Triangle.cpp Triangle.h Mesh.cpp Mesh.h AABB.cpp AABB.h kdTreeMesh.h)

set(HIPSYCL_TARGETS cuda:sm_60)
#set(HIPSYCL_TARGETS omp)
find_package(hipSYCL REQUIRED)
add_sycl_to_target(TARGET ${PROJECT_NAME} SOURCES main.cpp Ray.cpp Ray.h Sphere.cpp Sphere.h Camera.cpp Camera.h Material.cpp Material.h Random.h Diffuse.cpp Diffuse.h Emissive.cpp Emissive.h Trace.h Random.cpp Triangle.cpp Triangle.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})

target_compile_options(${PROJECT_NAME} PRIVATE -Ofast -fsingle-precision-constant -fgcse -fcuda-approx-transcendentals -fcuda-flush-denormals-to-zero -Xcuda-ptxas -maxrregcount=64)
#target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=leak -fsanitize=undefined -fsanitize=address)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=leak -fsanitize=undefined -fsanitize=address)
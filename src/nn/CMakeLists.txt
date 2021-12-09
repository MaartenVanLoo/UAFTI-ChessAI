cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(example-app)
#message(${CMAKE_CUDA_COMPILER})
#set (CMAKE_CUDA_COMPILER   C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit/CUDA/v11.3/bin/nvcc.exe)
#set (CUDA_NVCC_EXECUTABLE  C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit/CUDA/v11.3/bin/nvcc.exe)
set (CUDA_TOOLKIT_ROOT_DIR C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit)
#set (CUDA_INCLUDE_DIRS     C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit/CUDA/v11.3/include/x64)
#include_directories(${CUDA_INCLUDE_DIRS})
#set (CUDA_CUDART_LIBRARY   C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit/CUDA/v11.3/lib)
#message(${CMAKE_CUDA_COMPILER})

#enable_language(CUDA)
#include(CheckLanguage)
#check_language(CUDA)
#if(NOT DEFINED CMAKE_CUDA_STANDARD)
#    set(CMAKE_CUDA_STANDARD 11)
#    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
#endif()


set (CMAKE_PREFIX_PATH ../../../libtorch)
include_directories(../../../libtorch/include)
message(Prefix: ${CMAKE_PREFIX_PATH})
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(example-app libtorch.cpp evalModel.cpp)
target_link_libraries(example-app "${TORCH_LIBRARIES}")
if(MSVC)
    target_compile_options(example-app PRIVATE /W0)
endif()
set_property(TARGET example-app PROPERTY CXX_STANDARD 17)

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    add_custom_command(TARGET example-app
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${TORCH_DLLS}
            $<TARGET_FILE_DIR:example-app>)
endif (MSVC)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#wxwidgets
set(wxWidgets_ROOT_DIR "C:/Program Files/wxWidgets-3.1.5")
#set(wxWidgets_ROOT_DIR "D:/Program Files/wxWidgets-3.1.5")
find_package(wxWidgets 3 COMPONENTS  core base adv html xml aui REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(ChessAIinterface WIN32 ChessGUI.cpp)
set_target_properties(ChessAIinterface PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)


#link libraries
target_link_libraries(ChessAIinterface
        ${wxWidgets_LIBRARIES}
        )

#set compile flags
target_compile_options(ChessAIinterface
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>: /W3 /WX /Zc:__cplusplus>
        PUBLIC
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>>:--coverage -fprofile-arcs -ftest-coverage>)


# CMakeList.txt : CMake project for UAFTI-ChessAI, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("UAFTI-ChessAI")
set(CMAKE_CXX_STANDARD 20)

link_directories()

#Used external libraries (.lib)
include_directories()
link_libraries()

#resource files
set (APP_ICON_RESOURCE_WINDOWS "ChessAI.rc")

# Add source to this project's executable.

# Main UCI AI executable
set (AIlist "ChessAI.cpp" "ClassicBitBoard.cpp" "ClassicBitBoard.h" 
						  "EvalAgents.cpp" "EvalAgents.h" 
						  "Limits.cpp" "Limits.h" 
						  "PerfT.cpp" "PerfT.h" 
						  "Polyglot.cpp" "Polyglot.h" 
						  "SearchAgents.cpp" "SearchAgents.h"
						  "TranspositionTable.cpp" "TranspositionTable.h" 
						  "UCI.cpp" "UCI.h" 
						  "UCIOptions.cpp" "UCIOptions.h")

add_executable (UAFTI-ChessAI ${AIlist} ${APP_ICON_RESOURCE_WINDOWS})
set_target_properties(UAFTI-ChessAI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)


#Optiomization flags
message("Build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES "Release")
	target_compile_options(UAFTI-ChessAI  PRIVATE
	  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX- /Oi /Ot /O2 /Zi /GL>
	  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O3>)
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/ZC:inline>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/sdl>")

	#enable profiling option for visual studio
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")
	SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
endif()


#output information
message("${CMAKE_CXX_COMPILER}")
message("Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message("Cmake cxx flags:${CMAKE_CXX_FLAGS}")
message("Cmake cxx flags release:${CMAKE_CXX_FLAGS_RELEASE}")
message("Cmake cxx flags debug:${CMAKE_CXX_FLAGS_DEBUG}")

# TODO: Add tests and install targets if needed.

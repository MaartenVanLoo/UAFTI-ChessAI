# CMakeList.txt : CMake project for UAFTI-ChessAI, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("UAFTI-ChessAI")
set(CMAKE_CXX_STANDARD 20)

include_directories()
link_directories()

#Used external libraries (.lib)
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
set_target_properties(UAFTI-ChessAI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# TODO: Add tests and install targets if needed.

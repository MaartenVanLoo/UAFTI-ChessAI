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
message(${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE AIlist CONFIGURE_DEPENDS  ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/*.h")
message(${AIlist})
#set (AIlist  "src/chess/ClassicBitBoard.cpp" "src/chess/ClassicBitBoard.h"
#			 "src/EvalAgents.cpp" 			 "src/EvalAgents.h"
#			 "src/UCI/Limits.cpp" 		     "src/UCI/Limits.h"
#			 "src/UCI/PerfT.cpp" 			 "src/UCI/PerfT.h"
#			 "src/chess/Polyglot.cpp" 		 "src/chess/Polyglot.h"
#			 "src/SearchAgents.cpp" 		 "src/SearchAgents.h"
#			 "src/TranspositionTable.cpp" 	 "src/TranspositionTable.h"
#			 "src/UCI/UCI.cpp" 			     "src/UCI/UCI.h"
#			 "src/UCI/UCIOptions.cpp" 		 "src/UCI/UCIOptions.h"
#			 "ChessAI.cpp")

add_executable (UAFTI-ChessAI "ChessAI.cpp" ${AIlist} ${APP_ICON_RESOURCE_WINDOWS})

set_target_properties(UAFTI-ChessAI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)


#Optiomization flags
message("Build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES "Release")
	target_compile_options(UAFTI-ChessAI  PRIVATE
	  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX- /Oi /Ot /O2 /Zi /GL /MP>
	  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O3>)
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
	target_link_options(UAFTI-ChessAI PRIVATE "$<$<CONFIG:Release>:/LTCG>")


	#enable profiling option for visual studio
	SET_TARGET_PROPERTIES(UAFTI-ChessAI PROPERTIES LINK_FLAGS "/PROFILE")
	SET_TARGET_PROPERTIES(UAFTI-ChessAI PROPERTIES LINK_FLAGS "/DEBUG")
endif()
set(MSVC_COMPILE_FLAGS "/MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}" )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_COMPILE_FLAGS}" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MSVC_COMPILE_FLAGS}" )

#output information
message("${CMAKE_CXX_COMPILER}")
message("Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message("Cmake cxx flags:${CMAKE_CXX_FLAGS}")
message("Cmake cxx flags release:${CMAKE_CXX_FLAGS_RELEASE}")
message("Cmake cxx flags debug:${CMAKE_CXX_FLAGS_DEBUG}")

# TODO: Add tests and install targets if needed.
#include tests
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/)
add_subdirectory(src/GUI)


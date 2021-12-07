cmake_minimum_required(VERSION 3.14)
project ("UAFTI-ChessAI")
# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()


add_executable(
        TestChessAI
        ${AIlist}
        test.cpp
        TestClassicBitBoard.cpp
        TestPerfT.cpp
)
target_link_libraries(
        TestChessAI
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(TestChessAI)
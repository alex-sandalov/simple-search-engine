include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(SOURCES
        SearcherTests.cpp
        IndexerTests.cpp
        ParserArgumentTests.cpp
)

add_executable(SearchEngineTests ${SOURCES})

target_link_libraries(
        SearchEngineTests
        IndexerLibrary
        SearcherLibrary
        ParserArgumentLibrary
        GTest::gtest_main
)

target_include_directories(SearchEngineTests PRIVATE "${PROJECT_SOURCE_DIR}/lib")

include(GoogleTest)
gtest_discover_tests(SearchEngineTests)
cmake_minimum_required(VERSION 3.30)

file(GLOB TEST_SOURCES test_*.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_SOURCE})

    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

    target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)

    catch_discover_tests(${TEST_NAME} OUTPUT_DIR ${CMAKE_BINARY_DIR})
endforeach()

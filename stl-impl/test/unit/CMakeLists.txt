# 单元测试
file(GLOB_RECURSE UNIT_TESTS "*.cpp")

foreach(TEST ${UNIT_TESTS})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} GTest::GTest GTest::Main Threads::Threads)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
include_directories(${CMAKE_SOURCE_DIR}/src)

add_test(test_date_time test.cpp test_date_time.cpp)
target_link_libraries(test_date_time PRIVATE ${LIB_NAME})

add_test(test_int test.cpp test_int.cpp)
target_link_libraries(test_int PRIVATE ${LIB_NAME})

add_test(test_money test.cpp test_money.cpp)
target_link_libraries(test_money PRIVATE ${LIB_NAME})

add_test(test_str test.cpp test_str.cpp)
target_link_libraries(test_str PRIVATE ${LIB_NAME})

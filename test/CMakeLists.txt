add_executable(box_test test_main.cpp box_test.cpp)
target_include_directories(box_test PRIVATE ${INCLUDE_DIR})
target_include_directories(box_test SYSTEM PRIVATE ${LIBRARY_DIR})

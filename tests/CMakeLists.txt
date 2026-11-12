add_executable(bitforge_tests
    test_bitops.c
)

target_link_libraries(bitforge_tests
    PRIVATE bitforge
)

add_test(
    NAME bitforge_tests
    COMMAND bitforge_tests
)

add_executable(stream_tests
    test_stream.c
)

target_link_libraries(stream_tests
    PRIVATE bitforge
)

add_test(
    NAME stream_tests
    COMMAND stream_tests
)

add_executable(pattern_tests
    test_pattern.c
)

target_link_libraries(pattern_tests
    PRIVATE bitforge
)

add_test(
    NAME pattern_tests
    COMMAND pattern_tests
)

add_executable(buffer_tests
    test_buffer.c
)

target_link_libraries(buffer_tests
    PRIVATE bitforge
)

add_test(
    NAME buffer_tests
    COMMAND buffer_tests
)

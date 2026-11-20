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

add_executable(io_tests
    test_io.c
)

target_link_libraries(io_tests
    PRIVATE bitforge
)

add_test(
    NAME io_tests
    COMMAND io_tests
)

add_executable(status_tests
    test_status.c
)

target_link_libraries(status_tests
    PRIVATE bitforge
)

add_test(
    NAME status_tests
    COMMAND status_tests
)

add_executable(parser_tests
    test_parser.c
)

target_link_libraries(parser_tests
    PRIVATE bitforge
)

add_test(
    NAME parser_tests
    COMMAND parser_tests
)

add_executable(png_tests
    test_png.c
)

target_link_libraries(png_tests
    PRIVATE bitforge
)

add_test(
    NAME png_tests
    COMMAND png_tests
)

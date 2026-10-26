
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


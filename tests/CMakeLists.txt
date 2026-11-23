set(TESTS
    bitops
    stream
    pattern
    buffer
    io
    status
    parser
    png
    bmp
    wav
    elf
    pe
)

foreach(test IN LISTS TESTS)
    add_executable(${test}_tests
        test_${test}.c
    )

    target_link_libraries(${test}_tests
        PRIVATE bitforge
    )

    add_test(
        NAME ${test}_tests
        COMMAND ${test}_tests
    )
endforeach()

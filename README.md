# BitForge

**BitForge** is a lightweight C17 library for bit-level manipulation and analysis of 8-bit values.

## Features

- Read, set, clear, and toggle individual bits
- Count set bits in a byte
- Check whether a byte is a power of two
- Reverse the bit order of a byte
- Rotate bits to the left
- Status codes for error handling

## Requirements

- A C compiler with C17 support
- CMake 3.20 or newer

## Build

Configure and build the project using CMake:

```bash
cmake -S . -B build
cmake --build build
```

## Testing

Build the project and run the test suite with CTest:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

## Usage

Include the public header in your C source file:

```c
#include <bitforge/bitops.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t value = 0x00;

    if (bf_set(&value, 3) != BF_OK) {
        return 1;
    }

    printf("Value: 0x%02X\n", (unsigned int)value);

    return 0;
}
```

This example sets bit 3, changing the value from `0x00` to `0x08`.

## Project Status

BitForge is under active development. Its API and features may evolve.

## License

See the [LICENSE](LICENSE) file.

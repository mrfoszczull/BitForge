# BitForge

**BitForge** is a lightweight C17 library for bit-level manipulation and analysis of 8-bit values.

It provides a simple API for common bitwise operations, bit counting, bit reversal, and bit rotation, with status codes for error handling.

## Features

- Read individual bits
- Set, clear, and toggle individual bits
- Count the number of set bits in a byte
- Check whether a byte is a power of two
- Reverse the bit order of a byte
- Rotate bits to the left
- Status codes for error handling

## Requirements

- A C compiler with C17 support
- CMake 3.20 or newer
- GNU Make (optional, for Makefile shortcuts)

## Getting Started

Clone the repository and enter the project directory:

```bash
git clone https://github.com/mrfoszczull/BitForge
cd bitforge
```

Configure and build the project with CMake:

```bash
cmake -S . -B build
cmake --build build
```

## Testing

Run the test suite using CTest:

```bash
ctest --test-dir build --output-on-failure
```

If you use the provided Makefile, you can also run:

```bash
make test
```

Run both regular and sanitizer-enabled tests:

```bash
make check
```

Build and test with sanitizers only:

```bash
make sanitize
```

Remove generated build directories:

```bash
make clean
```

## Usage

Include the public header in your C source file:

```c
#include <bitforge/bitops.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint8_t value = 0x00;

    if (bf_set(&value, 3) != BF_OK) {
        return 1;
    }

    printf("Value: 0x%02X\n", (unsigned int)value);

    return 0;
}
```

Output:

```text
Value: 0x08
```

This example sets bit 3, changing the value from `0x00` to `0x08`.

## API Overview

BitForge provides functions for:

- Reading and modifying individual bits
- Counting set bits
- Checking whether a byte is a power of two
- Reversing the bit order of a byte
- Rotating bits to the left

Operations that can fail return status codes, allowing callers to check whether an operation completed successfully.

See the public header, `include/bitforge/bitops.h`, for the available functions, parameter types, and status codes.

## Project Status

BitForge 1.0.0 is the first stable release.

The public API is considered stable for the 1.x series. Breaking API changes are not expected before a future major release.

## License

See the [LICENSE](LICENSE) file for license information.


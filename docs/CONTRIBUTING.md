# Contributing to BitForge

Thank you for your interest in contributing to BitForge! Bug reports, fixes,
tests, documentation improvements and new ideas are all welcome.

## Table of Contents

- [Reporting bugs](#reporting-bugs)
- [Suggesting features](#suggesting-features)
- [Development setup](#development-setup)
- [Making changes](#making-changes)
- [Coding guidelines](#coding-guidelines)
- [API conventions](#api-conventions)
- [Testing](#testing)
- [Submitting a pull request](#submitting-a-pull-request)
- [API stability](#api-stability)
- [License](#license)

## Reporting bugs

Before opening an issue, please check whether it has already been reported.
A good bug report includes:

- The BitForge version (or commit hash).
- Your platform, compiler and CMake version.
- The exact steps or a minimal code snippet that reproduces the problem.
- The expected result and the actual result.

If the problem looks like a memory error or undefined behavior, please include
the output of a sanitizer-enabled build (see [Testing](#testing)).

## Suggesting features

Open an issue before starting work on a larger feature. Describe the use case
first and the proposed API second. This avoids wasted effort on changes that
do not fit the direction of the library.

Keep in mind that the public API of the 1.x series is intended to remain
stable, so new features are added without modifying existing functions
(see [API stability](#api-stability)).

## Development setup

Requirements:

- A C compiler with C17 support (GCC or Clang).
- CMake 3.21 or newer.
- Make (optional, for the provided Makefile).
- ClangFormat (for formatting).

Fork the repository, clone your fork and build:

```bash
git clone https://github.com/<your-username>/BitForge.git
cd BitForge
make build
```

Useful targets:

| Command | Description |
|---|---|
| `make build` | Configure and build |
| `make test` | Build and run tests with CTest |
| `make sanitize` | Build and run tests with AddressSanitizer and UBSan |
| `make check` | Run regular and sanitizer-enabled tests |
| `make format` | Format sources with ClangFormat |
| `make format-check` | Check formatting without modifying files |
| `make clean` | Remove build directories |

## Making changes

1. Create a branch from `main` with a descriptive name, for example
   `fix-rotate-overflow` or `add-bitbuf-api`.
2. Keep each pull request focused on one logical change. Unrelated fixes
   belong in separate pull requests.
3. Add or update tests together with the code change.
4. Update the documentation (Doxygen comments in headers, README) when the
   behavior or the API changes.
5. Run `make format` and `make check` before pushing.

Commit messages should have a short summary line in the imperative mood
(for example `Fix range check in bf_get_bits`), optionally followed by a blank
line and a longer explanation of *why* the change is needed.

## Coding guidelines

BitForge is written in **C17** and must compile without warnings with
`-Wall -Wextra -Wpedantic`.

- **Formatting** is defined by `.clang-format` (LLVM-based, Linux brace style,
  tabs, 80 columns). Do not format by hand; run `make format`.
- **Naming**: public functions use the `bf_` prefix, types and constants use
  `bf_` / `BF_`. Internal helpers must be `static`.
- **Portability**: use fixed-width types from `<stdint.h>` for data and
  `size_t` for sizes and indices. Do not rely on implementation-defined or
  undefined behavior (signed overflow, shifts by the full width of a type,
  strict aliasing violations, unaligned access through casted pointers).
- **Dependencies**: the library has no runtime dependencies beyond the C
  standard library. Please keep it that way.
- **Comments**: public functions are documented in headers using Doxygen
  (`@brief`, `@param`, `@return`). Describe valid ranges and error conditions.

## API conventions

New public functions should follow the existing conventions:

- Every function that can fail returns a `bf_status`. Results are returned
  through output parameters.
- Arguments are validated **before** any output is written. When a function
  returns an error, its output parameters and the data it was asked to modify
  must be left unchanged.
- Invalid arguments (`NULL` pointers, out-of-range indices or counts) return
  `BF_EINVAL`.
- Bit indices are **LSB-first**: index `0` is the least significant bit.
- Range operations take `bit_index` (first bit) and `bit_count` (number of
  bits). Range checks must be written so that they cannot overflow `size_t`.
- New status codes are only added when a function can actually return them.

## Testing

Tests live in `tests/` and use the lightweight `CHECK` macro defined in
`tests/test_bitops.c`.

```bash
make test       # regular build
make sanitize   # AddressSanitizer + UndefinedBehaviorSanitizer
make check      # both
```

When adding or changing a function, please cover:

- The normal behavior.
- Boundary values (first and last valid index, minimum and maximum count).
- Every error path (`NULL` pointers, indices and counts out of range).
- Behavior on error: data must stay unchanged.

Because the domain of 8-bit operations is tiny (256 values), exhaustive tests
are encouraged wherever they are practical. A bug fix should come with a test
that fails without the fix.

## Submitting a pull request

Before opening a pull request, make sure that:

- [ ] The project builds without warnings.
- [ ] `make check` passes (tests and sanitizers).
- [ ] `make format-check` passes.
- [ ] New and changed public functions are documented.
- [ ] New behavior is covered by tests.
- [ ] The pull request description explains **what** changed and **why**,
      and links the related issue, if there is one.

CI runs the build and tests with GCC and Clang, a sanitizer build, a format
check and an install test. All checks must pass before a pull request can be
merged. Reviews may ask for changes; this is a normal part of the process.

## API stability

The public API of the 1.x release series is intended to remain stable.
The following are considered breaking changes and are reserved for a major
release:

- Removing or renaming a public function, type or header.
- Changing the signature or documented behavior of an existing function.
- Changing the numeric values or meaning of existing `bf_status` codes.
- Changing the bit indexing convention.

Adding new functions, headers and status codes is compatible with 1.x.

## License

By contributing, you agree that your contributions will be licensed under the
[MIT License](../LICENSE), the same license that covers the project.

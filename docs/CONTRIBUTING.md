# Contributing to BitForge

Thank you for your interest in contributing to BitForge!

Bug reports, bug fixes, tests, documentation improvements, and new ideas are all welcome. This guide explains how to report issues, propose changes, build the project, and submit contributions.

## Table of Contents

- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Coding Guidelines](#coding-guidelines)
- [API Conventions](#api-conventions)
- [Testing](#testing)
- [Submitting a Pull Request](#submitting-a-pull-request)
- [API Stability](#api-stability)
- [Additional Resources](#additional-resources)

## Reporting Bugs

Before opening an issue, check whether it has already been reported.

A good bug report should include:

- The BitForge version or commit hash.
- Your operating system, compiler, and CMake version.
- The exact steps or a minimal code example needed to reproduce the problem.
- The expected result and the actual result.

If the problem involves a possible memory error or undefined behavior, include the output of a sanitizer-enabled build (see [Testing](#testing)).

For security-related vulnerabilities, follow the instructions in the project's [Security Policy](SECURITY.md) rather than reporting them publicly.

## Suggesting Features

Open an issue before starting work on a larger feature.

Describe the use case first and the proposed API second. This helps ensure that new functionality fits the project's direction and avoids unnecessary work.

Keep in mind that the public API of the 1.x release series is intended to remain stable. New features should be introduced without modifying existing functions (see [API Stability](#api-stability)).

## Development Setup

### Requirements

- A C compiler with C17 support (GCC or Clang).
- CMake 3.21 or newer.
- Make (optional, for the provided Makefile).
- ClangFormat (for source formatting).

### Building the Project

Fork the repository, clone your fork, and build the project:

```bash
git clone https://github.com/<your-username>/BitForge.git
cd BitForge
make build
```

### Available Make Targets

| Command | Description |
|---|---|
| `make build` | Configure and build the project. |
| `make test` | Build and run tests with CTest. |
| `make sanitize` | Build and run tests with AddressSanitizer and UBSan. |
| `make check` | Run regular and sanitizer-enabled tests. |
| `make format` | Format source files using ClangFormat. |
| `make format-check` | Check formatting without modifying files. |
| `make clean` | Remove build directories. |

## Making Changes

1. Create a branch from `main` with a descriptive name, such as `fix-rotate-overflow` or `add-bitbuf-api`.
2. Keep each pull request focused on one logical change. Unrelated fixes should be submitted separately.
3. Add or update tests alongside code changes.
4. Update the documentation, including Doxygen comments in headers and the README, whenever API behavior or functionality changes.
5. Run `make format` and `make check` before pushing your changes.

### Commit Messages

Commit messages should contain a short summary written in the imperative mood.

For example:

```text
Fix range check in bf_get_bits
```

An optional longer explanation may follow after a blank line, describing why the change is necessary.

## Coding Guidelines

BitForge is written in **C17** and must compile without warnings when using:

```text
-Wall -Wextra -Wpedantic
```

Follow these guidelines when contributing:

- **Formatting:** The `.clang-format` file defines the project's formatting rules (LLVM-based, Linux brace style, tabs, and 80-column limit). Use `make format` instead of formatting code manually.
- **Naming:** Public functions must use the `bf_` prefix. Types and constants must follow the `bf_` and `BF_` naming conventions, respectively. Internal helper functions must be declared `static`.
- **Portability:** Use fixed-width integer types from `<stdint.h>` for data and `size_t` for sizes and indices. Avoid implementation-defined behavior and undefined behavior, including signed overflow, shifts by the full width of a type, strict aliasing violations, and unaligned access through cast pointers.
- **Dependencies:** The library must not introduce runtime dependencies beyond the C standard library.
- **Documentation:** Public functions must be documented in headers using Doxygen (`@brief`, `@param`, and `@return`). Document valid argument ranges and possible error conditions.

## API Conventions

New public functions must follow the existing API conventions:

- Every function that can fail must return a `bf_status`. Results should be provided through output parameters.
- Validate all arguments before writing to output parameters or modifying data.
- If a function returns an error, its output parameters and any data it was asked to modify must remain unchanged.
- Invalid arguments, including `NULL` pointers and out-of-range indices or counts, must return `BF_EINVAL`.
- Bit indices must be **LSB-first**: index `0` represents the least significant bit.
- Range operations must use `bit_index` (the first bit) and `bit_count` (the number of bits). Range checks must be implemented so that they cannot overflow `size_t`.
- New status codes should only be introduced when a function can actually return them.

## Testing

Tests are located in `tests/` and use the lightweight `CHECK` macro defined in `tests/test_bitops.c`.

### Running Tests

Run the following commands from the project root:

```bash
make test       # Regular build and tests
make sanitize   # AddressSanitizer and UndefinedBehaviorSanitizer
make check      # Regular and sanitizer-enabled tests
```

### Test Coverage

When adding or modifying a function, cover:

- Normal operation and expected behavior.
- Boundary values, including the first and last valid indices and minimum and maximum counts.
- Every error path, including `NULL` pointers and out-of-range indices or counts.
- Error handling: data and output parameters must remain unchanged when an operation fails.

Because the domain of 8-bit operations is small (256 possible values), exhaustive testing is encouraged wherever practical.

Every bug fix should include a regression test that fails without the fix.

## Submitting a Pull Request

Before opening a pull request, make sure that:

- The project builds without warnings.
- `make check` passes, including tests and sanitizers.
- `make format-check` passes.
- New and modified public functions are documented.
- New functionality is covered by tests.
- The pull request description explains **what** changed and **why**, and links the related issue when applicable.

CI runs the build and tests with GCC and Clang, a sanitizer build, a formatting check, and an installation test.

All CI checks must pass before a pull request can be merged. Reviewers may request changes; this is a normal part of the contribution process.

## API Stability

The public API of the 1.x release series is intended to remain stable.

The following changes are considered breaking and are reserved for a major release:

- Removing or renaming a public function, type, or header.
- Changing the signature or documented behavior of an existing function.
- Changing the numeric values or meaning of existing `bf_status` codes.
- Changing the bit-indexing convention.

Adding new functions, headers, and status codes is compatible with the 1.x release series.

## Additional Resources

- [Project Documentation](README.md)
- [Security Policy](SECURITY.md)
- [License](../LICENSE)

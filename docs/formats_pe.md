

# PE / PE32+ Format Parser

Zero-allocation C17 parser for DOS, COFF, PE32/PE32+, and section tables.

Parses MZ/DOS headers, `PE\0\0` signature and COFF header, PE32 (`0x10B`) and PE32+ (`0x20B`) optional headers, data directories, section headers, RVA-to-file-offset translation, and zero-copy byte slices over [`bf_stream_t`](stream.md#bf_stream_t).

## Classes

| Name | Description |
|------|-------------|
| [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) | Parsed DOS header (`IMAGE_DOS_HEADER`) fields needed for PE navigation. |
| [`bf_pe_coff_header_t`](#bf_pe_coff_header_t) | COFF file header (`IMAGE_FILE_HEADER`) fields. |
| [`bf_pe_optional_header_t`](#bf_pe_optional_header_t) | Unified PE32 / PE32+ optional header view. |
| [`bf_pe_data_dir_t`](#bf_pe_data_dir_t) | Single `IMAGE_DATA_DIRECTORY` entry (`VirtualAddress` + `Size`). |
| [`bf_pe_section_t`](#bf_pe_section_t) | Parsed COFF/PE section header (`IMAGE_SECTION_HEADER`). |
| [`bf_pe_slice_t`](#bf_pe_slice_t) | Zero-copy slice pointing directly into the backing [`bf_stream_t`](stream.md#bf_stream_t) buffer. |

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_pe_data_dir_idx_t`](#bf_pe_data_dir_idx_t)  | Well-known PE data directory indices (`IMAGE_DIRECTORY_ENTRY_*`). |

---

### bf_pe_data_dir_idx_t

```cpp
enum bf_pe_data_dir_idx_t
```

Defined in include/bitforge/formats/pe.h:61

Well-known PE data directory indices (`IMAGE_DIRECTORY_ENTRY_*`).

| Value | Description |
|-------|-------------|
| `BF_PE_DIR_EXPORT` | Export directory table. |
| `BF_PE_DIR_IMPORT` | Import directory table. |
| `BF_PE_DIR_RESOURCE` | Resource directory table. |
| `BF_PE_DIR_EXCEPTION` | Exception table (`.pdata`). |
| `BF_PE_DIR_SECURITY` | Attribute certificate table (file offset). |
| `BF_PE_DIR_BASERELOC` | Base relocation table (`.reloc`). |
| `BF_PE_DIR_DEBUG` | Debug directory. |
| `BF_PE_DIR_TLS` | TLS directory. |
| `BF_PE_DIR_IAT` | Import Address Table (IAT). |
| `BF_PE_DIR_DELAY_IMPORT` | Delay-load import descriptor. |
| `BF_PE_DIR_CLR_RUNTIME` | CLR runtime header. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_pe_read_dos_header`](#bf_pe_read_dos_header)  | Reads and validates the MZ/DOS header without mutating on failure. |
| [`bf_status`](status.md#bf_status) | [`bf_pe_read_coff_header`](#bf_pe_read_coff_header)  | Reads `"PE\\0\\0"` signature and COFF header at `dos->pe_offset`. |
| [`bf_status`](status.md#bf_status) | [`bf_pe_read_optional_header`](#bf_pe_read_optional_header)  | Reads and validates a PE32 or PE32+ optional header after `coff`. |
| [`bf_status`](status.md#bf_status) | [`bf_pe_read_data_dir`](#bf_pe_read_data_dir)  | Reads data directory entry `dir_idx` from the optional header table. |
| [`bf_status`](status.md#bf_status) | [`bf_pe_read_section`](#bf_pe_read_section)  | Reads section header `sec_idx` (`< coff->number_of_sections`). |
| [`bf_status`](status.md#bf_status) | [`bf_pe_rva_to_offset`](#bf_pe_rva_to_offset)  | Translates a 32-bit `rva` into a raw file offset using section headers. |
| [`bf_status`](status.md#bf_status) | [`bf_pe_get_slice`](#bf_pe_get_slice)  | Returns a zero-copy byte slice at `[file_offset, file_offset + size)`. |

---

### bf_pe_read_dos_header

```cpp
bf_status bf_pe_read_dos_header(bf_stream_t * stream, bf_pe_dos_header_t * out_dos)
```

Defined in include/bitforge/formats/pe.h:204

Reads and validates the MZ/DOS header without mutating on failure.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at DOS header. |
| `out_dos` | [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) * | Populated DOS header on success. |

---

### bf_pe_read_coff_header

```cpp
bf_status bf_pe_read_coff_header(const bf_stream_t * stream, const bf_pe_dos_header_t * dos, bf_pe_coff_header_t * out_coff)
```

Defined in include/bitforge/formats/pe.h:217

Reads `"PE\\0\\0"` signature and COFF header at `dos->pe_offset`.

Does not mutate the caller's [`Streams`](stream.md#streams) offset.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the PE image. |
| `dos` | const [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) * | Validated DOS header from [`bf_pe_read_dos_header`](#bf_pe_read_dos_header). |
| `out_coff` | [`bf_pe_coff_header_t`](#bf_pe_coff_header_t) * | Populated COFF header on success. |

---

### bf_pe_read_optional_header

```cpp
bf_status bf_pe_read_optional_header(const bf_stream_t * stream, const bf_pe_dos_header_t * dos, const bf_pe_coff_header_t * coff, bf_pe_optional_header_t * out_opt)
```

Defined in include/bitforge/formats/pe.h:232

Reads and validates a PE32 or PE32+ optional header after `coff`.

Does not mutate the caller's [`Streams`](stream.md#streams) offset.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the PE image. |
| `dos` | const [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) * | Validated DOS header. |
| `coff` | const [`bf_pe_coff_header_t`](#bf_pe_coff_header_t) * | Validated COFF header. |
| `out_opt` | [`bf_pe_optional_header_t`](#bf_pe_optional_header_t) * | Populated optional header on success. |

---

### bf_pe_read_data_dir

```cpp
bf_status bf_pe_read_data_dir(const bf_stream_t * stream, const bf_pe_optional_header_t * opt, uint32_t dir_idx, bf_pe_data_dir_t * out_dir)
```

Defined in include/bitforge/formats/pe.h:246

Reads data directory entry `dir_idx` from the optional header table.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EOVERFLOW`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the PE image. |
| `opt` | const [`bf_pe_optional_header_t`](#bf_pe_optional_header_t) * | Parsed optional header. |
| `dir_idx` | `uint32_t` | Zero-based directory index (`< opt->number_of_rva_and_sizes`). |
| `out_dir` | [`bf_pe_data_dir_t`](#bf_pe_data_dir_t) * | Populated data directory entry on success. |

---

### bf_pe_read_section

```cpp
bf_status bf_pe_read_section(const bf_stream_t * stream, const bf_pe_dos_header_t * dos, const bf_pe_coff_header_t * coff, uint16_t sec_idx, bf_pe_section_t * out_sec)
```

Defined in include/bitforge/formats/pe.h:263

Reads section header `sec_idx` (`< coff->number_of_sections`).

Does not mutate the caller's [`Streams`](stream.md#streams) offset.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the PE image. |
| `dos` | const [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) * | Validated DOS header. |
| `coff` | const [`bf_pe_coff_header_t`](#bf_pe_coff_header_t) * | Validated COFF header. |
| `sec_idx` | `uint16_t` | Zero-based section index. |
| `out_sec` | [`bf_pe_section_t`](#bf_pe_section_t) * | Populated section header on success. |

---

### bf_pe_rva_to_offset

```cpp
bf_status bf_pe_rva_to_offset(const bf_stream_t * stream, const bf_pe_dos_header_t * dos, const bf_pe_coff_header_t * coff, uint32_t rva, uint64_t * out_offset)
```

Defined in include/bitforge/formats/pe.h:279

Translates a 32-bit `rva` into a raw file offset using section headers.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the PE image. |
| `dos` | const [`bf_pe_dos_header_t`](#bf_pe_dos_header_t) * | Validated DOS header. |
| `coff` | const [`bf_pe_coff_header_t`](#bf_pe_coff_header_t) * | Validated COFF header. |
| `rva` | `uint32_t` | Relative virtual address to translate. |
| `out_offset` | `uint64_t *` | Resolved file byte offset on success. |

---

### bf_pe_get_slice

```cpp
bf_status bf_pe_get_slice(const bf_stream_t * stream, uint64_t file_offset, uint64_t size, bf_pe_slice_t * out_slice)
```

Defined in include/bitforge/formats/pe.h:292

Returns a zero-copy byte slice at `[file_offset, file_offset + size)`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EOF`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream backing the slice. |
| `file_offset` | `uint64_t` | Starting byte offset within [`Streams`](stream.md#streams). |
| `size` | `uint64_t` | Requested slice length in bytes. |
| `out_slice` | [`bf_pe_slice_t`](#bf_pe_slice_t) * | Populated zero-copy slice on success. |


## Class Definitions



### bf_pe_dos_header_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_dos_header_t
```

Defined in include/bitforge/formats/pe.h:80

Parsed DOS header (`IMAGE_DOS_HEADER`) fields needed for PE navigation.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`e_magic`](#e_magic)  | DOS magic (`0x5A4D`, `"MZ"`). |
| `uint16_t` | [`e_cblp`](#e_cblp)  | Bytes on last page of file. |
| `uint16_t` | [`e_cp`](#e_cp)  | Pages in file. |
| `uint16_t` | [`e_lfanew`](#e_lfanew)  | Low 16 bits of [`e_lfanew`](#e_lfanew) (`0` if `pe_offset > UINT16_MAX`). |
| `uint32_t` | [`pe_offset`](#pe_offset)  | Full 32-bit [`e_lfanew`](#e_lfanew) offset to `"PE\\0\\0"`. |

---

##### e_magic

```cpp
uint16_t e_magic
```

Defined in include/bitforge/formats/pe.h:81

DOS magic (`0x5A4D`, `"MZ"`).

---

##### e_cblp

```cpp
uint16_t e_cblp
```

Defined in include/bitforge/formats/pe.h:82

Bytes on last page of file.

---

##### e_cp

```cpp
uint16_t e_cp
```

Defined in include/bitforge/formats/pe.h:83

Pages in file.

---

##### e_lfanew

```cpp
uint16_t e_lfanew
```

Defined in include/bitforge/formats/pe.h:84

Low 16 bits of [`e_lfanew`](#e_lfanew) (`0` if `pe_offset > UINT16_MAX`).

---

##### pe_offset

```cpp
uint32_t pe_offset
```

Defined in include/bitforge/formats/pe.h:87

Full 32-bit [`e_lfanew`](#e_lfanew) offset to `"PE\\0\\0"`.



### bf_pe_coff_header_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_coff_header_t
```

Defined in include/bitforge/formats/pe.h:93

COFF file header (`IMAGE_FILE_HEADER`) fields.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`machine`](#machine)  | Target machine type (`IMAGE_FILE_MACHINE_*`). |
| `uint16_t` | [`number_of_sections`](#number_of_sections)  | Number of section headers following optional header. |
| `uint32_t` | [`time_date_stamp`](#time_date_stamp)  | Linker timestamp (seconds since epoch). |
| `uint32_t` | [`pointer_to_symbol_table`](#pointer_to_symbol_table)  | Legacy COFF symbol table offset. |
| `uint32_t` | [`number_of_symbols`](#number_of_symbols)  | Legacy COFF symbol count. |
| `uint16_t` | [`size_of_optional_header`](#size_of_optional_header)  | Byte size of optional header following COFF header. |
| `uint16_t` | [`characteristics`](#characteristics)  | File attribute flags (`IMAGE_FILE_*`). |

---

##### machine

```cpp
uint16_t machine
```

Defined in include/bitforge/formats/pe.h:94

Target machine type (`IMAGE_FILE_MACHINE_*`).

---

##### number_of_sections

```cpp
uint16_t number_of_sections
```

Defined in include/bitforge/formats/pe.h:95

Number of section headers following optional header.

---

##### time_date_stamp

```cpp
uint32_t time_date_stamp
```

Defined in include/bitforge/formats/pe.h:98

Linker timestamp (seconds since epoch).

---

##### pointer_to_symbol_table

```cpp
uint32_t pointer_to_symbol_table
```

Defined in include/bitforge/formats/pe.h:100

Legacy COFF symbol table offset.

---

##### number_of_symbols

```cpp
uint32_t number_of_symbols
```

Defined in include/bitforge/formats/pe.h:101

Legacy COFF symbol count.

---

##### size_of_optional_header

```cpp
uint16_t size_of_optional_header
```

Defined in include/bitforge/formats/pe.h:102

Byte size of optional header following COFF header.

---

##### characteristics

```cpp
uint16_t characteristics
```

Defined in include/bitforge/formats/pe.h:104

File attribute flags (`IMAGE_FILE_*`).



### bf_pe_optional_header_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_optional_header_t
```

Defined in include/bitforge/formats/pe.h:110

Unified PE32 / PE32+ optional header view.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`magic`](#magic)  | `BF_PE32_MAGIC` (`0x10B`) or `BF_PE32PLUS_MAGIC` (`0x20B`). |
| `uint8_t` | [`major_linker_version`](#major_linker_version)  | Major linker version. |
| `uint8_t` | [`minor_linker_version`](#minor_linker_version)  | Minor linker version. |
| `uint32_t` | [`size_of_code`](#size_of_code)  | Combined size of code (`.text`) sections. |
| `uint32_t` | [`size_of_initialized_data`](#size_of_initialized_data)  | Combined size of initialized data sections. |
| `uint32_t` | [`size_of_uninitialized_data`](#size_of_uninitialized_data)  | Combined size of uninitialized (`.bss`) sections. |
| `uint32_t` | [`address_of_entry_point`](#address_of_entry_point)  | Entry point RVA relative to [`image_base`](#image_base). |
| `uint32_t` | [`base_of_code`](#base_of_code)  | Code section base RVA. |
| `uint32_t` | [`base_of_data`](#base_of_data)  | Data section base RVA (`0` for PE32+). |
| `uint64_t` | [`image_base`](#image_base)  | Preferred load address (`u32` widened for PE32, `u64` for PE32+). |
| `uint32_t` | [`section_alignment`](#section_alignment)  | In-memory section alignment in bytes. |
| `uint32_t` | [`file_alignment`](#file_alignment)  | On-disk section alignment in bytes. |
| `uint32_t` | [`size_of_image`](#size_of_image)  | Total image size in memory (multiple of [`section_alignment`](#section_alignment)). |
| `uint32_t` | [`size_of_headers`](#size_of_headers)  | Combined size of DOS, PE, COFF, optional, and section headers. |
| `uint32_t` | [`check_sum`](#check_sum)  | PE image checksum. |
| `uint16_t` | [`subsystem`](#subsystem)  | Required Windows subsystem (`IMAGE_SUBSYSTEM_*`). |
| `uint16_t` | [`dll_characteristics`](#dll_characteristics)  | DLL characteristics flags (`IMAGE_DLLCHARACTERISTICS_*`). |
| `uint64_t` | [`size_of_stack_reserve`](#size_of_stack_reserve)  | Reserved virtual stack size in bytes. |
| `uint64_t` | [`size_of_stack_commit`](#size_of_stack_commit)  | Initially committed virtual stack size in bytes. |
| `uint64_t` | [`size_of_heap_reserve`](#size_of_heap_reserve)  | Reserved virtual heap size in bytes. |
| `uint64_t` | [`size_of_heap_commit`](#size_of_heap_commit)  | Initially committed virtual heap size in bytes. |
| `uint32_t` | [`loader_flags`](#loader_flags)  | Obsolete loader flags (`0`). |
| `uint32_t` | [`number_of_rva_and_sizes`](#number_of_rva_and_sizes)  | Number of `IMAGE_DATA_DIRECTORY` entries. |
| `uint64_t` | [`data_dirs_offset`](#data_dirs_offset)  | File offset where `IMAGE_DATA_DIRECTORY` array begins. |

---

##### magic

```cpp
uint16_t magic
```

Defined in include/bitforge/formats/pe.h:111

`BF_PE32_MAGIC` (`0x10B`) or `BF_PE32PLUS_MAGIC` (`0x20B`).

---

##### major_linker_version

```cpp
uint8_t major_linker_version
```

Defined in include/bitforge/formats/pe.h:113

Major linker version.

---

##### minor_linker_version

```cpp
uint8_t minor_linker_version
```

Defined in include/bitforge/formats/pe.h:114

Minor linker version.

---

##### size_of_code

```cpp
uint32_t size_of_code
```

Defined in include/bitforge/formats/pe.h:115

Combined size of code (`.text`) sections.

---

##### size_of_initialized_data

```cpp
uint32_t size_of_initialized_data
```

Defined in include/bitforge/formats/pe.h:116

Combined size of initialized data sections.

---

##### size_of_uninitialized_data

```cpp
uint32_t size_of_uninitialized_data
```

Defined in include/bitforge/formats/pe.h:118

Combined size of uninitialized (`.bss`) sections.

---

##### address_of_entry_point

```cpp
uint32_t address_of_entry_point
```

Defined in include/bitforge/formats/pe.h:120

Entry point RVA relative to [`image_base`](#image_base).

---

##### base_of_code

```cpp
uint32_t base_of_code
```

Defined in include/bitforge/formats/pe.h:122

Code section base RVA.

---

##### base_of_data

```cpp
uint32_t base_of_data
```

Defined in include/bitforge/formats/pe.h:123

Data section base RVA (`0` for PE32+).

---

##### image_base

```cpp
uint64_t image_base
```

Defined in include/bitforge/formats/pe.h:124

Preferred load address (`u32` widened for PE32, `u64` for PE32+).

---

##### section_alignment

```cpp
uint32_t section_alignment
```

Defined in include/bitforge/formats/pe.h:127

In-memory section alignment in bytes.

---

##### file_alignment

```cpp
uint32_t file_alignment
```

Defined in include/bitforge/formats/pe.h:128

On-disk section alignment in bytes.

---

##### size_of_image

```cpp
uint32_t size_of_image
```

Defined in include/bitforge/formats/pe.h:129

Total image size in memory (multiple of [`section_alignment`](#section_alignment)).

---

##### size_of_headers

```cpp
uint32_t size_of_headers
```

Defined in include/bitforge/formats/pe.h:131

Combined size of DOS, PE, COFF, optional, and section headers.

---

##### check_sum

```cpp
uint32_t check_sum
```

Defined in include/bitforge/formats/pe.h:133

PE image checksum.

---

##### subsystem

```cpp
uint16_t subsystem
```

Defined in include/bitforge/formats/pe.h:135

Required Windows subsystem (`IMAGE_SUBSYSTEM_*`).

---

##### dll_characteristics

```cpp
uint16_t dll_characteristics
```

Defined in include/bitforge/formats/pe.h:136

DLL characteristics flags (`IMAGE_DLLCHARACTERISTICS_*`).

---

##### size_of_stack_reserve

```cpp
uint64_t size_of_stack_reserve
```

Defined in include/bitforge/formats/pe.h:139

Reserved virtual stack size in bytes.

---

##### size_of_stack_commit

```cpp
uint64_t size_of_stack_commit
```

Defined in include/bitforge/formats/pe.h:140

Initially committed virtual stack size in bytes.

---

##### size_of_heap_reserve

```cpp
uint64_t size_of_heap_reserve
```

Defined in include/bitforge/formats/pe.h:143

Reserved virtual heap size in bytes.

---

##### size_of_heap_commit

```cpp
uint64_t size_of_heap_commit
```

Defined in include/bitforge/formats/pe.h:144

Initially committed virtual heap size in bytes.

---

##### loader_flags

```cpp
uint32_t loader_flags
```

Defined in include/bitforge/formats/pe.h:146

Obsolete loader flags (`0`).

---

##### number_of_rva_and_sizes

```cpp
uint32_t number_of_rva_and_sizes
```

Defined in include/bitforge/formats/pe.h:147

Number of `IMAGE_DATA_DIRECTORY` entries.

---

##### data_dirs_offset

```cpp
uint64_t data_dirs_offset
```

Defined in include/bitforge/formats/pe.h:149

File offset where `IMAGE_DATA_DIRECTORY` array begins.



### bf_pe_data_dir_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_data_dir_t
```

Defined in include/bitforge/formats/pe.h:156

Single `IMAGE_DATA_DIRECTORY` entry (`VirtualAddress` + `Size`).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`virtual_address`](#virtual_address)  | RVA of directory table (`0` if unused). |
| `uint32_t` | [`size`](#size-3)  | Byte size of directory table (`0` if unused). |

---

##### virtual_address

```cpp
uint32_t virtual_address
```

Defined in include/bitforge/formats/pe.h:158

RVA of directory table (`0` if unused).

---

##### size

```cpp
uint32_t size
```

Defined in include/bitforge/formats/pe.h:159

Byte size of directory table (`0` if unused).



### bf_pe_section_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_section_t
```

Defined in include/bitforge/formats/pe.h:165

Parsed COFF/PE section header (`IMAGE_SECTION_HEADER`).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`name`](#name)  | NUL-terminated inline name (or raw 8 chars). |
| `bool` | [`has_long_name`](#has_long_name)  | `true` when `name[0] == '/'` (COFF string table ref). |
| `uint32_t` | [`long_name_offset`](#long_name_offset)  | Parsed offset into COFF string table if [`has_long_name`](#has_long_name). |
| `uint32_t` | [`virtual_size`](#virtual_size)  | In-memory size (`Misc.VirtualSize`). |
| `uint32_t` | [`virtual_address`](#virtual_address-1)  | Section start RVA. |
| `uint32_t` | [`size_of_raw_data`](#size_of_raw_data)  | Payload size on disk in bytes. |
| `uint32_t` | [`pointer_to_raw_data`](#pointer_to_raw_data)  | File offset of section payload. |
| `uint32_t` | [`pointer_to_relocations`](#pointer_to_relocations)  | File offset of COFF relocations. |
| `uint32_t` | [`pointer_to_linenumbers`](#pointer_to_linenumbers)  | Obsolete line numbers offset. |
| `uint16_t` | [`number_of_relocations`](#number_of_relocations)  | Number of COFF relocations. |
| `uint16_t` | [`number_of_linenumbers`](#number_of_linenumbers)  | Obsolete line numbers count. |
| `uint32_t` | [`characteristics`](#characteristics-1)  | Section flags (`IMAGE_SCN_*`). |

---

##### name

```cpp
char name[BF_PE_SECTION_NAME_LEN+1U]
```

Defined in include/bitforge/formats/pe.h:166

NUL-terminated inline name (or raw 8 chars).

---

##### has_long_name

```cpp
bool has_long_name
```

Defined in include/bitforge/formats/pe.h:168

`true` when `name[0] == '/'` (COFF string table ref).

---

##### long_name_offset

```cpp
uint32_t long_name_offset
```

Defined in include/bitforge/formats/pe.h:170

Parsed offset into COFF string table if [`has_long_name`](#has_long_name).

---

##### virtual_size

```cpp
uint32_t virtual_size
```

Defined in include/bitforge/formats/pe.h:172

In-memory size (`Misc.VirtualSize`).

---

##### virtual_address

```cpp
uint32_t virtual_address
```

Defined in include/bitforge/formats/pe.h:173

Section start RVA.

---

##### size_of_raw_data

```cpp
uint32_t size_of_raw_data
```

Defined in include/bitforge/formats/pe.h:174

Payload size on disk in bytes.

---

##### pointer_to_raw_data

```cpp
uint32_t pointer_to_raw_data
```

Defined in include/bitforge/formats/pe.h:175

File offset of section payload.

---

##### pointer_to_relocations

```cpp
uint32_t pointer_to_relocations
```

Defined in include/bitforge/formats/pe.h:177

File offset of COFF relocations.

---

##### pointer_to_linenumbers

```cpp
uint32_t pointer_to_linenumbers
```

Defined in include/bitforge/formats/pe.h:178

Obsolete line numbers offset.

---

##### number_of_relocations

```cpp
uint16_t number_of_relocations
```

Defined in include/bitforge/formats/pe.h:179

Number of COFF relocations.

---

##### number_of_linenumbers

```cpp
uint16_t number_of_linenumbers
```

Defined in include/bitforge/formats/pe.h:180

Obsolete line numbers count.

---

##### characteristics

```cpp
uint32_t characteristics
```

Defined in include/bitforge/formats/pe.h:181

Section flags (`IMAGE_SCN_*`).



### bf_pe_slice_t

```cpp
#include <pe.h>
```

```cpp
struct bf_pe_slice_t
```

Defined in include/bitforge/formats/pe.h:191

Zero-copy slice pointing directly into the backing [`bf_stream_t`](stream.md#bf_stream_t) buffer.

:::warning
The [`data`](#data-4) pointer remains valid only while the underlying stream buffer stays alive and unmodified. 

:::

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data-4)  | Pointer to slice bytes, or `NULL` when `size == 0`. |
| `size_t` | [`size`](#size-4)  | Slice length in bytes. |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/pe.h:193

Pointer to slice bytes, or `NULL` when `size == 0`.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/formats/pe.h:194

Slice length in bytes.


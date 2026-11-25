

# ELF Format Parser

Zero-allocation C17 parser for ELF32 and ELF64 binaries (LE and BE).

Provides header inspection, extended section count/string-table index resolution (`PN_XNUM` / `SHN_XINDEX`), program and section headers, section name lookup via `.shstrtab`, symbol tables (`.symtab` / `.dynsym`), `.dynamic` entries, relocation entries (`SHT_RELA` / `SHT_REL`), and zero-copy slice views over an aligned [`bf_stream_t`](stream.md#bf_stream_t).

## Classes

| Name | Description |
|------|-------------|
| [`bf_elf_header_t`](#bf_elf_header_t) | Unified ELF32/ELF64 file header view. |
| [`bf_elf_phdr_t`](#bf_elf_phdr_t) | Unified ELF32/ELF64 program header entry. |
| [`bf_elf_shdr_t`](#bf_elf_shdr_t) | Unified ELF32/ELF64 section header entry. |
| [`bf_elf_sym_t`](#bf_elf_sym_t) | Unified ELF32/ELF64 symbol table entry (`.symtab` or `.dynsym`). |
| [`bf_elf_dyn_t`](#bf_elf_dyn_t) | Unified ELF32/ELF64 dynamic entry (`.dynamic`). |
| [`bf_elf_rela_t`](#bf_elf_rela_t) | Unified ELF32/ELF64 relocation entry (`SHT_RELA` and `SHT_REL`). |
| [`bf_elf_slice_t`](#bf_elf_slice_t) | Zero-copy slice pointing directly into the backing [`bf_stream_t`](stream.md#bf_stream_t) buffer. |

## Macros

| Name | Description |
|------|-------------|
| [`BF_ELF_SHN_XINDEX`](#bf_elf_shn_xindex)  | Special `e_shstrndx` value indicating extended index in `shdr[0].sh_link`. |

---

### BF_ELF_SHN_XINDEX

```cpp
#define BF_ELF_SHN_XINDEX 0xFFFFU
```

Defined in include/bitforge/formats/elf.h:60

Special `e_shstrndx` value indicating extended index in `shdr[0].sh_link`.

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_elf_class_t`](#bf_elf_class_t)  | ELF address width class (`EI_CLASS`). |
| [`bf_elf_data_t`](#bf_elf_data_t)  | ELF byte order encoding (`EI_DATA`). |
| [`bf_elf_type_t`](#bf_elf_type_t)  | ELF object file type (`e_type`). |
| [`bf_elf_phdr_type_t`](#bf_elf_phdr_type_t)  | ELF segment type (`p_type` in program headers). |
| [`bf_elf_shdr_type_t`](#bf_elf_shdr_type_t)  | ELF section type (`sh_type` in section headers). |
| [`bf_elf_sym_bind_t`](#bf_elf_sym_bind_t)  | ELF symbol binding (`STB_*`). |
| [`bf_elf_sym_type_t`](#bf_elf_sym_type_t)  | ELF symbol type (`STT_*`). |
| [`bf_elf_dyn_tag_t`](#bf_elf_dyn_tag_t)  | Common ELF dynamic table tags (`d_tag` in `.dynamic`). |

---

### bf_elf_class_t

```cpp
enum bf_elf_class_t
```

Defined in include/bitforge/formats/elf.h:89

ELF address width class (`EI_CLASS`).

| Value | Description |
|-------|-------------|
| `BF_ELF_CLASS_NONE` | Invalid/unspecified ELF class. |
| `BF_ELF_CLASS_32` | 32-bit objects (`ELFCLASS32`). |
| `BF_ELF_CLASS_64` | 64-bit objects (`ELFCLASS64`). |

---

### bf_elf_data_t

```cpp
enum bf_elf_data_t
```

Defined in include/bitforge/formats/elf.h:98

ELF byte order encoding (`EI_DATA`).

| Value | Description |
|-------|-------------|
| `BF_ELF_DATA_NONE` | Invalid/unspecified endianness. |
| `BF_ELF_DATA_2LSB` | Two's complement little-endian (`ELFDATA2LSB`). |
| `BF_ELF_DATA_2MSB` | Two's complement big-endian (`ELFDATA2MSB`). |

---

### bf_elf_type_t

```cpp
enum bf_elf_type_t
```

Defined in include/bitforge/formats/elf.h:109

ELF object file type (`e_type`).

| Value | Description |
|-------|-------------|
| `BF_ELF_ET_NONE` | No file type (`ET_NONE`). |
| `BF_ELF_ET_REL` | Relocatable file (`ET_REL`). |
| `BF_ELF_ET_EXEC` | Executable file (`ET_EXEC`). |
| `BF_ELF_ET_DYN` | Shared object or PIE (`ET_DYN`). |
| `BF_ELF_ET_CORE` | Core file (`ET_CORE`). |

---

### bf_elf_phdr_type_t

```cpp
enum bf_elf_phdr_type_t
```

Defined in include/bitforge/formats/elf.h:120

ELF segment type (`p_type` in program headers).

| Value | Description |
|-------|-------------|
| `BF_ELF_PT_NULL` | Unused program header entry (`PT_NULL`). |
| `BF_ELF_PT_LOAD` | Loadable segment (`PT_LOAD`). |
| `BF_ELF_PT_DYNAMIC` | Dynamic linking information (`PT_DYNAMIC`). |
| `BF_ELF_PT_INTERP` | Program interpreter path (`PT_INTERP`). |
| `BF_ELF_PT_NOTE` | Auxiliary note information (`PT_NOTE`). |
| `BF_ELF_PT_PHDR` | Program header table itself (`PT_PHDR`). |

---

### bf_elf_shdr_type_t

```cpp
enum bf_elf_shdr_type_t
```

Defined in include/bitforge/formats/elf.h:133

ELF section type (`sh_type` in section headers).

| Value | Description |
|-------|-------------|
| `BF_ELF_SHT_NULL` | Inactive section header (`SHT_NULL`). |
| `BF_ELF_SHT_PROGBITS` | Program-defined data (`SHT_PROGBITS`). |
| `BF_ELF_SHT_SYMTAB` | Static symbol table (`SHT_SYMTAB`). |
| `BF_ELF_SHT_STRTAB` | String table (`SHT_STRTAB`). |
| `BF_ELF_SHT_RELA` | Relocation entries with explicit addends (`SHT_RELA`). |
| `BF_ELF_SHT_DYNAMIC` | Dynamic linking table (`SHT_DYNAMIC`). |
| `BF_ELF_SHT_NOBITS` | Uninitialized space occupying no file bytes (`SHT_NOBITS`). |
| `BF_ELF_SHT_REL` | Relocation entries without explicit addends (`SHT_REL`). |
| `BF_ELF_SHT_DYNSYM` | Dynamic symbol table (`SHT_DYNSYM`). |

---

### bf_elf_sym_bind_t

```cpp
enum bf_elf_sym_bind_t
```

Defined in include/bitforge/formats/elf.h:151

ELF symbol binding (`STB_*`).

| Value | Description |
|-------|-------------|
| `BF_ELF_STB_LOCAL` | Local symbol not visible outside defining object. |
| `BF_ELF_STB_GLOBAL` | Global symbol visible across combined objects. |
| `BF_ELF_STB_WEAK` | Weak symbol with lower precedence than global. |

---

### bf_elf_sym_type_t

```cpp
enum bf_elf_sym_type_t
```

Defined in include/bitforge/formats/elf.h:163

ELF symbol type (`STT_*`).

| Value | Description |
|-------|-------------|
| `BF_ELF_STT_NOTYPE` | Unspecified symbol type. |
| `BF_ELF_STT_OBJECT` | Data object (variable, array, etc.). |
| `BF_ELF_STT_FUNC` | Executable function or code entry. |
| `BF_ELF_STT_SECTION` | Section symbol. |
| `BF_ELF_STT_FILE` | Source file name symbol. |

---

### bf_elf_dyn_tag_t

```cpp
enum bf_elf_dyn_tag_t
```

Defined in include/bitforge/formats/elf.h:174

Common ELF dynamic table tags (`d_tag` in `.dynamic`).

| Value | Description |
|-------|-------------|
| `BF_ELF_DT_NULL` | Marks end of `_DYNAMIC` array (`DT_NULL`). |
| `BF_ELF_DT_NEEDED` | String table offset of needed library (`DT_NEEDED`). |
| `BF_ELF_DT_STRTAB` | Address of dynamic string table (`DT_STRTAB`). |
| `BF_ELF_DT_SYMTAB` | Address of dynamic symbol table (`DT_SYMTAB`). |
| `BF_ELF_DT_STRSZ` | Size in bytes of dynamic string table (`DT_STRSZ`). |
| `BF_ELF_DT_SYMENT` | Size in bytes of one dynamic symbol entry (`DT_SYMENT`). |
| `BF_ELF_DT_INIT` | Initialization function address (`DT_INIT`). |
| `BF_ELF_DT_FINI` | Termination function address (`DT_FINI`). |
| `BF_ELF_DT_SONAME` | String table offset of shared object name (`DT_SONAME`). |
| `BF_ELF_DT_RPATH` | Legacy library search path (`DT_RPATH`). |
| `BF_ELF_DT_RUNPATH` | Preferred library search path (`DT_RUNPATH`). |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_magic`](#bf_elf_read_magic)  | Verifies the 4-byte ELF magic sequence (`0x7F 'E' 'L' 'F'`). |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_header`](#bf_elf_read_header)  | Reads and validates an ELF32 or ELF64 file header. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_section_count`](#bf_elf_section_count)  | Resolves total section header count, supporting `PN_XNUM` (`shnum == 0`). |
| [`bf_status`](status.md#bf_status) | [`bf_elf_shstrndx`](#bf_elf_shstrndx)  | Resolves section header string table index, supporting `SHN_XINDEX`. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_phdr`](#bf_elf_read_phdr)  | Reads a single program header entry by index from `hdr->phoff`. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_shdr`](#bf_elf_read_shdr)  | Reads a single section header entry by index from `hdr->shoff`. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_get_section_name`](#bf_elf_get_section_name)  | Resolves a NUL-terminated section name from `.shstrtab`. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_sym`](#bf_elf_read_sym)  | Reads a symbol entry by index from a `SHT_SYMTAB` or `SHT_DYNSYM` section. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_get_sym_name`](#bf_elf_get_sym_name)  | Resolves a NUL-terminated symbol name from a string table slice. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_dyn`](#bf_elf_read_dyn)  | Reads a dynamic entry by index from an `SHT_DYNAMIC` section. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_read_rela`](#bf_elf_read_rela)  | Reads a relocation entry by index from an `SHT_RELA` or `SHT_REL` section. |
| [`bf_status`](status.md#bf_status) | [`bf_elf_get_slice`](#bf_elf_get_slice)  | Returns a zero-copy byte slice at `[file_offset, file_offset + size)`. |

---

### bf_elf_read_magic

```cpp
bf_status bf_elf_read_magic(bf_stream_t * stream)
```

Defined in include/bitforge/formats/elf.h:311

Verifies the 4-byte ELF magic sequence (`0x7F 'E' 'L' 'F'`).

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status) for invalid stream, [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain, or [`BF_EFORMAT`](status.md#bf_status) on mismatch.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream positioned at the ELF magic. |

---

### bf_elf_read_header

```cpp
bf_status bf_elf_read_header(bf_stream_t * stream, bf_elf_header_t * out_header)
```

Defined in include/bitforge/formats/elf.h:324

Reads and validates an ELF32 or ELF64 file header.

Advances [`Streams`](stream.md#streams) to `base_offset + hdr.ehsize` only on success; leaves [`Streams`](stream.md#streams) unchanged if validation, bounds checking, or integer decoding fails.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) for invalid arguments, [`BF_EOF`](status.md#bf_status) on truncated input, or [`BF_EFORMAT`](status.md#bf_status) for malformed header fields.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream positioned at ELF header. |
| `out_header` | [`bf_elf_header_t`](#bf_elf_header_t) * | Populated unified ELF header on success. |

---

### bf_elf_section_count

```cpp
bf_status bf_elf_section_count(const bf_stream_t * stream, const bf_elf_header_t * hdr, uint64_t * out_count)
```

Defined in include/bitforge/formats/elf.h:339

Resolves total section header count, supporting `PN_XNUM` (`shnum == 0`).

When `hdr->shnum == 0` and `hdr->shoff != 0`, reads `shdr[0].size` from the stream as the extended section count.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `out_count` | `uint64_t *` | Resolved number of section headers. |

---

### bf_elf_shstrndx

```cpp
bf_status bf_elf_shstrndx(const bf_stream_t * stream, const bf_elf_header_t * hdr, uint32_t * out_index)
```

Defined in include/bitforge/formats/elf.h:354

Resolves section header string table index, supporting `SHN_XINDEX`.

When `hdr->shstrndx == BF_ELF_SHN_XINDEX`, reads `shdr[0].link` as the extended section index.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `out_index` | `uint32_t *` | Resolved section header string table index. |

---

### bf_elf_read_phdr

```cpp
bf_status bf_elf_read_phdr(const bf_stream_t * stream, const bf_elf_header_t * hdr, uint16_t phnum_idx, bf_elf_phdr_t * out_phdr)
```

Defined in include/bitforge/formats/elf.h:369

Reads a single program header entry by index from `hdr->phoff`.

Does not mutate [`Streams`](stream.md#streams).

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `phnum_idx` | `uint16_t` | Zero-based program header index (`< hdr->phnum`). |
| `out_phdr` | [`bf_elf_phdr_t`](#bf_elf_phdr_t) * | Populated program header entry on success. |

---

### bf_elf_read_shdr

```cpp
bf_status bf_elf_read_shdr(const bf_stream_t * stream, const bf_elf_header_t * hdr, uint32_t shnum_idx, bf_elf_shdr_t * out_shdr)
```

Defined in include/bitforge/formats/elf.h:384

Reads a single section header entry by index from `hdr->shoff`.

Supports extended section counts (`PN_XNUM`) and does not mutate [`Streams`](stream.md#streams).

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `shnum_idx` | `uint32_t` | Zero-based section header index. |
| `out_shdr` | [`bf_elf_shdr_t`](#bf_elf_shdr_t) * | Populated section header entry on success. |

---

### bf_elf_get_section_name

```cpp
bf_status bf_elf_get_section_name(const bf_stream_t * stream, const bf_elf_header_t * hdr, const bf_elf_shdr_t * shdr, const char ** out_name)
```

Defined in include/bitforge/formats/elf.h:400

Resolves a NUL-terminated section name from `.shstrtab`.

:::note
Returned pointer points directly into `stream->data` and requires a terminating `'\0'` byte within the `.shstrtab` section slice.

:::

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `shdr` | const [`bf_elf_shdr_t`](#bf_elf_shdr_t) * | Section header whose `name` offset is resolved. |
| `out_name` | `const char **` | Pointer to NUL-terminated section name string. |

---

### bf_elf_read_sym

```cpp
bf_status bf_elf_read_sym(const bf_stream_t * stream, const bf_elf_header_t * hdr, const bf_elf_shdr_t * sym_shdr, uint32_t sym_idx, bf_elf_sym_t * out_sym)
```

Defined in include/bitforge/formats/elf.h:416

Reads a symbol entry by index from a `SHT_SYMTAB` or `SHT_DYNSYM` section.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `sym_shdr` | const [`bf_elf_shdr_t`](#bf_elf_shdr_t) * | Section header describing the symbol table. |
| `sym_idx` | `uint32_t` | Zero-based symbol entry index. |
| `out_sym` | [`bf_elf_sym_t`](#bf_elf_sym_t) * | Populated unified symbol entry on success. |

---

### bf_elf_get_sym_name

```cpp
bf_status bf_elf_get_sym_name(const bf_elf_slice_t * strtab, uint32_t name_offset, const char ** out_name)
```

Defined in include/bitforge/formats/elf.h:431

Resolves a NUL-terminated symbol name from a string table slice.

:::note
Returned pointer points directly into `strtab->data` and requires a terminating `'\0'` byte within `strtab->size`.

:::

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status) if unterminated.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `strtab` | const [`bf_elf_slice_t`](#bf_elf_slice_t) * | Zero-copy slice of `.strtab` or `.dynstr`. |
| `name_offset` | `uint32_t` | Byte offset within `strtab` ([`bf_elf_sym_t::name`](#name-2)). |
| `out_name` | `const char **` | Pointer to NUL-terminated symbol name string. |

---

### bf_elf_read_dyn

```cpp
bf_status bf_elf_read_dyn(const bf_stream_t * stream, const bf_elf_header_t * hdr, const bf_elf_shdr_t * dyn_shdr, uint32_t dyn_idx, bf_elf_dyn_t * out_dyn)
```

Defined in include/bitforge/formats/elf.h:445

Reads a dynamic entry by index from an `SHT_DYNAMIC` section.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `dyn_shdr` | const [`bf_elf_shdr_t`](#bf_elf_shdr_t) * | Section header describing `.dynamic`. |
| `dyn_idx` | `uint32_t` | Zero-based dynamic entry index. |
| `out_dyn` | [`bf_elf_dyn_t`](#bf_elf_dyn_t) * | Populated dynamic entry on success. |

---

### bf_elf_read_rela

```cpp
bf_status bf_elf_read_rela(const bf_stream_t * stream, const bf_elf_header_t * hdr, const bf_elf_shdr_t * rel_shdr, uint32_t rel_idx, bf_elf_rela_t * out_rela)
```

Defined in include/bitforge/formats/elf.h:462

Reads a relocation entry by index from an `SHT_RELA` or `SHT_REL` section.

For `SHT_REL` sections, `out_rela->addend` is set to `0`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream containing the ELF image. |
| `hdr` | const [`bf_elf_header_t`](#bf_elf_header_t) * | Parsed ELF file header. |
| `rel_shdr` | const [`bf_elf_shdr_t`](#bf_elf_shdr_t) * | Section header describing the relocation table. |
| `rel_idx` | `uint32_t` | Zero-based relocation entry index. |
| `out_rela` | [`bf_elf_rela_t`](#bf_elf_rela_t) * | Populated unified relocation entry on success. |

---

### bf_elf_get_slice

```cpp
bf_status bf_elf_get_slice(const bf_stream_t * stream, uint64_t file_offset, uint64_t size, bf_elf_slice_t * out_slice)
```

Defined in include/bitforge/formats/elf.h:475

Returns a zero-copy byte slice at `[file_offset, file_offset + size)`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EOF`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream backing the slice. |
| `file_offset` | `uint64_t` | Starting byte offset within [`Streams`](stream.md#streams). |
| `size` | `uint64_t` | Requested slice length in bytes. |
| `out_slice` | [`bf_elf_slice_t`](#bf_elf_slice_t) * | Populated zero-copy slice on success. |


## Class Definitions



### bf_elf_header_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_header_t
```

Defined in include/bitforge/formats/elf.h:199

Unified ELF32/ELF64 file header view.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t` | [`elf_class`](#elf_class)  | Address class ([`bf_elf_class_t`](#bf_elf_class_t)). |
| `uint8_t` | [`data_encoding`](#data_encoding)  | Byte order ([`bf_elf_data_t`](#bf_elf_data_t)). |
| `uint8_t` | [`ident_version`](#ident_version)  | Identification version (`EI_VERSION`, must be 1). |
| `uint8_t` | [`os_abi`](#os_abi)  | Target OS/ABI identifier (`EI_OSABI`). |
| `uint16_t` | [`type`](#type)  | Object file type ([`bf_elf_type_t`](#bf_elf_type_t)). |
| `uint16_t` | [`machine`](#machine-1)  | Target architecture (`EM_*`). |
| `uint32_t` | [`version`](#version)  | Object file version (`EV_CURRENT`, must be 1). |
| `uint64_t` | [`entry`](#entry)  | Entry point virtual address. |
| `uint64_t` | [`phoff`](#phoff)  | Program header table file offset. |
| `uint64_t` | [`shoff`](#shoff)  | Section header table file offset. |
| `uint32_t` | [`flags`](#flags)  | Processor-specific flags (`EF_*`). |
| `uint16_t` | [`ehsize`](#ehsize)  | ELF header size in bytes. |
| `uint16_t` | [`phentsize`](#phentsize)  | Program header table entry size in bytes. |
| `uint16_t` | [`phnum`](#phnum)  | Number of program header entries. |
| `uint16_t` | [`shentsize`](#shentsize)  | Section header table entry size in bytes. |
| `uint16_t` | [`shnum`](#shnum)  | Number of section header entries (`0` if `PN_XNUM`). |
| `uint16_t` | [`shstrndx`](#shstrndx)  | Section header string table index (or [`BF_ELF_SHN_XINDEX`](#bf_elf_shn_xindex)). |

---

##### elf_class

```cpp
uint8_t elf_class
```

Defined in include/bitforge/formats/elf.h:200

Address class ([`bf_elf_class_t`](#bf_elf_class_t)).

---

##### data_encoding

```cpp
uint8_t data_encoding
```

Defined in include/bitforge/formats/elf.h:201

Byte order ([`bf_elf_data_t`](#bf_elf_data_t)).

---

##### ident_version

```cpp
uint8_t ident_version
```

Defined in include/bitforge/formats/elf.h:202

Identification version (`EI_VERSION`, must be 1).

---

##### os_abi

```cpp
uint8_t os_abi
```

Defined in include/bitforge/formats/elf.h:204

Target OS/ABI identifier (`EI_OSABI`).

---

##### type

```cpp
uint16_t type
```

Defined in include/bitforge/formats/elf.h:205

Object file type ([`bf_elf_type_t`](#bf_elf_type_t)).

---

##### machine

```cpp
uint16_t machine
```

Defined in include/bitforge/formats/elf.h:206

Target architecture (`EM_*`).

---

##### version

```cpp
uint32_t version
```

Defined in include/bitforge/formats/elf.h:207

Object file version (`EV_CURRENT`, must be 1).

---

##### entry

```cpp
uint64_t entry
```

Defined in include/bitforge/formats/elf.h:208

Entry point virtual address.

---

##### phoff

```cpp
uint64_t phoff
```

Defined in include/bitforge/formats/elf.h:209

Program header table file offset.

---

##### shoff

```cpp
uint64_t shoff
```

Defined in include/bitforge/formats/elf.h:210

Section header table file offset.

---

##### flags

```cpp
uint32_t flags
```

Defined in include/bitforge/formats/elf.h:211

Processor-specific flags (`EF_*`).

---

##### ehsize

```cpp
uint16_t ehsize
```

Defined in include/bitforge/formats/elf.h:212

ELF header size in bytes.

---

##### phentsize

```cpp
uint16_t phentsize
```

Defined in include/bitforge/formats/elf.h:213

Program header table entry size in bytes.

---

##### phnum

```cpp
uint16_t phnum
```

Defined in include/bitforge/formats/elf.h:214

Number of program header entries.

---

##### shentsize

```cpp
uint16_t shentsize
```

Defined in include/bitforge/formats/elf.h:215

Section header table entry size in bytes.

---

##### shnum

```cpp
uint16_t shnum
```

Defined in include/bitforge/formats/elf.h:217

Number of section header entries (`0` if `PN_XNUM`).

---

##### shstrndx

```cpp
uint16_t shstrndx
```

Defined in include/bitforge/formats/elf.h:218

Section header string table index (or [`BF_ELF_SHN_XINDEX`](#bf_elf_shn_xindex)).



### bf_elf_phdr_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_phdr_t
```

Defined in include/bitforge/formats/elf.h:225

Unified ELF32/ELF64 program header entry.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`type`](#type-1)  | Segment type ([`bf_elf_phdr_type_t`](#bf_elf_phdr_type_t)). |
| `uint32_t` | [`flags`](#flags-1)  | Segment access flags (`PF_R`, `PF_W`, `PF_X`). |
| `uint64_t` | [`offset`](#offset)  | File offset of the segment payload. |
| `uint64_t` | [`vaddr`](#vaddr)  | Virtual address in memory image. |
| `uint64_t` | [`paddr`](#paddr)  | Physical address where applicable. |
| `uint64_t` | [`filesz`](#filesz)  | Segment size in file in bytes. |
| `uint64_t` | [`memsz`](#memsz)  | Segment size in memory in bytes. |
| `uint64_t` | [`align`](#align)  | Segment alignment in file and memory. |

---

##### type

```cpp
uint32_t type
```

Defined in include/bitforge/formats/elf.h:226

Segment type ([`bf_elf_phdr_type_t`](#bf_elf_phdr_type_t)).

---

##### flags

```cpp
uint32_t flags
```

Defined in include/bitforge/formats/elf.h:227

Segment access flags (`PF_R`, `PF_W`, `PF_X`).

---

##### offset

```cpp
uint64_t offset
```

Defined in include/bitforge/formats/elf.h:228

File offset of the segment payload.

---

##### vaddr

```cpp
uint64_t vaddr
```

Defined in include/bitforge/formats/elf.h:229

Virtual address in memory image.

---

##### paddr

```cpp
uint64_t paddr
```

Defined in include/bitforge/formats/elf.h:230

Physical address where applicable.

---

##### filesz

```cpp
uint64_t filesz
```

Defined in include/bitforge/formats/elf.h:231

Segment size in file in bytes.

---

##### memsz

```cpp
uint64_t memsz
```

Defined in include/bitforge/formats/elf.h:232

Segment size in memory in bytes.

---

##### align

```cpp
uint64_t align
```

Defined in include/bitforge/formats/elf.h:233

Segment alignment in file and memory.



### bf_elf_shdr_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_shdr_t
```

Defined in include/bitforge/formats/elf.h:239

Unified ELF32/ELF64 section header entry.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`name`](#name-1)  | Byte offset into `.shstrtab` for the section name. |
| `uint32_t` | [`type`](#type-2)  | Section type ([`bf_elf_shdr_type_t`](#bf_elf_shdr_type_t)). |
| `uint64_t` | [`flags`](#flags-2)  | Section attribute flags (`SHF_*`). |
| `uint64_t` | [`addr`](#addr)  | Virtual address when mapped into memory. |
| `uint64_t` | [`offset`](#offset-1)  | File offset of section data. |
| `uint64_t` | [`size`](#size-6)  | Section size in bytes. |
| `uint32_t` | [`link`](#link)  | Section header table index link (interpretation depends on [`type`](#type-2)). |
| `uint32_t` | [`info`](#info)  | Extra section information (interpretation depends on [`type`](#type-2)). |
| `uint64_t` | [`addralign`](#addralign)  | Required section alignment (`0` or `1` means unaligned). |
| `uint64_t` | [`entsize`](#entsize)  | Fixed entry size for tables, or `0` if none. |

---

##### name

```cpp
uint32_t name
```

Defined in include/bitforge/formats/elf.h:241

Byte offset into `.shstrtab` for the section name.

---

##### type

```cpp
uint32_t type
```

Defined in include/bitforge/formats/elf.h:242

Section type ([`bf_elf_shdr_type_t`](#bf_elf_shdr_type_t)).

---

##### flags

```cpp
uint64_t flags
```

Defined in include/bitforge/formats/elf.h:243

Section attribute flags (`SHF_*`).

---

##### addr

```cpp
uint64_t addr
```

Defined in include/bitforge/formats/elf.h:244

Virtual address when mapped into memory.

---

##### offset

```cpp
uint64_t offset
```

Defined in include/bitforge/formats/elf.h:245

File offset of section data.

---

##### size

```cpp
uint64_t size
```

Defined in include/bitforge/formats/elf.h:246

Section size in bytes.

---

##### link

```cpp
uint32_t link
```

Defined in include/bitforge/formats/elf.h:247

Section header table index link (interpretation depends on [`type`](#type-2)).

---

##### info

```cpp
uint32_t info
```

Defined in include/bitforge/formats/elf.h:249

Extra section information (interpretation depends on [`type`](#type-2)).

---

##### addralign

```cpp
uint64_t addralign
```

Defined in include/bitforge/formats/elf.h:251

Required section alignment (`0` or `1` means unaligned).

---

##### entsize

```cpp
uint64_t entsize
```

Defined in include/bitforge/formats/elf.h:253

Fixed entry size for tables, or `0` if none.



### bf_elf_sym_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_sym_t
```

Defined in include/bitforge/formats/elf.h:259

Unified ELF32/ELF64 symbol table entry (`.symtab` or `.dynsym`).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`name`](#name-2)  | Byte offset into associated string table (`.strtab` / `.dynstr`). |
| `uint8_t` | [`info`](#info-1)  | Symbol binding and type (`BF_ELF_ST_BIND` / `BF_ELF_ST_TYPE`). |
| `uint8_t` | [`other`](#other)  | Symbol visibility (`STV_*`). |
| `uint16_t` | [`shndx`](#shndx)  | Associated section header index or special `SHN_*` index. |
| `uint64_t` | [`value`](#value)  | Symbol value, address, or section offset. |
| `uint64_t` | [`size`](#size-7)  | Associated object or function size in bytes. |

---

##### name

```cpp
uint32_t name
```

Defined in include/bitforge/formats/elf.h:260

Byte offset into associated string table (`.strtab` / `.dynstr`).

---

##### info

```cpp
uint8_t info
```

Defined in include/bitforge/formats/elf.h:262

Symbol binding and type (`BF_ELF_ST_BIND` / `BF_ELF_ST_TYPE`).

---

##### other

```cpp
uint8_t other
```

Defined in include/bitforge/formats/elf.h:264

Symbol visibility (`STV_*`).

---

##### shndx

```cpp
uint16_t shndx
```

Defined in include/bitforge/formats/elf.h:265

Associated section header index or special `SHN_*` index.

---

##### value

```cpp
uint64_t value
```

Defined in include/bitforge/formats/elf.h:267

Symbol value, address, or section offset.

---

##### size

```cpp
uint64_t size
```

Defined in include/bitforge/formats/elf.h:268

Associated object or function size in bytes.



### bf_elf_dyn_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_dyn_t
```

Defined in include/bitforge/formats/elf.h:274

Unified ELF32/ELF64 dynamic entry (`.dynamic`).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int64_t` | [`tag`](#tag)  | Dynamic entry tag (`DT_*`). |
| `uint64_t` | [`val`](#val)  | Integer value or virtual address/offset depending on [`tag`](#tag). |

---

##### tag

```cpp
int64_t tag
```

Defined in include/bitforge/formats/elf.h:275

Dynamic entry tag (`DT_*`).

---

##### val

```cpp
uint64_t val
```

Defined in include/bitforge/formats/elf.h:276

Integer value or virtual address/offset depending on [`tag`](#tag).



### bf_elf_rela_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_rela_t
```

Defined in include/bitforge/formats/elf.h:283

Unified ELF32/ELF64 relocation entry (`SHT_RELA` and `SHT_REL`).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`offset`](#offset-2)  | Target file offset (relocatable) or virtual address (executable/DSO). |
| `uint64_t` | [`info`](#info-2)  | Encoded symbol index and relocation type. |
| `int64_t` | [`addend`](#addend)  | Signed addend for `SHT_RELA` entries; `0` for `SHT_REL`. |

---

##### offset

```cpp
uint64_t offset
```

Defined in include/bitforge/formats/elf.h:284

Target file offset (relocatable) or virtual address (executable/DSO).

---

##### info

```cpp
uint64_t info
```

Defined in include/bitforge/formats/elf.h:286

Encoded symbol index and relocation type.

---

##### addend

```cpp
int64_t addend
```

Defined in include/bitforge/formats/elf.h:287

Signed addend for `SHT_RELA` entries; `0` for `SHT_REL`.



### bf_elf_slice_t

```cpp
#include <elf.h>
```

```cpp
struct bf_elf_slice_t
```

Defined in include/bitforge/formats/elf.h:298

Zero-copy slice pointing directly into the backing [`bf_stream_t`](stream.md#bf_stream_t) buffer.

:::warning
The [`data`](#data-6) pointer remains valid only while the underlying stream buffer stays alive and unmodified. 

:::

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data-6)  | Pointer to slice bytes, or `NULL` when `size == 0`. |
| `size_t` | [`size`](#size-8)  | Slice length in bytes. |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/elf.h:300

Pointer to slice bytes, or `NULL` when `size == 0`.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/formats/elf.h:301

Slice length in bytes.


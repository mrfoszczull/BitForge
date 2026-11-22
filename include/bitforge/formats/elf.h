#ifndef BITFORGE_FORMATS_ELF_H
#define BITFORGE_FORMATS_ELF_H

#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup formats_elf ELF Format Parser
 * @brief Zero-allocation C17 parser for ELF32 and ELF64 binaries (LE and BE).
 *
 * Provides header inspection, extended section count/string-table index
 * resolution (`PN_XNUM` / `SHN_XINDEX`), program and section headers, section
 * name lookup via `.shstrtab`, symbol tables (`.symtab` / `.dynsym`),
 * `.dynamic` entries, relocation entries (`SHT_RELA` / `SHT_REL`), and
 * zero-copy slice views over an aligned `bf_stream_t`.
 * @{
 */

/** @name Header and Entry Sizes
 * @{
 */
#define BF_ELF_IDENT_LEN                                                       \
	16U /**< Size of ELF identification array `e_ident` in bytes. */
#define BF_ELF32_EHDR_LEN 52U /**< Minimum ELF32 file header size in bytes. */
#define BF_ELF64_EHDR_LEN 64U /**< Minimum ELF64 file header size in bytes. */
#define BF_ELF32_PHDR_LEN                                                      \
	32U /**< Minimum ELF32 program header entry size in bytes. */
#define BF_ELF64_PHDR_LEN                                                      \
	56U /**< Minimum ELF64 program header entry size in bytes. */
#define BF_ELF32_SHDR_LEN                                                      \
	40U /**< Minimum ELF32 section header entry size in bytes. */
#define BF_ELF64_SHDR_LEN                                                      \
	64U /**< Minimum ELF64 section header entry size in bytes. */
#define BF_ELF32_SYM_LEN 16U /**< Minimum ELF32 symbol entry size in bytes. */
#define BF_ELF64_SYM_LEN 24U /**< Minimum ELF64 symbol entry size in bytes. */
#define BF_ELF32_DYN_LEN 8U  /**< Minimum ELF32 dynamic entry size in bytes. */
#define BF_ELF64_DYN_LEN 16U /**< Minimum ELF64 dynamic entry size in bytes.   \
			      */
#define BF_ELF32_REL_LEN                                                       \
	8U /**< Minimum ELF32 `REL` relocation entry size in bytes. */
#define BF_ELF32_RELA_LEN                                                      \
	12U /**< Minimum ELF32 `RELA` relocation entry size in bytes. */
#define BF_ELF64_REL_LEN                                                       \
	16U /**< Minimum ELF64 `REL` relocation entry size in bytes. */
#define BF_ELF64_RELA_LEN                                                      \
	24U /**< Minimum ELF64 `RELA` relocation entry size in bytes. */
/** @} */

/** @brief Special `e_shstrndx` value indicating extended index in
 * `shdr[0].sh_link`. */
#define BF_ELF_SHN_XINDEX 0xFFFFU

/** @name Symbol and Relocation Bitfield Helpers
 * @{
 */
/** @brief Extracts symbol binding (`STB_*`) from `bf_elf_sym_t::info`. */
#define BF_ELF_ST_BIND(info) ((uint8_t)((uint8_t)(info) >> 4))

/** @brief Extracts symbol type (`STT_*`) from `bf_elf_sym_t::info`. */
#define BF_ELF_ST_TYPE(info) ((uint8_t)((uint8_t)(info) & 0x0FU))

/** @brief Extracts 32-bit relocation symbol index from `bf_elf_rela_t::info`.
 */
#define BF_ELF32_R_SYM(info) ((uint32_t)((uint32_t)(info) >> 8))

/** @brief Extracts 32-bit relocation type from `bf_elf_rela_t::info`. */
#define BF_ELF32_R_TYPE(info) ((uint8_t)((uint32_t)(info) & 0xFFU))

/** @brief Extracts 64-bit relocation symbol index from `bf_elf_rela_t::info`.
 */
#define BF_ELF64_R_SYM(info) ((uint32_t)((uint64_t)(info) >> 32))

/** @brief Extracts 64-bit relocation type from `bf_elf_rela_t::info`. */
#define BF_ELF64_R_TYPE(info) ((uint32_t)((uint64_t)(info) & 0xFFFFFFFFULL))
/** @} */

/**
 * @brief ELF address width class (`EI_CLASS`).
 */
typedef enum {
	BF_ELF_CLASS_NONE = 0U, /**< Invalid/unspecified ELF class. */
	BF_ELF_CLASS_32 = 1U,	/**< 32-bit objects (`ELFCLASS32`). */
	BF_ELF_CLASS_64 = 2U	/**< 64-bit objects (`ELFCLASS64`). */
} bf_elf_class_t;

/**
 * @brief ELF byte order encoding (`EI_DATA`).
 */
typedef enum {
	BF_ELF_DATA_NONE = 0U, /**< Invalid/unspecified endianness. */
	BF_ELF_DATA_2LSB =
	    1U, /**< Two's complement little-endian (`ELFDATA2LSB`). */
	BF_ELF_DATA_2MSB =
	    2U /**< Two's complement big-endian (`ELFDATA2MSB`). */
} bf_elf_data_t;

/**
 * @brief ELF object file type (`e_type`).
 */
typedef enum {
	BF_ELF_ET_NONE = 0U, /**< No file type (`ET_NONE`). */
	BF_ELF_ET_REL = 1U,  /**< Relocatable file (`ET_REL`). */
	BF_ELF_ET_EXEC = 2U, /**< Executable file (`ET_EXEC`). */
	BF_ELF_ET_DYN = 3U,  /**< Shared object or PIE (`ET_DYN`). */
	BF_ELF_ET_CORE = 4U  /**< Core file (`ET_CORE`). */
} bf_elf_type_t;

/**
 * @brief ELF segment type (`p_type` in program headers).
 */
typedef enum {
	BF_ELF_PT_NULL = 0U, /**< Unused program header entry (`PT_NULL`). */
	BF_ELF_PT_LOAD = 1U, /**< Loadable segment (`PT_LOAD`). */
	BF_ELF_PT_DYNAMIC =
	    2U, /**< Dynamic linking information (`PT_DYNAMIC`). */
	BF_ELF_PT_INTERP = 3U, /**< Program interpreter path (`PT_INTERP`). */
	BF_ELF_PT_NOTE = 4U,   /**< Auxiliary note information (`PT_NOTE`). */
	BF_ELF_PT_PHDR = 6U    /**< Program header table itself (`PT_PHDR`). */
} bf_elf_phdr_type_t;

/**
 * @brief ELF section type (`sh_type` in section headers).
 */
typedef enum {
	BF_ELF_SHT_NULL = 0U,	  /**< Inactive section header (`SHT_NULL`). */
	BF_ELF_SHT_PROGBITS = 1U, /**< Program-defined data (`SHT_PROGBITS`). */
	BF_ELF_SHT_SYMTAB = 2U,	  /**< Static symbol table (`SHT_SYMTAB`). */
	BF_ELF_SHT_STRTAB = 3U,	  /**< String table (`SHT_STRTAB`). */
	BF_ELF_SHT_RELA =
	    4U, /**< Relocation entries with explicit addends (`SHT_RELA`). */
	BF_ELF_SHT_DYNAMIC = 6U, /**< Dynamic linking table (`SHT_DYNAMIC`). */
	BF_ELF_SHT_NOBITS = 8U, /**< Uninitialized space occupying no file bytes
				   (`SHT_NOBITS`). */
	BF_ELF_SHT_REL =
	    9U, /**< Relocation entries without explicit addends (`SHT_REL`). */
	BF_ELF_SHT_DYNSYM = 11U /**< Dynamic symbol table (`SHT_DYNSYM`). */
} bf_elf_shdr_type_t;

/**
 * @brief ELF symbol binding (`STB_*`).
 */
typedef enum {
	BF_ELF_STB_LOCAL =
	    0U, /**< Local symbol not visible outside defining object. */
	BF_ELF_STB_GLOBAL =
	    1U, /**< Global symbol visible across combined objects. */
	BF_ELF_STB_WEAK =
	    2U /**< Weak symbol with lower precedence than global. */
} bf_elf_sym_bind_t;

/**
 * @brief ELF symbol type (`STT_*`).
 */
typedef enum {
	BF_ELF_STT_NOTYPE = 0U,	 /**< Unspecified symbol type. */
	BF_ELF_STT_OBJECT = 1U,	 /**< Data object (variable, array, etc.). */
	BF_ELF_STT_FUNC = 2U,	 /**< Executable function or code entry. */
	BF_ELF_STT_SECTION = 3U, /**< Section symbol. */
	BF_ELF_STT_FILE = 4U	 /**< Source file name symbol. */
} bf_elf_sym_type_t;

/**
 * @brief Common ELF dynamic table tags (`d_tag` in `.dynamic`).
 */
typedef enum {
	BF_ELF_DT_NULL = 0, /**< Marks end of `_DYNAMIC` array (`DT_NULL`). */
	BF_ELF_DT_NEEDED =
	    1, /**< String table offset of needed library (`DT_NEEDED`). */
	BF_ELF_DT_STRTAB =
	    5, /**< Address of dynamic string table (`DT_STRTAB`). */
	BF_ELF_DT_SYMTAB =
	    6, /**< Address of dynamic symbol table (`DT_SYMTAB`). */
	BF_ELF_DT_STRSZ =
	    10, /**< Size in bytes of dynamic string table (`DT_STRSZ`). */
	BF_ELF_DT_SYMENT =
	    11, /**< Size in bytes of one dynamic symbol entry (`DT_SYMENT`). */
	BF_ELF_DT_INIT =
	    12, /**< Initialization function address (`DT_INIT`). */
	BF_ELF_DT_FINI = 13, /**< Termination function address (`DT_FINI`). */
	BF_ELF_DT_SONAME =
	    14, /**< String table offset of shared object name (`DT_SONAME`). */
	BF_ELF_DT_RPATH = 15, /**< Legacy library search path (`DT_RPATH`). */
	BF_ELF_DT_RUNPATH =
	    29 /**< Preferred library search path (`DT_RUNPATH`). */
} bf_elf_dyn_tag_t;

/**
 * @brief Unified ELF32/ELF64 file header view.
 */
typedef struct {
	uint8_t elf_class;     /**< Address class (`bf_elf_class_t`). */
	uint8_t data_encoding; /**< Byte order (`bf_elf_data_t`). */
	uint8_t ident_version; /**< Identification version (`EI_VERSION`, must
				  be 1). */
	uint8_t os_abi;	       /**< Target OS/ABI identifier (`EI_OSABI`). */
	uint16_t type;	       /**< Object file type (`bf_elf_type_t`). */
	uint16_t machine;      /**< Target architecture (`EM_*`). */
	uint32_t version; /**< Object file version (`EV_CURRENT`, must be 1). */
	uint64_t entry;	  /**< Entry point virtual address. */
	uint64_t phoff;	  /**< Program header table file offset. */
	uint64_t shoff;	  /**< Section header table file offset. */
	uint32_t flags;	  /**< Processor-specific flags (`EF_*`). */
	uint16_t ehsize;  /**< ELF header size in bytes. */
	uint16_t phentsize; /**< Program header table entry size in bytes. */
	uint16_t phnum;	    /**< Number of program header entries. */
	uint16_t shentsize; /**< Section header table entry size in bytes. */
	uint16_t
	    shnum; /**< Number of section header entries (`0` if `PN_XNUM`). */
	uint16_t shstrndx; /**< Section header string table index (or
			      `BF_ELF_SHN_XINDEX`). */
} bf_elf_header_t;

/**
 * @brief Unified ELF32/ELF64 program header entry.
 */
typedef struct {
	uint32_t type;	 /**< Segment type (`bf_elf_phdr_type_t`). */
	uint32_t flags;	 /**< Segment access flags (`PF_R`, `PF_W`, `PF_X`). */
	uint64_t offset; /**< File offset of the segment payload. */
	uint64_t vaddr;	 /**< Virtual address in memory image. */
	uint64_t paddr;	 /**< Physical address where applicable. */
	uint64_t filesz; /**< Segment size in file in bytes. */
	uint64_t memsz;	 /**< Segment size in memory in bytes. */
	uint64_t align;	 /**< Segment alignment in file and memory. */
} bf_elf_phdr_t;

/**
 * @brief Unified ELF32/ELF64 section header entry.
 */
typedef struct {
	uint32_t
	    name; /**< Byte offset into `.shstrtab` for the section name. */
	uint32_t type;	 /**< Section type (`bf_elf_shdr_type_t`). */
	uint64_t flags;	 /**< Section attribute flags (`SHF_*`). */
	uint64_t addr;	 /**< Virtual address when mapped into memory. */
	uint64_t offset; /**< File offset of section data. */
	uint64_t size;	 /**< Section size in bytes. */
	uint32_t link;	 /**< Section header table index link (interpretation
			    depends on `type`). */
	uint32_t info; /**< Extra section information (interpretation depends on
			  `type`). */
	uint64_t addralign; /**< Required section alignment (`0` or `1` means
			       unaligned). */
	uint64_t entsize;   /**< Fixed entry size for tables, or `0` if none. */
} bf_elf_shdr_t;

/**
 * @brief Unified ELF32/ELF64 symbol table entry (`.symtab` or `.dynsym`).
 */
typedef struct {
	uint32_t name;	/**< Byte offset into associated string table (`.strtab`
			   / `.dynstr`). */
	uint8_t info;	/**< Symbol binding and type (`BF_ELF_ST_BIND` /
			   `BF_ELF_ST_TYPE`). */
	uint8_t other;	/**< Symbol visibility (`STV_*`). */
	uint16_t shndx; /**< Associated section header index or special `SHN_*`
			   index. */
	uint64_t value; /**< Symbol value, address, or section offset. */
	uint64_t size;	/**< Associated object or function size in bytes. */
} bf_elf_sym_t;

/**
 * @brief Unified ELF32/ELF64 dynamic entry (`.dynamic`).
 */
typedef struct {
	int64_t tag;  /**< Dynamic entry tag (`DT_*`). */
	uint64_t val; /**< Integer value or virtual address/offset depending on
			 `tag`. */
} bf_elf_dyn_t;

/**
 * @brief Unified ELF32/ELF64 relocation entry (`SHT_RELA` and `SHT_REL`).
 */
typedef struct {
	uint64_t offset; /**< Target file offset (relocatable) or virtual
			    address (executable/DSO). */
	uint64_t info;	 /**< Encoded symbol index and relocation type. */
	int64_t addend;	 /**< Signed addend for `SHT_RELA` entries; `0` for
			    `SHT_REL`. */
} bf_elf_rela_t;

/**
 * @brief Zero-copy slice pointing directly into the backing `bf_stream_t`
 * buffer.
 *
 * @warning The `data` pointer remains valid only while the underlying stream
 * buffer stays alive and unmodified.
 */
typedef struct {
	const uint8_t
	    *data;   /**< Pointer to slice bytes, or `NULL` when `size == 0`. */
	size_t size; /**< Slice length in bytes. */
} bf_elf_slice_t;

/**
 * @brief Verifies the 4-byte ELF magic sequence (`0x7F 'E' 'L' 'F'`).
 *
 * @param[in,out] stream Byte-aligned input stream positioned at the ELF magic.
 * @return `BF_OK` on match, `BF_EINVAL` for invalid stream, `BF_EOF` if fewer
 *         than 4 bytes remain, or `BF_EFORMAT` on mismatch.
 */
bf_status bf_elf_read_magic(bf_stream_t *stream);

/**
 * @brief Reads and validates an ELF32 or ELF64 file header.
 *
 * Advances `stream` to `base_offset + hdr.ehsize` only on success; leaves
 * `stream` unchanged if validation, bounds checking, or integer decoding fails.
 *
 * @param[in,out] stream     Byte-aligned input stream positioned at ELF header.
 * @param[out]    out_header Populated unified ELF header on success.
 * @return `BF_OK` on success, `BF_EINVAL` for invalid arguments, `BF_EOF` on
 *         truncated input, or `BF_EFORMAT` for malformed header fields.
 */
bf_status bf_elf_read_header(bf_stream_t *stream, bf_elf_header_t *out_header);

/**
 * @brief Resolves total section header count, supporting `PN_XNUM` (`shnum ==
 * 0`).
 *
 * When `hdr->shnum == 0` and `hdr->shoff != 0`, reads `shdr[0].size` from the
 * stream as the extended section count.
 *
 * @param[in]  stream    Byte-aligned stream containing the ELF image.
 * @param[in]  hdr       Parsed ELF file header.
 * @param[out] out_count Resolved number of section headers.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_section_count(
    const bf_stream_t *stream, const bf_elf_header_t *hdr, uint64_t *out_count);

/**
 * @brief Resolves section header string table index, supporting `SHN_XINDEX`.
 *
 * When `hdr->shstrndx == BF_ELF_SHN_XINDEX`, reads `shdr[0].link` as the
 * extended section index.
 *
 * @param[in]  stream    Byte-aligned stream containing the ELF image.
 * @param[in]  hdr       Parsed ELF file header.
 * @param[out] out_index Resolved section header string table index.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_shstrndx(
    const bf_stream_t *stream, const bf_elf_header_t *hdr, uint32_t *out_index);

/**
 * @brief Reads a single program header entry by index from `hdr->phoff`.
 *
 * Does not mutate `stream`.
 *
 * @param[in]  stream    Byte-aligned stream containing the ELF image.
 * @param[in]  hdr       Parsed ELF file header.
 * @param[in]  phnum_idx Zero-based program header index (`< hdr->phnum`).
 * @param[out] out_phdr  Populated program header entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_read_phdr(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, uint16_t phnum_idx, bf_elf_phdr_t *out_phdr);

/**
 * @brief Reads a single section header entry by index from `hdr->shoff`.
 *
 * Supports extended section counts (`PN_XNUM`) and does not mutate `stream`.
 *
 * @param[in]  stream    Byte-aligned stream containing the ELF image.
 * @param[in]  hdr       Parsed ELF file header.
 * @param[in]  shnum_idx Zero-based section header index.
 * @param[out] out_shdr  Populated section header entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_read_shdr(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, uint32_t shnum_idx, bf_elf_shdr_t *out_shdr);

/**
 * @brief Resolves a NUL-terminated section name from `.shstrtab`.
 *
 * @note Returned pointer points directly into `stream->data` and requires a
 *       terminating `'\0'` byte within the `.shstrtab` section slice.
 *
 * @param[in]  stream   Byte-aligned stream containing the ELF image.
 * @param[in]  hdr      Parsed ELF file header.
 * @param[in]  shdr     Section header whose `name` offset is resolved.
 * @param[out] out_name Pointer to NUL-terminated section name string.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_get_section_name(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, const bf_elf_shdr_t *shdr,
    const char **out_name);

/**
 * @brief Reads a symbol entry by index from a `SHT_SYMTAB` or `SHT_DYNSYM`
 * section.
 *
 * @param[in]  stream   Byte-aligned stream containing the ELF image.
 * @param[in]  hdr      Parsed ELF file header.
 * @param[in]  sym_shdr Section header describing the symbol table.
 * @param[in]  sym_idx  Zero-based symbol entry index.
 * @param[out] out_sym  Populated unified symbol entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_read_sym(const bf_stream_t *stream, const bf_elf_header_t *hdr,
    const bf_elf_shdr_t *sym_shdr, uint32_t sym_idx, bf_elf_sym_t *out_sym);

/**
 * @brief Resolves a NUL-terminated symbol name from a string table slice.
 *
 * @note Returned pointer points directly into `strtab->data` and requires a
 *       terminating `'\0'` byte within `strtab->size`.
 *
 * @param[in]  strtab      Zero-copy slice of `.strtab` or `.dynstr`.
 * @param[in]  name_offset Byte offset within `strtab` (`bf_elf_sym_t::name`).
 * @param[out] out_name    Pointer to NUL-terminated symbol name string.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT` if
 * unterminated.
 */
bf_status bf_elf_get_sym_name(
    const bf_elf_slice_t *strtab, uint32_t name_offset, const char **out_name);

/**
 * @brief Reads a dynamic entry by index from an `SHT_DYNAMIC` section.
 *
 * @param[in]  stream   Byte-aligned stream containing the ELF image.
 * @param[in]  hdr      Parsed ELF file header.
 * @param[in]  dyn_shdr Section header describing `.dynamic`.
 * @param[in]  dyn_idx  Zero-based dynamic entry index.
 * @param[out] out_dyn  Populated dynamic entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_read_dyn(const bf_stream_t *stream, const bf_elf_header_t *hdr,
    const bf_elf_shdr_t *dyn_shdr, uint32_t dyn_idx, bf_elf_dyn_t *out_dyn);

/**
 * @brief Reads a relocation entry by index from an `SHT_RELA` or `SHT_REL`
 * section.
 *
 * For `SHT_REL` sections, `out_rela->addend` is set to `0`.
 *
 * @param[in]  stream   Byte-aligned stream containing the ELF image.
 * @param[in]  hdr      Parsed ELF file header.
 * @param[in]  rel_shdr Section header describing the relocation table.
 * @param[in]  rel_idx  Zero-based relocation entry index.
 * @param[out] out_rela Populated unified relocation entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_elf_read_rela(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, const bf_elf_shdr_t *rel_shdr, uint32_t rel_idx,
    bf_elf_rela_t *out_rela);

/**
 * @brief Returns a zero-copy byte slice at `[file_offset, file_offset + size)`.
 *
 * @param[in]  stream      Byte-aligned stream backing the slice.
 * @param[in]  file_offset Starting byte offset within `stream`.
 * @param[in]  size        Requested slice length in bytes.
 * @param[out] out_slice   Populated zero-copy slice on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOVERFLOW`, or `BF_EOF`.
 */
bf_status bf_elf_get_slice(const bf_stream_t *stream, uint64_t file_offset,
    uint64_t size, bf_elf_slice_t *out_slice);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_ELF_H */

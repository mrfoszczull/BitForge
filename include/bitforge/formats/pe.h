#ifndef BITFORGE_FORMATS_PE_H
#define BITFORGE_FORMATS_PE_H

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
 * @defgroup formats_pe PE / PE32+ Format Parser
 * @brief Zero-allocation C17 parser for DOS, COFF, PE32/PE32+, and section
 * tables.
 *
 * Parses MZ/DOS headers, `PE\0\0` signature and COFF header, PE32 (`0x10B`)
 * and PE32+ (`0x20B`) optional headers, data directories, section headers,
 * RVA-to-file-offset translation, and zero-copy byte slices over `bf_stream_t`.
 * @{
 */

/** @name PE Header and Entry Sizes
 * @{
 */
/** @brief Standard `IMAGE_DOS_HEADER` size in bytes. */
#define BF_PE_DOS_HEADER_LEN 64U
/** @brief Size of `"PE\\0\\0"` signature in bytes. */
#define BF_PE_SIGNATURE_LEN 4U
/** @brief Size of COFF `IMAGE_FILE_HEADER` in bytes. */
#define BF_PE_COFF_HEADER_LEN 20U
/** @brief PE32 optional header size in bytes before data directories. */
#define BF_PE32_OPT_BASE_LEN 96U
/** @brief PE32+ optional header size in bytes before data directories. */
#define BF_PE32PLUS_OPT_BASE_LEN 112U
/** @brief Size of one `IMAGE_DATA_DIRECTORY` entry in bytes. */
#define BF_PE_DATA_DIR_LEN 8U
/** @brief Size of `IMAGE_SECTION_HEADER` in bytes. */
#define BF_PE_SECTION_HEADER_LEN 40U
/** @brief Inline section name field length in bytes. */
#define BF_PE_SECTION_NAME_LEN 8U
/** @brief Standard maximum number of data directories. */
#define BF_PE_MAX_DATA_DIRS 16U
/** @} */

/** @name Optional Header Magic Values
 * @{
 */
/** @brief PE32 optional header magic (`IMAGE_NT_OPTIONAL_HDR32_MAGIC`). */
#define BF_PE32_MAGIC 0x010BU
/** @brief PE32+ optional header magic (`IMAGE_NT_OPTIONAL_HDR64_MAGIC`). */
#define BF_PE32PLUS_MAGIC 0x020BU
/** @} */

/**
 * @brief Well-known PE data directory indices (`IMAGE_DIRECTORY_ENTRY_*`).
 */
typedef enum {
	BF_PE_DIR_EXPORT = 0U,	  /**< Export directory table. */
	BF_PE_DIR_IMPORT = 1U,	  /**< Import directory table. */
	BF_PE_DIR_RESOURCE = 2U,  /**< Resource directory table. */
	BF_PE_DIR_EXCEPTION = 3U, /**< Exception table (`.pdata`). */
	BF_PE_DIR_SECURITY =
	    4U, /**< Attribute certificate table (file offset). */
	BF_PE_DIR_BASERELOC = 5U,     /**< Base relocation table (`.reloc`). */
	BF_PE_DIR_DEBUG = 6U,	      /**< Debug directory. */
	BF_PE_DIR_TLS = 9U,	      /**< TLS directory. */
	BF_PE_DIR_IAT = 12U,	      /**< Import Address Table (IAT). */
	BF_PE_DIR_DELAY_IMPORT = 13U, /**< Delay-load import descriptor. */
	BF_PE_DIR_CLR_RUNTIME = 14U   /**< CLR runtime header. */
} bf_pe_data_dir_idx_t;

/**
 * @brief Parsed DOS header (`IMAGE_DOS_HEADER`) fields needed for PE
 * navigation.
 */
typedef struct {
	uint16_t e_magic;  /**< DOS magic (`0x5A4D`, `"MZ"`). */
	uint16_t e_cblp;   /**< Bytes on last page of file. */
	uint16_t e_cp;	   /**< Pages in file. */
	uint16_t e_lfanew; /**< Low 16 bits of `e_lfanew` (`0` if `pe_offset >
			      UINT16_MAX`). */
	uint32_t
	    pe_offset; /**< Full 32-bit `e_lfanew` offset to `"PE\\0\\0"`. */
} bf_pe_dos_header_t;

/**
 * @brief COFF file header (`IMAGE_FILE_HEADER`) fields.
 */
typedef struct {
	uint16_t machine; /**< Target machine type (`IMAGE_FILE_MACHINE_*`). */
	uint16_t number_of_sections; /**< Number of section headers following
					optional header. */
	uint32_t
	    time_date_stamp; /**< Linker timestamp (seconds since epoch). */
	uint32_t
	    pointer_to_symbol_table; /**< Legacy COFF symbol table offset. */
	uint32_t number_of_symbols;  /**< Legacy COFF symbol count. */
	uint16_t size_of_optional_header; /**< Byte size of optional header
					     following COFF header. */
	uint16_t characteristics; /**< File attribute flags (`IMAGE_FILE_*`). */
} bf_pe_coff_header_t;

/**
 * @brief Unified PE32 / PE32+ optional header view.
 */
typedef struct {
	uint16_t magic; /**< `BF_PE32_MAGIC` (`0x10B`) or `BF_PE32PLUS_MAGIC`
			   (`0x20B`). */
	uint8_t major_linker_version; /**< Major linker version. */
	uint8_t minor_linker_version; /**< Minor linker version. */
	uint32_t size_of_code; /**< Combined size of code (`.text`) sections. */
	uint32_t size_of_initialized_data;   /**< Combined size of initialized
						data sections. */
	uint32_t size_of_uninitialized_data; /**< Combined size of uninitialized
						(`.bss`) sections. */
	uint32_t address_of_entry_point;     /**< Entry point RVA relative to
						`image_base`. */
	uint32_t base_of_code;		     /**< Code section base RVA. */
	uint32_t base_of_data; /**< Data section base RVA (`0` for PE32+). */
	uint64_t image_base;   /**< Preferred load address (`u32` widened for
				  PE32, `u64` for PE32+). */
	uint32_t
	    section_alignment;	  /**< In-memory section alignment in bytes. */
	uint32_t file_alignment;  /**< On-disk section alignment in bytes. */
	uint32_t size_of_image;	  /**< Total image size in memory (multiple of
				     `section_alignment`). */
	uint32_t size_of_headers; /**< Combined size of DOS, PE, COFF, optional,
				     and section headers. */
	uint32_t check_sum;	  /**< PE image checksum. */
	uint16_t
	    subsystem; /**< Required Windows subsystem (`IMAGE_SUBSYSTEM_*`). */
	uint16_t dll_characteristics; /**< DLL characteristics flags
					 (`IMAGE_DLLCHARACTERISTICS_*`). */
	uint64_t
	    size_of_stack_reserve; /**< Reserved virtual stack size in bytes. */
	uint64_t size_of_stack_commit; /**< Initially committed virtual stack
					  size in bytes. */
	uint64_t
	    size_of_heap_reserve; /**< Reserved virtual heap size in bytes. */
	uint64_t size_of_heap_commit; /**< Initially committed virtual heap size
					 in bytes. */
	uint32_t loader_flags;	      /**< Obsolete loader flags (`0`). */
	uint32_t number_of_rva_and_sizes; /**< Number of `IMAGE_DATA_DIRECTORY`
					     entries. */
	uint64_t data_dirs_offset; /**< File offset where `IMAGE_DATA_DIRECTORY`
				      array begins. */
} bf_pe_optional_header_t;

/**
 * @brief Single `IMAGE_DATA_DIRECTORY` entry (`VirtualAddress` + `Size`).
 */
typedef struct {
	uint32_t
	    virtual_address; /**< RVA of directory table (`0` if unused). */
	uint32_t size; /**< Byte size of directory table (`0` if unused). */
} bf_pe_data_dir_t;

/**
 * @brief Parsed COFF/PE section header (`IMAGE_SECTION_HEADER`).
 */
typedef struct {
	char name[BF_PE_SECTION_NAME_LEN +
		  1U];	    /**< NUL-terminated inline name (or raw 8 chars). */
	bool has_long_name; /**< `true` when `name[0] == '/'` (COFF string table
			       ref). */
	uint32_t long_name_offset; /**< Parsed offset into COFF string table if
				      `has_long_name`. */
	uint32_t virtual_size;	   /**< In-memory size (`Misc.VirtualSize`). */
	uint32_t virtual_address;  /**< Section start RVA. */
	uint32_t size_of_raw_data; /**< Payload size on disk in bytes. */
	uint32_t pointer_to_raw_data; /**< File offset of section payload. */
	uint32_t
	    pointer_to_relocations; /**< File offset of COFF relocations. */
	uint32_t pointer_to_linenumbers; /**< Obsolete line numbers offset. */
	uint16_t number_of_relocations;	 /**< Number of COFF relocations. */
	uint16_t number_of_linenumbers;	 /**< Obsolete line numbers count. */
	uint32_t characteristics;	 /**< Section flags (`IMAGE_SCN_*`). */
} bf_pe_section_t;

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
} bf_pe_slice_t;

/**
 * @brief Reads and validates the MZ/DOS header without mutating on failure.
 *
 * @param[in,out] stream   Byte-aligned stream positioned at DOS header.
 * @param[out]    out_dos  Populated DOS header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_pe_read_dos_header(
    bf_stream_t *stream, bf_pe_dos_header_t *out_dos);

/**
 * @brief Reads `"PE\\0\\0"` signature and COFF header at `dos->pe_offset`.
 *
 * Does not mutate the caller's `stream` offset.
 *
 * @param[in]  stream   Byte-aligned stream containing the PE image.
 * @param[in]  dos      Validated DOS header from `bf_pe_read_dos_header`.
 * @param[out] out_coff Populated COFF header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_pe_read_coff_header(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, bf_pe_coff_header_t *out_coff);

/**
 * @brief Reads and validates a PE32 or PE32+ optional header after `coff`.
 *
 * Does not mutate the caller's `stream` offset.
 *
 * @param[in]  stream  Byte-aligned stream containing the PE image.
 * @param[in]  dos     Validated DOS header.
 * @param[in]  coff    Validated COFF header.
 * @param[out] out_opt Populated optional header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_pe_read_optional_header(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    bf_pe_optional_header_t *out_opt);

/**
 * @brief Reads data directory entry `dir_idx` from the optional header table.
 *
 * @param[in]  stream  Byte-aligned stream containing the PE image.
 * @param[in]  opt     Parsed optional header.
 * @param[in]  dir_idx Zero-based directory index (`<
 * opt->number_of_rva_and_sizes`).
 * @param[out] out_dir Populated data directory entry on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EOVERFLOW`.
 */
bf_status bf_pe_read_data_dir(const bf_stream_t *stream,
    const bf_pe_optional_header_t *opt, uint32_t dir_idx,
    bf_pe_data_dir_t *out_dir);

/**
 * @brief Reads section header `sec_idx` (`< coff->number_of_sections`).
 *
 * Does not mutate the caller's `stream` offset.
 *
 * @param[in]  stream  Byte-aligned stream containing the PE image.
 * @param[in]  dos     Validated DOS header.
 * @param[in]  coff    Validated COFF header.
 * @param[in]  sec_idx Zero-based section index.
 * @param[out] out_sec Populated section header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_pe_read_section(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    uint16_t sec_idx, bf_pe_section_t *out_sec);

/**
 * @brief Translates a 32-bit `rva` into a raw file offset using section
 * headers.
 *
 * @param[in]  stream     Byte-aligned stream containing the PE image.
 * @param[in]  dos        Validated DOS header.
 * @param[in]  coff       Validated COFF header.
 * @param[in]  rva        Relative virtual address to translate.
 * @param[out] out_offset Resolved file byte offset on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 * `BF_EFORMAT`.
 */
bf_status bf_pe_rva_to_offset(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    uint32_t rva, uint64_t *out_offset);

/**
 * @brief Returns a zero-copy byte slice at `[file_offset, file_offset + size)`.
 *
 * @param[in]  stream      Byte-aligned stream backing the slice.
 * @param[in]  file_offset Starting byte offset within `stream`.
 * @param[in]  size        Requested slice length in bytes.
 * @param[out] out_slice   Populated zero-copy slice on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOVERFLOW`, or `BF_EOF`.
 */
bf_status bf_pe_get_slice(const bf_stream_t *stream, uint64_t file_offset,
    uint64_t size, bf_pe_slice_t *out_slice);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_PE_H */

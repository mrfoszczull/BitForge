#include <bitforge/formats/pe.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

static void write_u16_le(uint8_t *dst, uint16_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
}

static void write_u32_le(uint8_t *dst, uint32_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
	dst[2] = (uint8_t)((val >> 16) & 0xFFU);
	dst[3] = (uint8_t)((val >> 24) & 0xFFU);
}

static void write_u64_le(uint8_t *dst, uint64_t val)
{
	for (size_t i = 0U; i < 8U; i++) {
		dst[i] = (uint8_t)((val >> (i * 8U)) & 0xFFU);
	}
}

static void write_dos_header(
    uint8_t *dst, uint16_t cblp, uint16_t cp, uint32_t pe_offset)
{
	memset(dst, 0, BF_PE_DOS_HEADER_LEN);
	write_u16_le(dst + 0U, 0x5A4DU); /* "MZ" */
	write_u16_le(dst + 2U, cblp);
	write_u16_le(dst + 4U, cp);
	write_u32_le(dst + 0x3CU, pe_offset);
}

static void write_pe_sig_and_coff(uint8_t *dst, uint16_t machine,
    uint16_t num_sections, uint16_t opt_size, uint16_t characteristics)
{
	write_u32_le(dst + 0U, 0x00004550U); /* "PE\0\0" */
	write_u16_le(dst + 4U, machine);
	write_u16_le(dst + 6U, num_sections);
	write_u32_le(dst + 8U, 0x65A00000U);
	write_u32_le(dst + 12U, 0U);
	write_u32_le(dst + 16U, 0U);
	write_u16_le(dst + 20U, opt_size);
	write_u16_le(dst + 22U, characteristics);
}

static void write_section_header(uint8_t *dst, const char raw_name[8],
    uint32_t virtual_size, uint32_t virtual_address, uint32_t size_of_raw_data,
    uint32_t pointer_to_raw_data, uint32_t characteristics)
{
	memset(dst, 0, BF_PE_SECTION_HEADER_LEN);
	memcpy(dst + 0U, raw_name, 8U);
	write_u32_le(dst + 8U, virtual_size);
	write_u32_le(dst + 12U, virtual_address);
	write_u32_le(dst + 16U, size_of_raw_data);
	write_u32_le(dst + 20U, pointer_to_raw_data);
	write_u32_le(dst + 36U, characteristics);
}

static void test_pe32_headers_dirs_sections_and_rva(void)
{
	/* Layout (PE32, opt_size = 96 + 16*8 = 224 bytes):
	 * [0..64)     DOS header, pe_offset = 64
	 * [64..68)    "PE\0\0"
	 * [68..88)    COFF header (20B): machine=0x014C (i386), num_sections=2,
	 * opt_size=224 [88..184)   PE32 optional header base (96B),
	 * magic=0x10B, image_base=0x00400000 [184..312)  16 data directories
	 * (128B); entry 1 (IMPORT) at 184 + 8 = 192: rva=0x2000, size=64
	 * [312..392)  2 section headers (2 * 40B = 80B)
	 * [392..400)  8 bytes of ".text" payload ("CODE1234")
	 * [400..464)  64 bytes of ".rdata" payload
	 */
	uint8_t img[464] = {0};
	uint16_t opt_size =
	    (uint16_t)(BF_PE32_OPT_BASE_LEN +
		       BF_PE_MAX_DATA_DIRS * BF_PE_DATA_DIR_LEN);

	write_dos_header(img, 128U, 1U, 64U);
	write_pe_sig_and_coff(img + 64U, 0x014CU, 2U, opt_size, 0x0102U);

	uint8_t *opt = img + 88U;
	write_u16_le(opt + 0U, BF_PE32_MAGIC);
	opt[2] = 14U;
	opt[3] = 0U;
	write_u32_le(opt + 4U, 8U);	      /* SizeOfCode */
	write_u32_le(opt + 8U, 64U);	      /* SizeOfInitializedData */
	write_u32_le(opt + 12U, 0U);	      /* SizeOfUninitializedData */
	write_u32_le(opt + 16U, 0x1004U);     /* AddressOfEntryPoint */
	write_u32_le(opt + 20U, 0x1000U);     /* BaseOfCode */
	write_u32_le(opt + 24U, 0x2000U);     /* BaseOfData */
	write_u32_le(opt + 28U, 0x00400000U); /* ImageBase */
	write_u32_le(opt + 32U, 0x1000U);     /* SectionAlignment */
	write_u32_le(opt + 36U, 0x200U);      /* FileAlignment */
	/* [40..52): 12 bytes skipped (OS/Image/Subsystem versions +
	 * Win32VersionValue) */
	write_u32_le(opt + 52U, 0x4000U);	      /* SizeOfImage */
	write_u32_le(opt + 56U, 392U);		      /* SizeOfHeaders */
	write_u32_le(opt + 60U, 0U);		      /* CheckSum */
	write_u16_le(opt + 64U, 3U);		      /* Subsystem (CONSOLE) */
	write_u16_le(opt + 66U, 0x8140U);	      /* DllCharacteristics */
	write_u32_le(opt + 68U, 0x100000U);	      /* SizeOfStackReserve */
	write_u32_le(opt + 72U, 0x1000U);	      /* SizeOfStackCommit */
	write_u32_le(opt + 76U, 0x100000U);	      /* SizeOfHeapReserve */
	write_u32_le(opt + 80U, 0x1000U);	      /* SizeOfHeapCommit */
	write_u32_le(opt + 84U, 0U);		      /* LoaderFlags */
	write_u32_le(opt + 88U, BF_PE_MAX_DATA_DIRS); /* NumberOfRvaAndSizes */

	/* Data directory 1 (IMPORT) at opt + 96 + 8 */
	write_u32_le(opt + 96U + BF_PE_DIR_IMPORT * 8U + 0U, 0x2000U);
	write_u32_le(opt + 96U + BF_PE_DIR_IMPORT * 8U + 4U, 64U);

	write_section_header(img + 312U + 0U * BF_PE_SECTION_HEADER_LEN,
	    ".text\0\0\0", 16U, 0x1000U, 8U, 392U, 0x60000020U);
	write_section_header(img + 312U + 1U * BF_PE_SECTION_HEADER_LEN,
	    "/12\0\0\0\0\0", 128U, 0x2000U, 64U, 400U, 0x40000040U);
	memcpy(img + 392U, "CODE1234", 8U);

	bf_stream_t s;
	bf_pe_dos_header_t dos;
	bf_pe_coff_header_t coff;
	bf_pe_optional_header_t opt_hdr;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_pe_read_dos_header(&s, &dos) == BF_OK);
	CHECK(s.byte_offset == BF_PE_DOS_HEADER_LEN);
	CHECK(dos.e_magic == 0x5A4DU && dos.pe_offset == 64U);

	CHECK(bf_pe_read_coff_header(&s, &dos, &coff) == BF_OK);
	CHECK(coff.machine == 0x014CU && coff.number_of_sections == 2U);

	CHECK(bf_pe_read_optional_header(&s, &dos, &coff, &opt_hdr) == BF_OK);
	CHECK(opt_hdr.magic == BF_PE32_MAGIC);
	CHECK(opt_hdr.image_base == 0x00400000ULL);
	CHECK(opt_hdr.address_of_entry_point == 0x1004U);

	bf_pe_data_dir_t dir;
	CHECK(
	    bf_pe_read_data_dir(&s, &opt_hdr, BF_PE_DIR_IMPORT, &dir) == BF_OK);
	CHECK(dir.virtual_address == 0x2000U && dir.size == 64U);

	bf_pe_section_t sec0, sec1;
	CHECK(bf_pe_read_section(&s, &dos, &coff, 0U, &sec0) == BF_OK);
	CHECK(strcmp(sec0.name, ".text") == 0 && !sec0.has_long_name);

	CHECK(bf_pe_read_section(&s, &dos, &coff, 1U, &sec1) == BF_OK);
	CHECK(sec1.has_long_name && sec1.long_name_offset == 12U);

	uint64_t file_off = 0U;
	CHECK(
	    bf_pe_rva_to_offset(&s, &dos, &coff, 0x1004U, &file_off) == BF_OK);
	CHECK(file_off == 396U);

	bf_pe_slice_t sl;
	CHECK(bf_pe_get_slice(&s, sec0.pointer_to_raw_data,
		  sec0.size_of_raw_data, &sl) == BF_OK);
	CHECK(sl.size == 8U);
	CHECK(sl.data != NULL && memcmp(sl.data, "CODE1234", 8U) == 0);

	printf("test_pe32_headers_dirs_sections_and_rva finished.\n");
}

static void test_pe32plus_and_errors(void)
{
	/* PE32+ (0x20B) with 64-bit ImageBase and StackReserve */
	uint8_t img[320] = {0};
	uint16_t opt_size =
	    (uint16_t)(BF_PE32PLUS_OPT_BASE_LEN + 2U * BF_PE_DATA_DIR_LEN);

	write_dos_header(img, 64U, 1U, 64U);
	write_pe_sig_and_coff(img + 64U, 0x8664U, 0U, opt_size, 0x0022U);

	uint8_t *opt = img + 88U;
	write_u16_le(opt + 0U, BF_PE32PLUS_MAGIC);
	write_u32_le(opt + 16U, 0x1000U); /* AddressOfEntryPoint */
	write_u32_le(opt + 20U, 0x1000U); /* BaseOfCode */
	write_u64_le(opt + 24U, 0x0000000140000000ULL); /* ImageBase (64-bit) */
	write_u32_le(opt + 32U, 0x1000U);		/* SectionAlignment */
	write_u32_le(opt + 36U, 0x200U);		/* FileAlignment */
	/* [40..52): 12 bytes skipped */
	write_u32_le(opt + 52U, 0x2000U);     /* SizeOfImage */
	write_u32_le(opt + 56U, 216U);	      /* SizeOfHeaders */
	write_u16_le(opt + 64U, 3U);	      /* Subsystem */
	write_u16_le(opt + 66U, 0U);	      /* DllCharacteristics */
	write_u64_le(opt + 68U, 0x200000ULL); /* SizeOfStackReserve */
	write_u64_le(opt + 76U, 0x4000ULL);   /* SizeOfStackCommit */
	write_u64_le(opt + 84U, 0x100000ULL); /* SizeOfHeapReserve */
	write_u64_le(opt + 92U, 0x1000ULL);   /* SizeOfHeapCommit */
	write_u32_le(opt + 100U, 0U);	      /* LoaderFlags */
	write_u32_le(opt + 104U, 2U);	      /* NumberOfRvaAndSizes */

	bf_stream_t s;
	bf_pe_dos_header_t dos;
	bf_pe_coff_header_t coff;
	bf_pe_optional_header_t opt_hdr;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_pe_read_dos_header(&s, &dos) == BF_OK);
	CHECK(bf_pe_read_coff_header(&s, &dos, &coff) == BF_OK);
	CHECK(bf_pe_read_optional_header(&s, &dos, &coff, &opt_hdr) == BF_OK);
	CHECK(opt_hdr.magic == BF_PE32PLUS_MAGIC);
	CHECK(opt_hdr.image_base == 0x0000000140000000ULL);
	CHECK(opt_hdr.size_of_stack_reserve == 0x200000ULL);

	/* Invalid DOS magic -> BF_EFORMAT without advancing stream */
	uint8_t bad_dos[BF_PE_DOS_HEADER_LEN] = {0};
	memcpy(bad_dos, "NOPE", 4U);
	CHECK(bf_stream_init(&s, bad_dos, sizeof(bad_dos)) == BF_OK);
	CHECK(bf_pe_read_dos_header(&s, &dos) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* Truncated DOS header -> BF_EOF */
	CHECK(bf_stream_init(&s, img, 16U) == BF_OK);
	CHECK(bf_pe_read_dos_header(&s, &dos) == BF_EOF);

	/* NULL / invalid args -> BF_EINVAL */
	bf_pe_data_dir_t dir;
	bf_pe_section_t sec;
	bf_pe_slice_t sl;
	uint64_t off = 0U;
	CHECK(bf_pe_read_dos_header(NULL, &dos) == BF_EINVAL);
	CHECK(bf_pe_read_dos_header(&s, NULL) == BF_EINVAL);
	CHECK(bf_pe_read_coff_header(NULL, &dos, &coff) == BF_EINVAL);
	CHECK(bf_pe_read_optional_header(NULL, &dos, &coff, &opt_hdr) ==
	      BF_EINVAL);
	CHECK(bf_pe_read_data_dir(NULL, &opt_hdr, 0U, &dir) == BF_EINVAL);
	CHECK(bf_pe_read_section(NULL, &dos, &coff, 0U, &sec) == BF_EINVAL);
	CHECK(bf_pe_rva_to_offset(NULL, &dos, &coff, 0U, &off) == BF_EINVAL);
	CHECK(bf_pe_get_slice(NULL, 0U, 4U, &sl) == BF_EINVAL);

	printf("test_pe32plus_and_errors finished.\n");
}

int main(void)
{
	test_pe32_headers_dirs_sections_and_rva();
	test_pe32plus_and_errors();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All PE parser tests passed.\n");
	return EXIT_SUCCESS;
}

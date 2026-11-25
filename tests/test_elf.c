#include <bitforge/formats/elf.h>
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

static void write_u16_enc(uint8_t *dst, uint8_t enc, uint16_t val)
{
	if (enc == BF_ELF_DATA_2MSB) {
		dst[0] = (uint8_t)((val >> 8) & 0xFFU);
		dst[1] = (uint8_t)(val & 0xFFU);
	} else {
		dst[0] = (uint8_t)(val & 0xFFU);
		dst[1] = (uint8_t)((val >> 8) & 0xFFU);
	}
}

static void write_u32_enc(uint8_t *dst, uint8_t enc, uint32_t val)
{
	if (enc == BF_ELF_DATA_2MSB) {
		dst[0] = (uint8_t)((val >> 24) & 0xFFU);
		dst[1] = (uint8_t)((val >> 16) & 0xFFU);
		dst[2] = (uint8_t)((val >> 8) & 0xFFU);
		dst[3] = (uint8_t)(val & 0xFFU);
	} else {
		dst[0] = (uint8_t)(val & 0xFFU);
		dst[1] = (uint8_t)((val >> 8) & 0xFFU);
		dst[2] = (uint8_t)((val >> 16) & 0xFFU);
		dst[3] = (uint8_t)((val >> 24) & 0xFFU);
	}
}

static void write_u64_enc(uint8_t *dst, uint8_t enc, uint64_t val)
{
	if (enc == BF_ELF_DATA_2MSB) {
		for (size_t i = 0U; i < 8U; i++) {
			dst[i] = (uint8_t)((val >> ((7U - i) * 8U)) & 0xFFU);
		}
	} else {
		for (size_t i = 0U; i < 8U; i++) {
			dst[i] = (uint8_t)((val >> (i * 8U)) & 0xFFU);
		}
	}
}

static void write_s64_enc(uint8_t *dst, uint8_t enc, int64_t val)
{
	uint64_t raw = 0U;
	memcpy(&raw, &val, sizeof(raw));
	write_u64_enc(dst, enc, raw);
}

static void write_s32_enc(uint8_t *dst, uint8_t enc, int32_t val)
{
	uint32_t raw = 0U;
	memcpy(&raw, &val, sizeof(raw));
	write_u32_enc(dst, enc, raw);
}

static void write_ehdr64(uint8_t *dst, uint8_t enc, uint16_t type,
    uint16_t machine, uint64_t entry, uint64_t phoff, uint64_t shoff,
    uint16_t phentsize, uint16_t phnum, uint16_t shentsize, uint16_t shnum,
    uint16_t shstrndx)
{
	memset(dst, 0, BF_ELF64_EHDR_LEN);
	dst[0] = 0x7FU;
	dst[1] = 'E';
	dst[2] = 'L';
	dst[3] = 'F';
	dst[4] = BF_ELF_CLASS_64;
	dst[5] = enc;
	dst[6] = 1U;

	write_u16_enc(dst + 16U, enc, type);
	write_u16_enc(dst + 18U, enc, machine);
	write_u32_enc(dst + 20U, enc, 1U);
	write_u64_enc(dst + 24U, enc, entry);
	write_u64_enc(dst + 32U, enc, phoff);
	write_u64_enc(dst + 40U, enc, shoff);
	write_u32_enc(dst + 48U, enc, 0U);
	write_u16_enc(dst + 52U, enc, BF_ELF64_EHDR_LEN);
	write_u16_enc(dst + 54U, enc, phentsize);
	write_u16_enc(dst + 56U, enc, phnum);
	write_u16_enc(dst + 58U, enc, shentsize);
	write_u16_enc(dst + 60U, enc, shnum);
	write_u16_enc(dst + 62U, enc, shstrndx);
}

static void write_ehdr32(uint8_t *dst, uint8_t enc, uint16_t type,
    uint16_t machine, uint32_t entry, uint32_t phoff, uint32_t shoff,
    uint16_t phentsize, uint16_t phnum, uint16_t shentsize, uint16_t shnum,
    uint16_t shstrndx)
{
	memset(dst, 0, BF_ELF32_EHDR_LEN);
	dst[0] = 0x7FU;
	dst[1] = 'E';
	dst[2] = 'L';
	dst[3] = 'F';
	dst[4] = BF_ELF_CLASS_32;
	dst[5] = enc;
	dst[6] = 1U;

	write_u16_enc(dst + 16U, enc, type);
	write_u16_enc(dst + 18U, enc, machine);
	write_u32_enc(dst + 20U, enc, 1U);
	write_u32_enc(dst + 24U, enc, entry);
	write_u32_enc(dst + 28U, enc, phoff);
	write_u32_enc(dst + 32U, enc, shoff);
	write_u32_enc(dst + 36U, enc, 0U);
	write_u16_enc(dst + 40U, enc, BF_ELF32_EHDR_LEN);
	write_u16_enc(dst + 42U, enc, phentsize);
	write_u16_enc(dst + 44U, enc, phnum);
	write_u16_enc(dst + 46U, enc, shentsize);
	write_u16_enc(dst + 48U, enc, shnum);
	write_u16_enc(dst + 50U, enc, shstrndx);
}

static void write_phdr64(uint8_t *dst, uint8_t enc, uint32_t type,
    uint32_t flags, uint64_t offset, uint64_t vaddr, uint64_t filesz,
    uint64_t memsz, uint64_t align)
{
	write_u32_enc(dst + 0U, enc, type);
	write_u32_enc(dst + 4U, enc, flags);
	write_u64_enc(dst + 8U, enc, offset);
	write_u64_enc(dst + 16U, enc, vaddr);
	write_u64_enc(dst + 24U, enc, vaddr);
	write_u64_enc(dst + 32U, enc, filesz);
	write_u64_enc(dst + 40U, enc, memsz);
	write_u64_enc(dst + 48U, enc, align);
}

static void write_shdr64(uint8_t *dst, uint8_t enc, uint32_t name,
    uint32_t type, uint64_t flags, uint64_t addr, uint64_t offset,
    uint64_t size, uint32_t link, uint32_t info, uint64_t addralign,
    uint64_t entsize)
{
	write_u32_enc(dst + 0U, enc, name);
	write_u32_enc(dst + 4U, enc, type);
	write_u64_enc(dst + 8U, enc, flags);
	write_u64_enc(dst + 16U, enc, addr);
	write_u64_enc(dst + 24U, enc, offset);
	write_u64_enc(dst + 32U, enc, size);
	write_u32_enc(dst + 40U, enc, link);
	write_u32_enc(dst + 44U, enc, info);
	write_u64_enc(dst + 48U, enc, addralign);
	write_u64_enc(dst + 56U, enc, entsize);
}

static void write_shdr32(uint8_t *dst, uint8_t enc, uint32_t name,
    uint32_t type, uint32_t flags, uint32_t addr, uint32_t offset,
    uint32_t size, uint32_t link, uint32_t info, uint32_t addralign,
    uint32_t entsize)
{
	write_u32_enc(dst + 0U, enc, name);
	write_u32_enc(dst + 4U, enc, type);
	write_u32_enc(dst + 8U, enc, flags);
	write_u32_enc(dst + 12U, enc, addr);
	write_u32_enc(dst + 16U, enc, offset);
	write_u32_enc(dst + 20U, enc, size);
	write_u32_enc(dst + 24U, enc, link);
	write_u32_enc(dst + 28U, enc, info);
	write_u32_enc(dst + 32U, enc, addralign);
	write_u32_enc(dst + 36U, enc, entsize);
}

static void test_elf64_le_headers_phdrs_shdrs_and_slices(void)
{
	/* Layout:
	 * [0..64)     ELF64 LE header (1 phdr, 3 shdrs, shstrndx=2)
	 * [64..120)   phdr[0] (PT_LOAD at off=120, filesz=4, memsz=4)
	 * [120..128)  payload ("ABCD" at 120..124)
	 * [128..142)  shstrtab ("\0.text\0.shstrtab" = 14 bytes)
	 * [144..336)  3 * shdr64 (192 bytes):
	 *               0: SHT_NULL
	 *               1: ".text" (SHT_PROGBITS, off=120, size=4)
	 *               2: ".shstrtab" (SHT_STRTAB, off=128, size=14)
	 */
	uint8_t img[336] = {0};
	uint8_t enc = BF_ELF_DATA_2LSB;

	static const char shstr[] = "\0.text\0.shstrtab";
	memcpy(img + 128U, shstr, sizeof(shstr));
	memcpy(img + 120U, "ABCD", 4U);

	write_ehdr64(img, enc, BF_ELF_ET_EXEC, 62U, 0x400000ULL, 64U, 144U,
	    BF_ELF64_PHDR_LEN, 1U, BF_ELF64_SHDR_LEN, 3U, 2U);

	write_phdr64(img + 64U, enc, BF_ELF_PT_LOAD, 5U, 120U, 0x400000ULL, 4U,
	    4U, 0x1000ULL);

	write_shdr64(img + 144U + 0U * BF_ELF64_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_NULL, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U);
	write_shdr64(img + 144U + 1U * BF_ELF64_SHDR_LEN, enc, 1U,
	    BF_ELF_SHT_PROGBITS, 6U, 0x400000ULL, 120U, 4U, 0U, 0U, 4U, 0U);
	write_shdr64(img + 144U + 2U * BF_ELF64_SHDR_LEN, enc, 7U,
	    BF_ELF_SHT_STRTAB, 0U, 0U, 128U, (uint64_t)sizeof(shstr), 0U, 0U,
	    1U, 0U);

	bf_stream_t s;
	bf_elf_header_t hdr;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_OK);
	CHECK(s.byte_offset == BF_ELF64_EHDR_LEN);
	CHECK(hdr.elf_class == BF_ELF_CLASS_64);
	CHECK(hdr.data_encoding == BF_ELF_DATA_2LSB);
	CHECK(hdr.entry == 0x400000ULL);
	CHECK(hdr.phnum == 1U);
	CHECK(hdr.shnum == 3U);

	bf_elf_phdr_t ph;
	CHECK(bf_elf_read_phdr(&s, &hdr, 0U, &ph) == BF_OK);
	CHECK(ph.type == BF_ELF_PT_LOAD);
	CHECK(ph.offset == 120U);
	CHECK(ph.filesz == 4U);

	bf_elf_shdr_t sh1;
	const char *sec_name = NULL;
	CHECK(bf_elf_read_shdr(&s, &hdr, 1U, &sh1) == BF_OK);
	CHECK(bf_elf_get_section_name(&s, &hdr, &sh1, &sec_name) == BF_OK);
	CHECK(sec_name != NULL && strcmp(sec_name, ".text") == 0);

	bf_elf_slice_t sl;
	CHECK(bf_elf_get_slice(&s, sh1.offset, sh1.size, &sl) == BF_OK);
	CHECK(sl.size == 4U);
	CHECK(sl.data != NULL && memcmp(sl.data, "ABCD", 4U) == 0);

	printf("test_elf64_le_headers_phdrs_shdrs_and_slices finished.\n");
}

static void test_elf32_be_syms_dyn_and_relocations(void)
{
	/* ELF32 Big-Endian object with:
	 * - shdr[0]: SHT_NULL
	 * - shdr[1]: .strtab at off=256 ("\0main\0libfoo.so")
	 * - shdr[2]: .symtab at off=272, size=32 (2 * 16B Elf32_Sym)
	 * - shdr[3]: .dynamic at off=304, size=16 (2 * 8B Elf32_Dyn)
	 * - shdr[4]: .rela.text at off=320, size=12 (1 * 12B Elf32_Rela)
	 * - shdr[5]: .rel.text at off=332, size=8  (1 * 8B Elf32_Rel)
	 * Section table at off=344 (6 * 40B = 240B) -> total 584 bytes.
	 */
	uint8_t img[584] = {0};
	uint8_t enc = BF_ELF_DATA_2MSB;

	static const char strtab_data[] = "\0main\0libfoo.so";
	memcpy(img + 256U, strtab_data, sizeof(strtab_data));

	/* sym[1]: name=1 ("main"), value=0x100, size=0x20,
	 * info=(STB_GLOBAL<<4)|STT_FUNC, shndx=1 */
	write_u32_enc(img + 272U + 16U + 0U, enc, 1U);
	write_u32_enc(img + 272U + 16U + 4U, enc, 0x100U);
	write_u32_enc(img + 272U + 16U + 8U, enc, 0x20U);
	img[272U + 16U + 12U] =
	    (uint8_t)((BF_ELF_STB_GLOBAL << 4) | BF_ELF_STT_FUNC);
	img[272U + 16U + 13U] = 0U;
	write_u16_enc(img + 272U + 16U + 14U, enc, 1U);

	/* dyn[0]: DT_NEEDED (1), val=6 ("libfoo.so"); dyn[1]: DT_NULL (0) */
	write_s32_enc(img + 304U + 0U, enc, BF_ELF_DT_NEEDED);
	write_u32_enc(img + 304U + 4U, enc, 6U);
	write_s32_enc(img + 304U + 8U, enc, BF_ELF_DT_NULL);
	write_u32_enc(img + 304U + 12U, enc, 0U);

	/* rela[0]: offset=0x104, info=(1<<8)|2, addend=-4 */
	uint32_t r32_info = (1U << 8) | 2U;
	write_u32_enc(img + 320U + 0U, enc, 0x104U);
	write_u32_enc(img + 320U + 4U, enc, r32_info);
	write_s32_enc(img + 320U + 8U, enc, -4);

	/* rel[0]: offset=0x108, info=(1<<8)|3 */
	write_u32_enc(img + 332U + 0U, enc, 0x108U);
	write_u32_enc(img + 332U + 4U, enc, (1U << 8) | 3U);

	write_ehdr32(img, enc, BF_ELF_ET_REL, 8U, 0U, 0U, 344U,
	    BF_ELF32_PHDR_LEN, 0U, BF_ELF32_SHDR_LEN, 6U, 1U);

	write_shdr32(img + 344U + 0U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_NULL, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U);
	write_shdr32(img + 344U + 1U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_STRTAB, 0U, 0U, 256U, (uint32_t)sizeof(strtab_data), 0U,
	    0U, 1U, 0U);
	write_shdr32(img + 344U + 2U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_SYMTAB, 0U, 0U, 272U, 32U, 1U, 1U, 4U, BF_ELF32_SYM_LEN);
	write_shdr32(img + 344U + 3U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_DYNAMIC, 0U, 0U, 304U, 16U, 1U, 0U, 4U,
	    BF_ELF32_DYN_LEN);
	write_shdr32(img + 344U + 4U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_RELA, 0U, 0U, 320U, 12U, 2U, 1U, 4U, BF_ELF32_RELA_LEN);
	write_shdr32(img + 344U + 5U * BF_ELF32_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_REL, 0U, 0U, 332U, 8U, 2U, 1U, 4U, BF_ELF32_REL_LEN);

	bf_stream_t s;
	bf_elf_header_t hdr;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_OK);
	CHECK(hdr.elf_class == BF_ELF_CLASS_32);
	CHECK(hdr.data_encoding == BF_ELF_DATA_2MSB);

	bf_elf_shdr_t str_sh, sym_sh, dyn_sh, rela_sh, rel_sh;
	CHECK(bf_elf_read_shdr(&s, &hdr, 1U, &str_sh) == BF_OK);
	CHECK(bf_elf_read_shdr(&s, &hdr, 2U, &sym_sh) == BF_OK);
	CHECK(bf_elf_read_shdr(&s, &hdr, 3U, &dyn_sh) == BF_OK);
	CHECK(bf_elf_read_shdr(&s, &hdr, 4U, &rela_sh) == BF_OK);
	CHECK(bf_elf_read_shdr(&s, &hdr, 5U, &rel_sh) == BF_OK);

	bf_elf_slice_t strtab;
	CHECK(
	    bf_elf_get_slice(&s, str_sh.offset, str_sh.size, &strtab) == BF_OK);

	bf_elf_sym_t sym;
	const char *sym_name = NULL;
	CHECK(bf_elf_read_sym(&s, &hdr, &sym_sh, 1U, &sym) == BF_OK);
	CHECK(BF_ELF_ST_BIND(sym.info) == BF_ELF_STB_GLOBAL);
	CHECK(BF_ELF_ST_TYPE(sym.info) == BF_ELF_STT_FUNC);
	CHECK(sym.value == 0x100U && sym.size == 0x20U);
	CHECK(bf_elf_get_sym_name(&strtab, sym.name, &sym_name) == BF_OK);
	CHECK(sym_name != NULL && strcmp(sym_name, "main") == 0);

	bf_elf_dyn_t dyn;
	const char *needed_name = NULL;
	CHECK(bf_elf_read_dyn(&s, &hdr, &dyn_sh, 0U, &dyn) == BF_OK);
	CHECK(dyn.tag == BF_ELF_DT_NEEDED && dyn.val == 6U);
	CHECK(bf_elf_get_sym_name(&strtab, (uint32_t)dyn.val, &needed_name) ==
	      BF_OK);
	CHECK(needed_name != NULL && strcmp(needed_name, "libfoo.so") == 0);

	bf_elf_rela_t rela, rel;
	CHECK(bf_elf_read_rela(&s, &hdr, &rela_sh, 0U, &rela) == BF_OK);
	CHECK(rela.offset == 0x104U && rela.addend == -4);
	CHECK(BF_ELF32_R_SYM(rela.info) == 1U &&
	      BF_ELF32_R_TYPE(rela.info) == 2U);

	CHECK(bf_elf_read_rela(&s, &hdr, &rel_sh, 0U, &rel) == BF_OK);
	CHECK(rel.offset == 0x108U && rel.addend == 0);
	CHECK(
	    BF_ELF32_R_SYM(rel.info) == 1U && BF_ELF32_R_TYPE(rel.info) == 3U);

	printf("test_elf32_be_syms_dyn_and_relocations finished.\n");
}

static void test_elf64_relocations_and_helpers(void)
{
	/* Layout:
	 * [0..64)    ELF64 LE header (shoff=64, shnum=1, shstrndx=0)
	 * [64..128)  shdr[0] (SHT_RELA, offset=128, size=24, entsize=24)
	 * [128..152) Elf64_Rela entry (offset=0x401020, sym=7, type=4,
	 * addend=-16)
	 */
	uint8_t img[152] = {0};
	uint8_t enc = BF_ELF_DATA_2LSB;

	uint64_t r64_info = ((uint64_t)7U << 32) | 4U;

	write_ehdr64(img, enc, BF_ELF_ET_REL, 62U, 0U, 0U, 64U,
	    BF_ELF64_PHDR_LEN, 0U, BF_ELF64_SHDR_LEN, 1U, 0U);
	write_shdr64(img + 64U, enc, 0U, BF_ELF_SHT_RELA, 0U, 0U, 128U, 24U, 0U,
	    0U, 8U, BF_ELF64_RELA_LEN);
	write_u64_enc(img + 128U + 0U, enc, 0x401020ULL);
	write_u64_enc(img + 128U + 8U, enc, r64_info);
	write_s64_enc(img + 128U + 16U, enc, -16);

	bf_stream_t s;
	bf_elf_header_t hdr;
	bf_elf_shdr_t rel_sh;
	bf_elf_rela_t rela;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_OK);
	CHECK(bf_elf_read_shdr(&s, &hdr, 0U, &rel_sh) == BF_OK);
	CHECK(bf_elf_read_rela(&s, &hdr, &rel_sh, 0U, &rela) == BF_OK);
	CHECK(rela.offset == 0x401020ULL);
	CHECK(rela.addend == -16);
	CHECK(BF_ELF64_R_SYM(rela.info) == 7U);
	CHECK(BF_ELF64_R_TYPE(rela.info) == 4U);

	printf("test_elf64_relocations_and_helpers finished.\n");
}

static void test_elf_pn_xnum_shn_xindex_and_errors(void)
{
	/* Extended count/index via shdr[0]:
	 * shnum=0 -> sh0.size = 3; shstrndx=BF_ELF_SHN_XINDEX -> sh0.link = 2.
	 * Layout:
	 * [0..64)     ELF64 LE header
	 * [64..80)    .shstrtab ("\0.xsec\0.shstrtab" = 14 bytes)
	 * [80..272)   3 * shdr64 (192 bytes)
	 */
	uint8_t img[272] = {0};
	uint8_t enc = BF_ELF_DATA_2LSB;

	static const char shstr[] = "\0.xsec\0.shstrtab";
	memcpy(img + 64U, shstr, sizeof(shstr));

	write_ehdr64(img, enc, BF_ELF_ET_DYN, 62U, 0U, 0U, 80U,
	    BF_ELF64_PHDR_LEN, 0U, BF_ELF64_SHDR_LEN, 0U, BF_ELF_SHN_XINDEX);

	write_shdr64(img + 80U + 0U * BF_ELF64_SHDR_LEN, enc, 0U,
	    BF_ELF_SHT_NULL, 0U, 0U, 0U, 3U, 2U, 0U, 0U, 0U);
	write_shdr64(img + 80U + 1U * BF_ELF64_SHDR_LEN, enc, 1U,
	    BF_ELF_SHT_PROGBITS, 2U, 0U, 64U, 4U, 0U, 0U, 1U, 0U);
	write_shdr64(img + 80U + 2U * BF_ELF64_SHDR_LEN, enc, 7U,
	    BF_ELF_SHT_STRTAB, 0U, 0U, 64U, (uint64_t)sizeof(shstr), 0U, 0U, 1U,
	    0U);

	bf_stream_t s;
	bf_elf_header_t hdr;
	CHECK(bf_stream_init(&s, img, sizeof(img)) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_OK);

	uint64_t sh_count = 0U;
	uint32_t str_idx = 0U;
	CHECK(bf_elf_section_count(&s, &hdr, &sh_count) == BF_OK &&
	      sh_count == 3U);
	CHECK(bf_elf_shstrndx(&s, &hdr, &str_idx) == BF_OK && str_idx == 2U);

	bf_elf_shdr_t sh1;
	const char *name = NULL;
	CHECK(bf_elf_read_shdr(&s, &hdr, 1U, &sh1) == BF_OK);
	CHECK(bf_elf_get_section_name(&s, &hdr, &sh1, &name) == BF_OK);
	CHECK(name != NULL && strcmp(name, ".xsec") == 0);

	/* Bad magic -> BF_EFORMAT without advancing stream */
	uint8_t bad_magic[64] = {0};
	memcpy(bad_magic, "NOPE", 4U);
	CHECK(bf_stream_init(&s, bad_magic, sizeof(bad_magic)) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* Truncated header -> BF_EOF */
	CHECK(bf_stream_init(&s, img, 16U) == BF_OK);
	CHECK(bf_elf_read_header(&s, &hdr) == BF_EOF);

	/* NULL / invalid args -> BF_EINVAL */
	bf_elf_phdr_t ph;
	bf_elf_shdr_t sh;
	bf_elf_sym_t sym;
	bf_elf_dyn_t dyn;
	bf_elf_rela_t rela;
	bf_elf_slice_t sl;
	CHECK(bf_elf_read_magic(NULL) == BF_EINVAL);
	CHECK(bf_elf_read_header(NULL, &hdr) == BF_EINVAL);
	CHECK(bf_elf_read_header(&s, NULL) == BF_EINVAL);
	CHECK(bf_elf_read_phdr(&s, NULL, 0U, &ph) == BF_EINVAL);
	CHECK(bf_elf_read_shdr(&s, NULL, 0U, &sh) == BF_EINVAL);
	CHECK(bf_elf_read_sym(&s, NULL, &sh, 0U, &sym) == BF_EINVAL);
	CHECK(bf_elf_read_dyn(&s, NULL, &sh, 0U, &dyn) == BF_EINVAL);
	CHECK(bf_elf_read_rela(&s, NULL, &sh, 0U, &rela) == BF_EINVAL);
	CHECK(bf_elf_get_slice(NULL, 0U, 4U, &sl) == BF_EINVAL);

	printf("test_elf_pn_xnum_shn_xindex_and_errors finished.\n");
}

int main(void)
{
	test_elf64_le_headers_phdrs_shdrs_and_slices();
	test_elf32_be_syms_dyn_and_relocations();
	test_elf64_relocations_and_helpers();
	test_elf_pn_xnum_shn_xindex_and_errors();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All ELF parser tests passed.\n");
	return EXIT_SUCCESS;
}

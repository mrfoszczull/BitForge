#include <bitforge/formats/elf.h>
#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool stream_aligned_ready(const bf_stream_t *stream)
{
	if (stream == NULL) {
		return false;
	}
	if (stream->data == NULL && stream->size > 0U) {
		return false;
	}
	if (stream->byte_offset > stream->size) {
		return false;
	}
	if (stream->bit_offset != 0U) {
		return false;
	}
	return true;
}

static bf_status read_u16_endian(
    bf_stream_t *s, uint8_t data_encoding, uint16_t *out)
{
	if (data_encoding == BF_ELF_DATA_2LSB) {
		return bf_parser_read_u16_le(s, out);
	}
	if (data_encoding == BF_ELF_DATA_2MSB) {
		return bf_parser_read_u16_be(s, out);
	}
	return BF_EFORMAT;
}

static bf_status read_u32_endian(
    bf_stream_t *s, uint8_t data_encoding, uint32_t *out)
{
	if (data_encoding == BF_ELF_DATA_2LSB) {
		return bf_parser_read_u32_le(s, out);
	}
	if (data_encoding == BF_ELF_DATA_2MSB) {
		return bf_parser_read_u32_be(s, out);
	}
	return BF_EFORMAT;
}

static bf_status read_u64_endian(
    bf_stream_t *s, uint8_t data_encoding, uint64_t *out)
{
	if (data_encoding == BF_ELF_DATA_2LSB) {
		return bf_parser_read_u64_le(s, out);
	}
	if (data_encoding == BF_ELF_DATA_2MSB) {
		return bf_parser_read_u64_be(s, out);
	}
	return BF_EFORMAT;
}

static bf_status read_s64_endian(
    bf_stream_t *s, uint8_t data_encoding, int64_t *out)
{
	uint64_t raw = 0U;
	bf_status st = read_u64_endian(s, data_encoding, &raw);
	if (st != BF_OK) {
		return st;
	}
	int64_t signed_val = 0;
	memcpy(&signed_val, &raw, sizeof(signed_val));
	*out = signed_val;
	return BF_OK;
}

static bf_status read_s32_as_s64_endian(
    bf_stream_t *s, uint8_t data_encoding, int64_t *out)
{
	uint32_t raw = 0U;
	bf_status st = read_u32_endian(s, data_encoding, &raw);
	if (st != BF_OK) {
		return st;
	}
	int32_t signed32 = 0;
	memcpy(&signed32, &raw, sizeof(signed32));
	*out = (int64_t)signed32;
	return BF_OK;
}

static bf_status read_addr_or_off(
    bf_stream_t *s, uint8_t elf_class, uint8_t data_encoding, uint64_t *out)
{
	if (elf_class == BF_ELF_CLASS_32) {
		uint32_t v32 = 0U;
		bf_status st = read_u32_endian(s, data_encoding, &v32);
		if (st != BF_OK) {
			return st;
		}
		*out = (uint64_t)v32;
		return BF_OK;
	}
	if (elf_class == BF_ELF_CLASS_64) {
		return read_u64_endian(s, data_encoding, out);
	}
	return BF_EFORMAT;
}

static bool u64_to_size(uint64_t val, size_t *out)
{
	if (val > (uint64_t)SIZE_MAX) {
		return false;
	}
	*out = (size_t)val;
	return true;
}

static bf_status extract_cstr_from_slice(
    const uint8_t *buf, size_t buf_size, size_t offset, const char **out_str)
{
	if (buf == NULL || out_str == NULL) {
		return BF_EINVAL;
	}
	if (offset >= buf_size) {
		return BF_EOF;
	}

	size_t i = offset;
	while (i < buf_size && buf[i] != '\0') {
		i++;
	}
	if (i == buf_size) {
		return BF_EFORMAT;
	}

	*out_str = (const char *)(buf + offset);
	return BF_OK;
}

static bf_status locate_indexed_entry(const bf_stream_t *stream,
    uint64_t table_off, uint64_t table_size, uint16_t entry_size, uint64_t idx,
    uint16_t min_entry_size, bf_stream_t *out_entry_stream)
{
	if (!stream_aligned_ready(stream) || out_entry_stream == NULL) {
		return BF_EINVAL;
	}
	if (entry_size < min_entry_size) {
		return BF_EFORMAT;
	}
	if ((uint64_t)entry_size != 0U &&
	    idx > UINT64_MAX / (uint64_t)entry_size) {
		return BF_EOVERFLOW;
	}

	uint64_t rel_off = idx * (uint64_t)entry_size;
	if (rel_off > UINT64_MAX - (uint64_t)entry_size) {
		return BF_EOVERFLOW;
	}
	if (rel_off + (uint64_t)entry_size > table_size) {
		return BF_EOF;
	}
	if (table_off > UINT64_MAX - rel_off) {
		return BF_EOVERFLOW;
	}

	uint64_t abs_off = table_off + rel_off;
	size_t abs_sz = 0U;
	size_t ent_sz = 0U;
	if (!u64_to_size(abs_off, &abs_sz) ||
	    !u64_to_size((uint64_t)entry_size, &ent_sz)) {
		return BF_EOVERFLOW;
	}

	if (abs_sz > stream->size || ent_sz > stream->size - abs_sz) {
		return BF_EOF;
	}

	return bf_stream_init(out_entry_stream, stream->data + abs_sz, ent_sz);
}

/* Raw section header reader avoiding recursion into bf_elf_section_count. */
static bf_status read_shdr_at(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, uint64_t table_size, uint32_t idx,
    bf_elf_shdr_t *out_shdr)
{
	uint16_t min_sz = (hdr->elf_class == BF_ELF_CLASS_32)
			      ? BF_ELF32_SHDR_LEN
			      : BF_ELF64_SHDR_LEN;
	bf_stream_t es;
	bf_status st = locate_indexed_entry(stream, hdr->shoff, table_size,
	    hdr->shentsize, (uint64_t)idx, min_sz, &es);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_shdr_t sh;
	memset(&sh, 0, sizeof(sh));
	uint8_t enc = hdr->data_encoding;

	if (hdr->elf_class == BF_ELF_CLASS_32) {
		uint32_t fl = 0U, ad = 0U, of = 0U, sz = 0U, al = 0U, esz = 0U;
		if (read_u32_endian(&es, enc, &sh.name) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.type) != BF_OK ||
		    read_u32_endian(&es, enc, &fl) != BF_OK ||
		    read_u32_endian(&es, enc, &ad) != BF_OK ||
		    read_u32_endian(&es, enc, &of) != BF_OK ||
		    read_u32_endian(&es, enc, &sz) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.link) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.info) != BF_OK ||
		    read_u32_endian(&es, enc, &al) != BF_OK ||
		    read_u32_endian(&es, enc, &esz) != BF_OK) {
			return BF_EFORMAT;
		}
		sh.flags = fl;
		sh.addr = ad;
		sh.offset = of;
		sh.size = sz;
		sh.addralign = al;
		sh.entsize = esz;
	} else {
		if (read_u32_endian(&es, enc, &sh.name) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.type) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.flags) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.addr) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.offset) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.size) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.link) != BF_OK ||
		    read_u32_endian(&es, enc, &sh.info) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.addralign) != BF_OK ||
		    read_u64_endian(&es, enc, &sh.entsize) != BF_OK) {
			return BF_EFORMAT;
		}
	}

	*out_shdr = sh;
	return BF_OK;
}

static bf_status read_shdr_zero(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, bf_elf_shdr_t *out_shdr0)
{
	if (!stream_aligned_ready(stream) || hdr == NULL || out_shdr0 == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}
	if (hdr->shoff == 0U || hdr->shentsize == 0U) {
		return BF_EFORMAT;
	}

	uint64_t min_table_size = (uint64_t)hdr->shentsize;
	return read_shdr_at(stream, hdr, min_table_size, 0U, out_shdr0);
}

bf_status bf_elf_read_magic(bf_stream_t *stream)
{
	static const uint8_t k_elf_magic[4] = {0x7FU, 'E', 'L', 'F'};

	if (!stream_aligned_ready(stream)) {
		return BF_EINVAL;
	}

	return bf_parser_expect_bytes(stream, k_elf_magic, sizeof(k_elf_magic));
}

bf_status bf_elf_read_header(bf_stream_t *stream, bf_elf_header_t *out_header)
{
	if (!stream_aligned_ready(stream) || out_header == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	size_t base_offset = probe.byte_offset;
	bf_status status = bf_elf_read_magic(&probe);
	if (status != BF_OK) {
		return status;
	}

	uint8_t ident[BF_ELF_IDENT_LEN] = {0U};
	ident[0] = 0x7FU;
	ident[1] = 'E';
	ident[2] = 'L';
	ident[3] = 'F';

	if (12U > probe.size - probe.byte_offset) {
		return BF_EOF;
	}
	memcpy(ident + 4U, probe.data + probe.byte_offset, 12U);
	probe.byte_offset += 12U;

	uint8_t elf_class = ident[4];
	uint8_t data_enc = ident[5];
	uint8_t ident_ver = ident[6];

	if ((elf_class != BF_ELF_CLASS_32 && elf_class != BF_ELF_CLASS_64) ||
	    (data_enc != BF_ELF_DATA_2LSB && data_enc != BF_ELF_DATA_2MSB) ||
	    ident_ver != 1U) {
		return BF_EFORMAT;
	}

	uint16_t min_ehsize = (elf_class == BF_ELF_CLASS_32)
				  ? BF_ELF32_EHDR_LEN
				  : BF_ELF64_EHDR_LEN;

	bf_elf_header_t hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.elf_class = elf_class;
	hdr.data_encoding = data_enc;
	hdr.ident_version = ident_ver;
	hdr.os_abi = ident[7];

	status = read_u16_endian(&probe, data_enc, &hdr.type);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.machine);
	if (status != BF_OK) {
		return status;
	}
	status = read_u32_endian(&probe, data_enc, &hdr.version);
	if (status != BF_OK) {
		return status;
	}
	status = read_addr_or_off(&probe, elf_class, data_enc, &hdr.entry);
	if (status != BF_OK) {
		return status;
	}
	status = read_addr_or_off(&probe, elf_class, data_enc, &hdr.phoff);
	if (status != BF_OK) {
		return status;
	}
	status = read_addr_or_off(&probe, elf_class, data_enc, &hdr.shoff);
	if (status != BF_OK) {
		return status;
	}
	status = read_u32_endian(&probe, data_enc, &hdr.flags);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.ehsize);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.phentsize);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.phnum);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.shentsize);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.shnum);
	if (status != BF_OK) {
		return status;
	}
	status = read_u16_endian(&probe, data_enc, &hdr.shstrndx);
	if (status != BF_OK) {
		return status;
	}

	if (hdr.version != 1U || hdr.ehsize < min_ehsize) {
		return BF_EFORMAT;
	}

	if ((size_t)hdr.ehsize > probe.size - base_offset) {
		return BF_EOF;
	}

	probe.byte_offset = base_offset + (size_t)hdr.ehsize;
	*out_header = hdr;
	*stream = probe;
	return BF_OK;
}

bf_status bf_elf_section_count(
    const bf_stream_t *stream, const bf_elf_header_t *hdr, uint64_t *out_count)
{
	if (!stream_aligned_ready(stream) || hdr == NULL || out_count == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}

	if (hdr->shnum != 0U) {
		*out_count = (uint64_t)hdr->shnum;
		return BF_OK;
	}

	if (hdr->shoff == 0U) {
		*out_count = 0U;
		return BF_OK;
	}

	bf_elf_shdr_t sh0;
	bf_status st = read_shdr_zero(stream, hdr, &sh0);
	if (st != BF_OK) {
		return st;
	}

	*out_count = sh0.size;
	return BF_OK;
}

bf_status bf_elf_shstrndx(
    const bf_stream_t *stream, const bf_elf_header_t *hdr, uint32_t *out_index)
{
	if (!stream_aligned_ready(stream) || hdr == NULL || out_index == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}

	if (hdr->shstrndx != BF_ELF_SHN_XINDEX) {
		*out_index = (uint32_t)hdr->shstrndx;
		return BF_OK;
	}

	bf_elf_shdr_t sh0;
	bf_status st = read_shdr_zero(stream, hdr, &sh0);
	if (st != BF_OK) {
		return st;
	}

	*out_index = sh0.link;
	return BF_OK;
}

bf_status bf_elf_read_phdr(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, uint16_t phnum_idx, bf_elf_phdr_t *out_phdr)
{
	if (hdr == NULL || out_phdr == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}
	if (phnum_idx >= hdr->phnum) {
		return BF_EOF;
	}

	uint16_t min_sz = (hdr->elf_class == BF_ELF_CLASS_32)
			      ? BF_ELF32_PHDR_LEN
			      : BF_ELF64_PHDR_LEN;
	uint64_t table_size = (uint64_t)hdr->phnum * (uint64_t)hdr->phentsize;
	bf_stream_t es;
	bf_status st = locate_indexed_entry(stream, hdr->phoff, table_size,
	    hdr->phentsize, (uint64_t)phnum_idx, min_sz, &es);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_phdr_t ph;
	memset(&ph, 0, sizeof(ph));
	uint8_t enc = hdr->data_encoding;

	if (hdr->elf_class == BF_ELF_CLASS_32) {
		uint32_t off = 0U, va = 0U, pa = 0U, fsz = 0U, msz = 0U,
			 al = 0U;
		if (read_u32_endian(&es, enc, &ph.type) != BF_OK ||
		    read_u32_endian(&es, enc, &off) != BF_OK ||
		    read_u32_endian(&es, enc, &va) != BF_OK ||
		    read_u32_endian(&es, enc, &pa) != BF_OK ||
		    read_u32_endian(&es, enc, &fsz) != BF_OK ||
		    read_u32_endian(&es, enc, &msz) != BF_OK ||
		    read_u32_endian(&es, enc, &ph.flags) != BF_OK ||
		    read_u32_endian(&es, enc, &al) != BF_OK) {
			return BF_EFORMAT;
		}
		ph.offset = off;
		ph.vaddr = va;
		ph.paddr = pa;
		ph.filesz = fsz;
		ph.memsz = msz;
		ph.align = al;
	} else {
		if (read_u32_endian(&es, enc, &ph.type) != BF_OK ||
		    read_u32_endian(&es, enc, &ph.flags) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.offset) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.vaddr) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.paddr) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.filesz) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.memsz) != BF_OK ||
		    read_u64_endian(&es, enc, &ph.align) != BF_OK) {
			return BF_EFORMAT;
		}
	}

	*out_phdr = ph;
	return BF_OK;
}

bf_status bf_elf_read_shdr(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, uint32_t shnum_idx, bf_elf_shdr_t *out_shdr)
{
	if (hdr == NULL || out_shdr == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}

	uint64_t count = 0U;
	bf_status st = bf_elf_section_count(stream, hdr, &count);
	if (st != BF_OK) {
		return st;
	}
	if ((uint64_t)shnum_idx >= count) {
		return BF_EOF;
	}
	if (count > 0U && (uint64_t)hdr->shentsize > UINT64_MAX / count) {
		return BF_EOVERFLOW;
	}

	uint64_t table_size = count * (uint64_t)hdr->shentsize;
	return read_shdr_at(stream, hdr, table_size, shnum_idx, out_shdr);
}

bf_status bf_elf_get_section_name(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, const bf_elf_shdr_t *shdr,
    const char **out_name)
{
	if (!stream_aligned_ready(stream) || hdr == NULL || shdr == NULL ||
	    out_name == NULL) {
		return BF_EINVAL;
	}

	uint32_t str_idx = 0U;
	bf_status st = bf_elf_shstrndx(stream, hdr, &str_idx);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_shdr_t str_shdr;
	st = bf_elf_read_shdr(stream, hdr, str_idx, &str_shdr);
	if (st != BF_OK) {
		return st;
	}
	if (str_shdr.type != BF_ELF_SHT_STRTAB) {
		return BF_EFORMAT;
	}

	bf_elf_slice_t str_slice;
	st = bf_elf_get_slice(
	    stream, str_shdr.offset, str_shdr.size, &str_slice);
	if (st != BF_OK) {
		return st;
	}

	return extract_cstr_from_slice(
	    str_slice.data, str_slice.size, (size_t)shdr->name, out_name);
}

bf_status bf_elf_read_sym(const bf_stream_t *stream, const bf_elf_header_t *hdr,
    const bf_elf_shdr_t *sym_shdr, uint32_t sym_idx, bf_elf_sym_t *out_sym)
{
	if (hdr == NULL || sym_shdr == NULL || out_sym == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}
	if (sym_shdr->type != BF_ELF_SHT_SYMTAB &&
	    sym_shdr->type != BF_ELF_SHT_DYNSYM) {
		return BF_EFORMAT;
	}

	uint16_t min_sz = (hdr->elf_class == BF_ELF_CLASS_32)
			      ? BF_ELF32_SYM_LEN
			      : BF_ELF64_SYM_LEN;
	uint16_t entry_sz = (sym_shdr->entsize >= min_sz)
				? (uint16_t)sym_shdr->entsize
				: min_sz;

	bf_stream_t es;
	bf_status st = locate_indexed_entry(stream, sym_shdr->offset,
	    sym_shdr->size, entry_sz, (uint64_t)sym_idx, min_sz, &es);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_sym_t sym;
	memset(&sym, 0, sizeof(sym));
	uint8_t enc = hdr->data_encoding;

	if (hdr->elf_class == BF_ELF_CLASS_32) {
		uint32_t val = 0U, sz = 0U;
		if (read_u32_endian(&es, enc, &sym.name) != BF_OK ||
		    read_u32_endian(&es, enc, &val) != BF_OK ||
		    read_u32_endian(&es, enc, &sz) != BF_OK ||
		    bf_parser_read_u8(&es, &sym.info) != BF_OK ||
		    bf_parser_read_u8(&es, &sym.other) != BF_OK ||
		    read_u16_endian(&es, enc, &sym.shndx) != BF_OK) {
			return BF_EFORMAT;
		}
		sym.value = val;
		sym.size = sz;
	} else {
		if (read_u32_endian(&es, enc, &sym.name) != BF_OK ||
		    bf_parser_read_u8(&es, &sym.info) != BF_OK ||
		    bf_parser_read_u8(&es, &sym.other) != BF_OK ||
		    read_u16_endian(&es, enc, &sym.shndx) != BF_OK ||
		    read_u64_endian(&es, enc, &sym.value) != BF_OK ||
		    read_u64_endian(&es, enc, &sym.size) != BF_OK) {
			return BF_EFORMAT;
		}
	}

	*out_sym = sym;
	return BF_OK;
}

bf_status bf_elf_get_sym_name(
    const bf_elf_slice_t *strtab, uint32_t name_offset, const char **out_name)
{
	if (strtab == NULL || out_name == NULL) {
		return BF_EINVAL;
	}
	if (strtab->size > 0U && strtab->data == NULL) {
		return BF_EINVAL;
	}

	return extract_cstr_from_slice(
	    strtab->data, strtab->size, (size_t)name_offset, out_name);
}

bf_status bf_elf_read_dyn(const bf_stream_t *stream, const bf_elf_header_t *hdr,
    const bf_elf_shdr_t *dyn_shdr, uint32_t dyn_idx, bf_elf_dyn_t *out_dyn)
{
	if (hdr == NULL || dyn_shdr == NULL || out_dyn == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}
	if (dyn_shdr->type != BF_ELF_SHT_DYNAMIC) {
		return BF_EFORMAT;
	}

	uint16_t min_sz = (hdr->elf_class == BF_ELF_CLASS_32)
			      ? BF_ELF32_DYN_LEN
			      : BF_ELF64_DYN_LEN;
	uint16_t entry_sz = (dyn_shdr->entsize >= min_sz)
				? (uint16_t)dyn_shdr->entsize
				: min_sz;

	bf_stream_t es;
	bf_status st = locate_indexed_entry(stream, dyn_shdr->offset,
	    dyn_shdr->size, entry_sz, (uint64_t)dyn_idx, min_sz, &es);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_dyn_t dyn;
	memset(&dyn, 0, sizeof(dyn));
	uint8_t enc = hdr->data_encoding;

	if (hdr->elf_class == BF_ELF_CLASS_32) {
		uint32_t raw_val = 0U;
		if (read_s32_as_s64_endian(&es, enc, &dyn.tag) != BF_OK ||
		    read_u32_endian(&es, enc, &raw_val) != BF_OK) {
			return BF_EFORMAT;
		}
		dyn.val = (uint64_t)raw_val;
	} else {
		if (read_s64_endian(&es, enc, &dyn.tag) != BF_OK ||
		    read_u64_endian(&es, enc, &dyn.val) != BF_OK) {
			return BF_EFORMAT;
		}
	}

	*out_dyn = dyn;
	return BF_OK;
}

bf_status bf_elf_read_rela(const bf_stream_t *stream,
    const bf_elf_header_t *hdr, const bf_elf_shdr_t *rel_shdr, uint32_t rel_idx,
    bf_elf_rela_t *out_rela)
{
	if (hdr == NULL || rel_shdr == NULL || out_rela == NULL) {
		return BF_EINVAL;
	}
	if (hdr->elf_class != BF_ELF_CLASS_32 &&
	    hdr->elf_class != BF_ELF_CLASS_64) {
		return BF_EFORMAT;
	}
	if (rel_shdr->type != BF_ELF_SHT_RELA &&
	    rel_shdr->type != BF_ELF_SHT_REL) {
		return BF_EFORMAT;
	}

	bool has_addend = (rel_shdr->type == BF_ELF_SHT_RELA);
	uint16_t min_sz =
	    (hdr->elf_class == BF_ELF_CLASS_32)
		? (has_addend ? BF_ELF32_RELA_LEN : BF_ELF32_REL_LEN)
		: (has_addend ? BF_ELF64_RELA_LEN : BF_ELF64_REL_LEN);
	uint16_t entry_sz = (rel_shdr->entsize >= min_sz)
				? (uint16_t)rel_shdr->entsize
				: min_sz;

	bf_stream_t es;
	bf_status st = locate_indexed_entry(stream, rel_shdr->offset,
	    rel_shdr->size, entry_sz, (uint64_t)rel_idx, min_sz, &es);
	if (st != BF_OK) {
		return st;
	}

	bf_elf_rela_t rela;
	memset(&rela, 0, sizeof(rela));
	uint8_t enc = hdr->data_encoding;

	if (hdr->elf_class == BF_ELF_CLASS_32) {
		uint32_t off32 = 0U, info32 = 0U;
		if (read_u32_endian(&es, enc, &off32) != BF_OK ||
		    read_u32_endian(&es, enc, &info32) != BF_OK) {
			return BF_EFORMAT;
		}
		rela.offset = (uint64_t)off32;
		rela.info = (uint64_t)info32;
		if (has_addend) {
			if (read_s32_as_s64_endian(&es, enc, &rela.addend) !=
			    BF_OK) {
				return BF_EFORMAT;
			}
		}
	} else {
		if (read_u64_endian(&es, enc, &rela.offset) != BF_OK ||
		    read_u64_endian(&es, enc, &rela.info) != BF_OK) {
			return BF_EFORMAT;
		}
		if (has_addend) {
			if (read_s64_endian(&es, enc, &rela.addend) != BF_OK) {
				return BF_EFORMAT;
			}
		}
	}

	*out_rela = rela;
	return BF_OK;
}

bf_status bf_elf_get_slice(const bf_stream_t *stream, uint64_t file_offset,
    uint64_t size, bf_elf_slice_t *out_slice)
{
	if (!stream_aligned_ready(stream) || out_slice == NULL) {
		return BF_EINVAL;
	}
	if (file_offset > UINT64_MAX - size) {
		return BF_EOVERFLOW;
	}

	size_t off_sz = 0U;
	size_t len_sz = 0U;
	if (!u64_to_size(file_offset, &off_sz) || !u64_to_size(size, &len_sz)) {
		return BF_EOVERFLOW;
	}

	if (off_sz > stream->size || len_sz > stream->size - off_sz) {
		return BF_EOF;
	}

	out_slice->data = (len_sz > 0U) ? (stream->data + off_sz) : NULL;
	out_slice->size = len_sz;
	return BF_OK;
}

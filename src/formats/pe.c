#include <bitforge/formats/pe.h>
#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define PE_DOS_MAGIC 0x5A4DU /* "MZ" in little-endian u16 */
#define PE_LFANEW_OFFSET 0x3CU
#define PE_SIG_DW_LE 0x00004550U /* "PE\0\0" in little-endian u32 */

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

static bool u64_to_size(uint64_t val, size_t *out)
{
	if (val > (uint64_t)SIZE_MAX) {
		return false;
	}
	*out = (size_t)val;
	return true;
}

static bf_status sub_stream_at(const bf_stream_t *stream, uint64_t offset,
    uint64_t span, bf_stream_t *out_sub)
{
	if (!stream_aligned_ready(stream) || out_sub == NULL) {
		return BF_EINVAL;
	}
	if (offset > UINT64_MAX - span) {
		return BF_EOVERFLOW;
	}

	size_t off_sz = 0U;
	size_t len_sz = 0U;
	if (!u64_to_size(offset, &off_sz) || !u64_to_size(span, &len_sz)) {
		return BF_EOVERFLOW;
	}
	if (off_sz > stream->size || len_sz > stream->size - off_sz) {
		return BF_EOF;
	}

	return bf_stream_init(out_sub, stream->data + off_sz, len_sz);
}

static void parse_section_name(
    const uint8_t raw[BF_PE_SECTION_NAME_LEN], bf_pe_section_t *out_sec)
{
	memcpy(out_sec->name, raw, BF_PE_SECTION_NAME_LEN);
	out_sec->name[BF_PE_SECTION_NAME_LEN] = '\0';
	out_sec->has_long_name = (raw[0] == '/');
	out_sec->long_name_offset = 0U;

	if (out_sec->has_long_name) {
		uint32_t acc = 0U;
		for (size_t i = 1U; i < BF_PE_SECTION_NAME_LEN; i++) {
			uint8_t ch = raw[i];
			if (ch == '\0') {
				break;
			}
			if (ch >= (uint8_t)'0' && ch <= (uint8_t)'9') {
				acc = acc * 10U + (uint32_t)(ch - (uint8_t)'0');
			} else {
				break;
			}
		}
		out_sec->long_name_offset = acc;
	}
}

bf_status bf_pe_read_dos_header(
    bf_stream_t *stream, bf_pe_dos_header_t *out_dos)
{
	if (!stream_aligned_ready(stream) || out_dos == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	size_t start_off = probe.byte_offset;

	if (BF_PE_DOS_HEADER_LEN > probe.size - start_off) {
		return BF_EOF;
	}

	uint16_t magic = 0U;
	if (bf_parser_read_u16_le(&probe, &magic) != BF_OK) {
		return BF_EOF;
	}
	if (magic != PE_DOS_MAGIC) {
		return BF_EFORMAT;
	}

	uint16_t cblp = 0U;
	uint16_t cp = 0U;
	if (bf_parser_read_u16_le(&probe, &cblp) != BF_OK ||
	    bf_parser_read_u16_le(&probe, &cp) != BF_OK) {
		return BF_EOF;
	}

	if (PE_LFANEW_OFFSET > probe.size - start_off) {
		return BF_EOF;
	}
	probe.byte_offset = start_off + PE_LFANEW_OFFSET;

	uint32_t lfanew = 0U;
	if (bf_parser_read_u32_le(&probe, &lfanew) != BF_OK) {
		return BF_EOF;
	}

	bf_pe_dos_header_t dos;
	memset(&dos, 0, sizeof(dos));
	dos.e_magic = magic;
	dos.e_cblp = cblp;
	dos.e_cp = cp;
	dos.e_lfanew = (lfanew <= (uint32_t)UINT16_MAX) ? (uint16_t)lfanew : 0U;
	dos.pe_offset = lfanew;

	probe.byte_offset = start_off + BF_PE_DOS_HEADER_LEN;
	*out_dos = dos;
	*stream = probe;
	return BF_OK;
}

bf_status bf_pe_read_coff_header(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, bf_pe_coff_header_t *out_coff)
{
	if (!stream_aligned_ready(stream) || dos == NULL || out_coff == NULL) {
		return BF_EINVAL;
	}
	if (dos->e_magic != PE_DOS_MAGIC) {
		return BF_EFORMAT;
	}

	uint64_t total_len =
	    (uint64_t)BF_PE_SIGNATURE_LEN + (uint64_t)BF_PE_COFF_HEADER_LEN;
	bf_stream_t s;
	bf_status st =
	    sub_stream_at(stream, (uint64_t)dos->pe_offset, total_len, &s);
	if (st != BF_OK) {
		return st;
	}

	uint32_t pe_sig = 0U;
	if (bf_parser_read_u32_le(&s, &pe_sig) != BF_OK) {
		return BF_EOF;
	}
	if (pe_sig != PE_SIG_DW_LE) {
		return BF_EFORMAT;
	}

	bf_pe_coff_header_t coff;
	memset(&coff, 0, sizeof(coff));
	if (bf_parser_read_u16_le(&s, &coff.machine) != BF_OK ||
	    bf_parser_read_u16_le(&s, &coff.number_of_sections) != BF_OK ||
	    bf_parser_read_u32_le(&s, &coff.time_date_stamp) != BF_OK ||
	    bf_parser_read_u32_le(&s, &coff.pointer_to_symbol_table) != BF_OK ||
	    bf_parser_read_u32_le(&s, &coff.number_of_symbols) != BF_OK ||
	    bf_parser_read_u16_le(&s, &coff.size_of_optional_header) != BF_OK ||
	    bf_parser_read_u16_le(&s, &coff.characteristics) != BF_OK) {
		return BF_EOF;
	}

	*out_coff = coff;
	return BF_OK;
}

bf_status bf_pe_read_optional_header(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    bf_pe_optional_header_t *out_opt)
{
	if (!stream_aligned_ready(stream) || dos == NULL || coff == NULL ||
	    out_opt == NULL) {
		return BF_EINVAL;
	}
	if (coff->size_of_optional_header == 0U) {
		return BF_EFORMAT;
	}

	uint64_t hdr_prefix_len =
	    (uint64_t)BF_PE_SIGNATURE_LEN + (uint64_t)BF_PE_COFF_HEADER_LEN;
	if ((uint64_t)dos->pe_offset > UINT64_MAX - hdr_prefix_len) {
		return BF_EOVERFLOW;
	}
	uint64_t opt_off = (uint64_t)dos->pe_offset + hdr_prefix_len;

	bf_stream_t s;
	bf_status st = sub_stream_at(
	    stream, opt_off, (uint64_t)coff->size_of_optional_header, &s);
	if (st != BF_OK) {
		return st;
	}

	uint16_t magic = 0U;
	if (bf_parser_read_u16_le(&s, &magic) != BF_OK) {
		return BF_EOF;
	}
	if (magic != BF_PE32_MAGIC && magic != BF_PE32PLUS_MAGIC) {
		return BF_EFORMAT;
	}

	bool is_pe32plus = (magic == BF_PE32PLUS_MAGIC);
	uint16_t min_base_len =
	    is_pe32plus ? BF_PE32PLUS_OPT_BASE_LEN : BF_PE32_OPT_BASE_LEN;
	if (coff->size_of_optional_header < min_base_len) {
		return BF_EFORMAT;
	}

	bf_pe_optional_header_t opt;
	memset(&opt, 0, sizeof(opt));
	opt.magic = magic;

	if (bf_parser_read_u8(&s, &opt.major_linker_version) != BF_OK ||
	    bf_parser_read_u8(&s, &opt.minor_linker_version) != BF_OK ||
	    bf_parser_read_u32_le(&s, &opt.size_of_code) != BF_OK ||
	    bf_parser_read_u32_le(&s, &opt.size_of_initialized_data) != BF_OK ||
	    bf_parser_read_u32_le(&s, &opt.size_of_uninitialized_data) !=
		BF_OK ||
	    bf_parser_read_u32_le(&s, &opt.address_of_entry_point) != BF_OK ||
	    bf_parser_read_u32_le(&s, &opt.base_of_code) != BF_OK) {
		return BF_EOF;
	}

	if (!is_pe32plus) {
		uint32_t base_data = 0U;
		uint32_t img_base32 = 0U;
		uint32_t stk_res32 = 0U, stk_com32 = 0U, hp_res32 = 0U,
			 hp_com32 = 0U;
		if (bf_parser_read_u32_le(&s, &base_data) != BF_OK ||
		    bf_parser_read_u32_le(&s, &img_base32) != BF_OK) {
			return BF_EOF;
		}
		opt.base_of_data = base_data;
		opt.image_base = (uint64_t)img_base32;

		if (bf_parser_read_u32_le(&s, &opt.section_alignment) !=
			BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.file_alignment) != BF_OK) {
			return BF_EOF;
		}
		if (12U > s.size - s.byte_offset) {
			return BF_EOF;
		}
		s.byte_offset += 12U;

		if (bf_parser_read_u32_le(&s, &opt.size_of_image) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.size_of_headers) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.check_sum) != BF_OK ||
		    bf_parser_read_u16_le(&s, &opt.subsystem) != BF_OK ||
		    bf_parser_read_u16_le(&s, &opt.dll_characteristics) !=
			BF_OK ||
		    bf_parser_read_u32_le(&s, &stk_res32) != BF_OK ||
		    bf_parser_read_u32_le(&s, &stk_com32) != BF_OK ||
		    bf_parser_read_u32_le(&s, &hp_res32) != BF_OK ||
		    bf_parser_read_u32_le(&s, &hp_com32) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.loader_flags) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.number_of_rva_and_sizes) !=
			BF_OK) {
			return BF_EOF;
		}
		opt.size_of_stack_reserve = (uint64_t)stk_res32;
		opt.size_of_stack_commit = (uint64_t)stk_com32;
		opt.size_of_heap_reserve = (uint64_t)hp_res32;
		opt.size_of_heap_commit = (uint64_t)hp_com32;
	} else {
		if (bf_parser_read_u64_le(&s, &opt.image_base) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.section_alignment) !=
			BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.file_alignment) != BF_OK) {
			return BF_EOF;
		}
		if (12U > s.size - s.byte_offset) {
			return BF_EOF;
		}
		s.byte_offset += 12U;

		if (bf_parser_read_u32_le(&s, &opt.size_of_image) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.size_of_headers) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.check_sum) != BF_OK ||
		    bf_parser_read_u16_le(&s, &opt.subsystem) != BF_OK ||
		    bf_parser_read_u16_le(&s, &opt.dll_characteristics) !=
			BF_OK ||
		    bf_parser_read_u64_le(&s, &opt.size_of_stack_reserve) !=
			BF_OK ||
		    bf_parser_read_u64_le(&s, &opt.size_of_stack_commit) !=
			BF_OK ||
		    bf_parser_read_u64_le(&s, &opt.size_of_heap_reserve) !=
			BF_OK ||
		    bf_parser_read_u64_le(&s, &opt.size_of_heap_commit) !=
			BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.loader_flags) != BF_OK ||
		    bf_parser_read_u32_le(&s, &opt.number_of_rva_and_sizes) !=
			BF_OK) {
			return BF_EOF;
		}
	}

	uint64_t dirs_rel = (uint64_t)min_base_len;
	if (opt_off > UINT64_MAX - dirs_rel) {
		return BF_EOVERFLOW;
	}
	opt.data_dirs_offset = opt_off + dirs_rel;

	*out_opt = opt;
	return BF_OK;
}

bf_status bf_pe_read_data_dir(const bf_stream_t *stream,
    const bf_pe_optional_header_t *opt, uint32_t dir_idx,
    bf_pe_data_dir_t *out_dir)
{
	if (!stream_aligned_ready(stream) || opt == NULL || out_dir == NULL) {
		return BF_EINVAL;
	}
	if (dir_idx >= opt->number_of_rva_and_sizes) {
		return BF_EOF;
	}

	uint64_t rel = (uint64_t)dir_idx * (uint64_t)BF_PE_DATA_DIR_LEN;
	if (opt->data_dirs_offset > UINT64_MAX - rel) {
		return BF_EOVERFLOW;
	}

	bf_stream_t s;
	bf_status st = sub_stream_at(stream, opt->data_dirs_offset + rel,
	    (uint64_t)BF_PE_DATA_DIR_LEN, &s);
	if (st != BF_OK) {
		return st;
	}

	bf_pe_data_dir_t dir;
	memset(&dir, 0, sizeof(dir));
	if (bf_parser_read_u32_le(&s, &dir.virtual_address) != BF_OK ||
	    bf_parser_read_u32_le(&s, &dir.size) != BF_OK) {
		return BF_EOF;
	}

	*out_dir = dir;
	return BF_OK;
}

bf_status bf_pe_read_section(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    uint16_t sec_idx, bf_pe_section_t *out_sec)
{
	if (!stream_aligned_ready(stream) || dos == NULL || coff == NULL ||
	    out_sec == NULL) {
		return BF_EINVAL;
	}
	if (sec_idx >= coff->number_of_sections) {
		return BF_EOF;
	}

	uint64_t sec_table_off = (uint64_t)dos->pe_offset +
				 (uint64_t)BF_PE_SIGNATURE_LEN +
				 (uint64_t)BF_PE_COFF_HEADER_LEN +
				 (uint64_t)coff->size_of_optional_header;
	uint64_t rel = (uint64_t)sec_idx * (uint64_t)BF_PE_SECTION_HEADER_LEN;
	if (sec_table_off > UINT64_MAX - rel) {
		return BF_EOVERFLOW;
	}

	bf_stream_t s;
	bf_status st = sub_stream_at(stream, sec_table_off + rel,
	    (uint64_t)BF_PE_SECTION_HEADER_LEN, &s);
	if (st != BF_OK) {
		return st;
	}

	uint8_t raw_name[BF_PE_SECTION_NAME_LEN] = {0U};
	if (BF_PE_SECTION_NAME_LEN > s.size - s.byte_offset) {
		return BF_EOF;
	}
	memcpy(raw_name, s.data + s.byte_offset, BF_PE_SECTION_NAME_LEN);
	s.byte_offset += BF_PE_SECTION_NAME_LEN;

	bf_pe_section_t sec;
	memset(&sec, 0, sizeof(sec));
	parse_section_name(raw_name, &sec);

	if (bf_parser_read_u32_le(&s, &sec.virtual_size) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.virtual_address) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.size_of_raw_data) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.pointer_to_raw_data) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.pointer_to_relocations) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.pointer_to_linenumbers) != BF_OK ||
	    bf_parser_read_u16_le(&s, &sec.number_of_relocations) != BF_OK ||
	    bf_parser_read_u16_le(&s, &sec.number_of_linenumbers) != BF_OK ||
	    bf_parser_read_u32_le(&s, &sec.characteristics) != BF_OK) {
		return BF_EOF;
	}

	*out_sec = sec;
	return BF_OK;
}

bf_status bf_pe_rva_to_offset(const bf_stream_t *stream,
    const bf_pe_dos_header_t *dos, const bf_pe_coff_header_t *coff,
    uint32_t rva, uint64_t *out_offset)
{
	if (!stream_aligned_ready(stream) || dos == NULL || coff == NULL ||
	    out_offset == NULL) {
		return BF_EINVAL;
	}

	for (uint16_t i = 0U; i < coff->number_of_sections; i++) {
		bf_pe_section_t sec;
		bf_status st = bf_pe_read_section(stream, dos, coff, i, &sec);
		if (st != BF_OK) {
			return st;
		}

		uint32_t span = (sec.virtual_size > sec.size_of_raw_data)
				    ? sec.virtual_size
				    : sec.size_of_raw_data;
		if (span == 0U) {
			continue;
		}

		uint64_t start = (uint64_t)sec.virtual_address;
		uint64_t end = start + (uint64_t)span;
		if ((uint64_t)rva >= start && (uint64_t)rva < end) {
			uint64_t delta = (uint64_t)rva - start;
			if (delta >= (uint64_t)sec.size_of_raw_data) {
				return BF_EFORMAT;
			}
			uint64_t raw_off =
			    (uint64_t)sec.pointer_to_raw_data + delta;
			size_t off_sz = 0U;
			if (!u64_to_size(raw_off, &off_sz) ||
			    off_sz > stream->size) {
				return BF_EOF;
			}
			*out_offset = raw_off;
			return BF_OK;
		}
	}

	return BF_EFORMAT;
}

bf_status bf_pe_get_slice(const bf_stream_t *stream, uint64_t file_offset,
    uint64_t size, bf_pe_slice_t *out_slice)
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

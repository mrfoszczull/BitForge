#include <bitforge/status.h>

const char *bf_status_string(bf_status status)
{
	switch (status) {
	case BF_OK:
		return "Success";
	case BF_EINVAL:
		return "Invalid argument";
	case BF_ENOMEM:
		return "Out of memory";
	case BF_EIO:
		return "Input/output error";
	case BF_EOF:
		return "End of input";
	case BF_ENOTFOUND:
		return "Not found";
	case BF_EOVERFLOW:
		return "Arithmetic overflow";
	case BF_EUNSUPPORTED:
		return "Operation not supported";
	case BF_EFORMAT:
		return "Invalid data format";
	default:
		return "Unknown status";
	}
}

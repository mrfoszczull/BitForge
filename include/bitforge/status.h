#ifndef BITFORGE_STATUS_H
#define BITFORGE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Status codes returned by BitForge functions.
 */
typedef enum {
	BF_OK,	     /**< Operation completed successfully. */
	BF_EINVAL,   /**< An argument is invalid. */
	BF_ENOMEM,   /**< Memory allocation failed. */
	BF_EIO,	     /**< An input/output operation failed. */
	BF_EOF,	     /**< End of input was reached. */
	BF_ENOTFOUND /**< Pattern is not found. */
} bf_status;

/**
 * @brief Returns a human-readable string for a status code.
 *
 * @param status Status code to describe.
 * @return A constant string describing the status.
 */
const char *bf_status_string(bf_status status);

#ifdef __cplusplus
}
#endif

#endif

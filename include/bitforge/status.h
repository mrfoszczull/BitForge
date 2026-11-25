#ifndef BITFORGE_STATUS_H
#define BITFORGE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup status Status codes
 * @brief Status codes and error handling
 * @{
 */

/**
 * @brief Status codes returned by BitForge functions.
 *
 * These status codes are shared across all BitForge modules and indicate
 * whether an operation completed successfully or encountered an error.
 */
typedef enum {
	BF_OK,	   /**< Operation completed successfully. */
	BF_EINVAL, /**< An argument is invalid or an object is in an invalid
		      state. */
	BF_ENOMEM, /**< Memory allocation failed. */
	BF_EIO,	   /**< An input/output operation failed. */
	BF_EOF,	   /**< End of input was reached before the requested operation
		      could be completed. */
	BF_ENOTFOUND, /**< The requested element, value, or pattern was not
			 found. */
	BF_EOVERFLOW, /**< An arithmetic operation or requested size exceeds the
			 representable range. */
	BF_EUNSUPPORTED, /**< The requested operation or feature is not
			    supported. */
	BF_EFORMAT /**< Input data does not conform to the expected format or
		      structure. */
} bf_status;

/**
 * @brief Returns a human-readable string for a status code.
 *
 * @param status Status code to describe.
 * @return A constant string describing the status.
 */
const char *bf_status_string(bf_status status);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_STATUS_H */

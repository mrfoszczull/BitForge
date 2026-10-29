#ifndef STATUS_H
#define STATUS_H

/**
 * @brief Status codes returned by BitForge functions.
 */
typedef enum {
    BF_OK,     /**< Operation completed successfully. */
    BF_EINVAL, /**< An argument is invalid. */
    BF_ENOMEM, /**< Memory allocation failed. */
    BF_EIO,    /**< An input/output operation failed. */
    BF_EOF     /**< End of input was reached. */
} bf_status;

/**
 * @brief Returns a human-readable string for a status code.
 *
 * @param status Status code to describe.
 * @return A constant string describing the status.
 */
const char *bf_status_string(bf_status status);

#endif

#ifndef STATUS_H
#define STATUS_H

typedef enum { BF_OK, BF_EINVAL, BF_ENOMEM, BF_EIO, BF_EOF } bf_status;

const char *bf_status_string(bf_status status);

#endif

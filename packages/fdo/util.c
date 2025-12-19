// Copyright 2024 VinCSS JSC. All rights reserved.
//
// Filename: fdo/util.c
//   Author: VanNC
//  Created: 03/12/2024 22:01:16 +07:00

#include "util.h"

#include <ctype.h>
#include <stdlib.h>

#include "safe_lib.h"
#include "snprintf_s.h"
#include "util.h"

#define N_BYTES_PER_LINE 16

bool file_exists(char const *filename) {
    FILE *fd = NULL;

    if (filename == NULL) {
        return false;
    }

    if (fd = fopen(filename, "rb"), fd == NULL) {
        return false;
    }

    if (fclose(fd) == EOF) {
        LOG(LOG_INFO, "file_exists->fclose failed");
    }

    return true;
}

size_t get_file_size(char const *filename) {
    size_t length = 0;
    FILE  *fd     = fopen(filename, "rb");

    if (fd == NULL) {
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    length = ftell(fd);
    if (fclose(fd) == EOF) {
        LOG(LOG_INFO, "get_file_size->fclose failed");
    }

    return length;
}

int read_buffer_from_file(const char *filename, uint8_t *buffer, size_t size) {
    FILE  *fd         = fopen(filename, "rb");
    size_t bytes_read = 0;

    if (fd == NULL) {
        return -1;
    }

    if (bytes_read = fread(buffer, 1, size, fd), bytes_read != size) {
        if (fclose(fd) == EOF) {
            LOG(LOG_INFO, "read_buffer_from_file->fclose failed");
        }
        return -1;
    }

    if (fclose(fd) == EOF) {
        LOG(LOG_INFO, "read_buffer_from_file->fclose failed");
    }

    return 0;
}

int write_buffer_to_file(const char *filename, uint8_t *buffer, size_t size) {
    FILE  *fd              = fopen(filename, "w");
    size_t n_bytes_written = 0;

    if (fd == NULL) {
        return -1;
    }

    if (n_bytes_written = fwrite(buffer, 1, size, fd), n_bytes_written != size) {
        if (fclose(fd) == EOF) {
            LOG(LOG_INFO, "write_buffer_to_file->fclose failed");
        }
        return -1;
    }

    if (fclose(fd) == EOF) {
        LOG(LOG_INFO, "write_buffer_to_file->fclose failed");
    }

    return 0;
}

static inline char DEREF(_byte_to_hex(char DEREF(buff), uint8_t value)) {
    uint8_t lo = (value >> 0) & 0xf;
    uint8_t hi = (value >> 4) & 0xf;

    buff = SET_UINT8(buff, hi + (hi < 0xa ? 0x30 : 0x61 - 0xa));
    buff = SET_UINT8(buff, lo + (lo < 0xa ? 0x30 : 0x61 - 0xa));

    return buff;
}

void *fdo_alloc(size_t size) {
    void *buf = NULL;

    if (size == 0 || size > R_MAX_SIZE) {
        LOG(LOG_ERROR, "failed, size should be between 1 and %d", R_MAX_SIZE);
        goto end;
    }

    if (buf = malloc(size), buf == NULL) {
        LOG(LOG_ERROR, "fdo_alloc->malloc failed");
        goto end;
    }

    if (memset_s(buf, size, 0) != 0) {
        LOG(LOG_ERROR, "fdo_alloc->memset_s failed");
        fdo_free(buf);
        goto end;
    }

end:
    return buf;
}

// Copyright 2024 VinCSS JSC. All rights reserved.
//
// Filename: fdo/util.h
//   Author: VanNC
//  Created: 03/12/2024 22:00:08 +07:00

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "klibc/log.h"
#include "klibc/macro.h"

MODULE_LOG_TAG("fdo");

#define fdo_free(x)                                                                                                    \
    {                                                                                                                  \
        free(x);                                                                                                       \
        x = NULL;                                                                                                      \
    }

#define LOG_ERROR                          KLIBC_LOG_ERROR
#define LOG_INFO                           KLIBC_LOG_INFO
#define LOG_WARN                           KLIBC_LOG_WARNING
#define LOG_DEBUG                          KLIBC_LOG_DEBUG
#define LOG_DEBUGNTS                       KLIBC_LOG_DEBUGNTS
#define LOG_ALL                            KLIBC_LOG_ALL

#define LOG_MAX_LEVEL                      KLIBC_LOG_MAX_LEVEL
#define LOG_LEVEL                          KLIBC_LOG_LEVEL

#define LOG(level, format, ...)            KLIBC_LOG_PRINTF(level, format, ##__VA_ARGS__)
#define HEXDUMP(title, buff, n_bytes_buff) KLIBC_LOG_HEXDUMP(LOG_INFO, title, buff, n_bytes_buff)

#define BUFF_SIZE_4_BYTES                  4
#define BUFF_SIZE_8_BYTES                  8
#define BUFF_SIZE_10_BYTES                 10
#define BUFF_SIZE_16_BYTES                 16
#define BUFF_SIZE_32_BYTES                 32
#define BUFF_SIZE_48_BYTES                 48
#define BUFF_SIZE_64_BYTES                 64
#define BUFF_SIZE_128_BYTES                128
#define BUFF_SIZE_256_BYTES                256
#define BUFF_SIZE_512_BYTES                512
#define BUFF_SIZE_1K_BYTES                 1024
#define BUFF_SIZE_2K_BYTES                 2048
#define BUFF_SIZE_4K_BYTES                 4096
#define BUFF_SIZE_8K_BYTES                 8192

#define BUFF_SIZE_64K_BYTES                64000
#define R_MAX_SIZE                         BUFF_SIZE_64K_BYTES

#define FDO_MAX_STR_SIZE                   BUFF_SIZE_512_BYTES

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test if file exists
 *
 * @param filename
 * @return true
 * @return false
 */
bool file_exists(char const *filename);

/**
 * @brief Get file size
 *
 * @param filename
 * @return size_t
 */
size_t get_file_size(char const *filename);

/**
 * @brief Read a buffer from content of a file, non NULL-terminated.
 *
 * @param filename
 * @param buffer
 * @param size
 * @return int
 */
int read_buffer_from_file(const char *filename, uint8_t *buffer, size_t size);

/**
 * @brief Write a buffer from content of a file, non NULL-terminated.
 *
 * @param filename
 * @param buffer
 * @param size
 * @return int
 */
int write_buffer_to_file(const char *filename, uint8_t *buffer, size_t size);

/**
 * @brief Allocate a buffer and set its contents to 0 before using it.
 *
 * @param size
 * @return void*
 */
void *fdo_alloc(size_t size);

#ifdef __cplusplus
}
#endif
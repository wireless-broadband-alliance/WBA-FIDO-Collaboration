// Copyright 2024 VinCSS JSC. All rights reserved.
//
// Filename: main.c
//   Author: VanNC
//  Created: 03/12/2024 12:22:12 +07:00

#include <fcntl.h>
#ifdef TARGET_FDO_MODEL_CM5
#include <gpiod.h>
#endif
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "blob.h"
#include "fdo.h"
#include "fdoblockio.h"
#include "klibc/macro.h"
#include "load_credentials.h"
#include "safe_lib_errno.h"
#include "safe_str_lib.h"
#include "util.h"

#define ERROR_RETRY_COUNT 5

#define UCI_BIN           "/sbin/uci"
#define WIFI_BIN          "/sbin/wifi"
#define SERVICE_BIN       "/sbin/service"
#define UBUS_BIN          "/bin/ubus"
#define REBOOT_BIN        "/sbin/reboot"
#define SH_BIN            "/bin/sh"

#define FDO_ROUTER_BIN    "/etc/fdo/router.bin"
#define FDO_RESET_FILE    "/etc/fdo/reset"

#ifdef TARGET_FDO_MODEL_CM5

#define GPIO_CHIP           "/dev/gpiochip0"
#define GPIO_CONSUMER       "FDO"
#define GPIO_LINE_LED_RED   25
#define GPIO_LINE_LED_GREEN 8
#define GPIO_LINE_LED_BLUE  7
#define GPIO_LINE_BTN_RESET 12

#ifdef TARGET_FDO_MODEL_CM5
#define WIFI_DEVICE_NAME "radio0"
#else
#error "WIFI_DEVICE_NAME not defined for this target"
#endif

typedef struct gpiod_chip         gpiod_chip_t;
typedef struct gpiod_line_request gpiod_line_t;
typedef struct gpiod_line_event   gpiod_line_event_t;

static gpiod_line_t DEREF(s_gpio_line_led_red)   = NULLPTR;
static gpiod_line_t DEREF(s_gpio_line_led_green) = NULLPTR;
static gpiod_line_t DEREF(s_gpio_line_led_blue)  = NULLPTR;
static gpiod_line_t DEREF(s_gpio_line_btn_reset) = NULLPTR;

#endif

static int safe_exec(char DEREF(binary), ...);

static const int8_t b64_table[256] = {
    /*   0–15 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /*  16–31 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /*  32–47 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    /*  48–63 */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    /*  64–79 */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    /*  80–95 */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    /*  96–111 */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    /* 112–127 */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    /* 128–143 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 144–159 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 160–175 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 176–191 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 192–207 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 208–223 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 224–239 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 240–255 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t base64_decode(const char *in, uint8_t *out) {
    uint32_t buf  = 0;
    int      bits = 0;
    size_t   len  = 0;

    while (*in) {
        char c = *in++;
        if (c == '=') {
            break;
        }

        int8_t val = b64_table[(uint8_t)c];
        if (val < 0) {
            continue; // skip invalid characters (spaces, newlines)
        }

        buf = (buf << 6) | val;
        bits += 6;

        if (bits >= 8) {
            bits -= 8;
            out[len++] = (buf >> bits) & 0xFF;
        }
    }
    return len;
}

static size_t get_timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return (ts.tv_nsec / 1000000) + ts.tv_sec * 1000;
}

static fdo_sdk_service_info_module *fdo_sv_info_modules_init(void) { return NULL; }

static int fdo_error_callback(fdo_sdk_status type, fdo_sdk_error errorcode) { return FDO_SUCCESS; }

static int setup_wifi_roaming() {
    char DEREF(buff) = fdo_alloc(1024);
    int  ret         = -1;

    if (!buff) {
        LOG(LOG_ERROR, "setup_wifi_roaming() malloc failed");
        return ret;
    }

    LOG(LOG_INFO, "setup wifi roaming");

    fdo_dev_cred_t *cred = app_get_credentials();
    if (!cred || !cred->mfg_blk || !cred->mfg_blk->d) {
        LOG(LOG_ERROR, "invalid device credentials");
        return ret;
    }

    fdo_string_t *devinfo = cred->mfg_blk->d;
    HEXDUMP("device info cbor", devinfo->bytes, devinfo->byte_sz);

    uint8_t *out = (uint8_t *)fdo_alloc((devinfo->byte_sz * 3) / 4 + 4);
    size_t   len = base64_decode(devinfo->bytes, out);
    HEXDUMP("device info decoded", out, len);

    fdor_t fdor       = { 0 };
    fdor.b.block      = out;
    fdor.b.block_size = len;

    if (!fdor_parser_init(&fdor)) {
        LOG(LOG_ERROR, "FDOR Parser Initialization failed!");
        goto end;
    }

    size_t array_length = 0;
    if (!fdor_array_length(&fdor, &array_length) || array_length < 2) {
        LOG(LOG_ERROR, "Device Info read: Invalid array length, expected = 2, got = %zu", array_length);
        goto end;
    }

    if (!fdor_start_array(&fdor)) {
        LOG(LOG_ERROR, "Device Info read: Begin Array not found");
        goto end;
    }

    char domain_match[256] = { 0 };
    if (!fdor_text_string(&fdor, domain_match, sizeof(domain_match) - 1)) {
        LOG(LOG_ERROR, "Device Info read: Domain Match not found");
        goto end;
    }

    char rcois[256] = { 0 };
    if (!fdor_text_string(&fdor, rcois, sizeof(rcois) - 1)) {
        LOG(LOG_ERROR, "Device Info read: RCOIs not found");
        goto end;
    }

    LOG(LOG_INFO, "domain match: %s", domain_match);
    LOG(LOG_INFO, "rcois: %s", rcois);

    // Setting wireless
    sprintf(buff, "wireless.%s.disabled=0", WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s=wifi-iface", WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.device=%s", WIFI_DEVICE_NAME, WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.mode=sta", WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.ssid=%s", WIFI_DEVICE_NAME, "Dummy");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.encryption=wpa2", WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.disabled=0", WIFI_DEVICE_NAME);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.network=%s", WIFI_DEVICE_NAME, "wwan");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.eap_type=%s", WIFI_DEVICE_NAME, "tls");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.identity=%s", WIFI_DEVICE_NAME, "dummy@example.com");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.ca_cert=%s", WIFI_DEVICE_NAME, "/etc/fdo/server.crt");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.priv_key=%s", WIFI_DEVICE_NAME, "/etc/fdo/client.crt");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.domain_suffix_match=%s", WIFI_DEVICE_NAME, domain_match);
    safe_exec(UCI_BIN, "add_list", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.iw_enabled=%s", WIFI_DEVICE_NAME, "1");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.iw_rcois=%s", WIFI_DEVICE_NAME, rcois);
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    sprintf(buff, "wireless.default_roaming_%s.ieee80211w=%s", WIFI_DEVICE_NAME, "1");
    safe_exec(UCI_BIN, "set", buff, NULLPTR);

    // safe_exec(UCI_BIN, "commit", "network", NULLPTR);
    // safe_exec(SERVICE_BIN, "network", "reload", NULLPTR);

    safe_exec(UCI_BIN, "commit", "wireless", NULLPTR);
    safe_exec(WIFI_BIN, "reload", NULLPTR);

    ret = 0;

end:
    if (out) {
        fdo_free(out);
    }

    return ret;
}

static int fdo_init(bool resale) {
    int32_t                      ret               = -1;
    fdo_sdk_status               sdk_status        = FDO_ERROR;
    fdo_sdk_device_state         sdk_device_status = FDO_STATE_ERROR;
    fdo_sdk_service_info_module *module_info       = NULL;

    if (ret = configure_normal_blob(), ret != 0) {
        LOG(LOG_ERROR, "configure normal blob, status=%" PRId32 "", ret);
        goto end;
    }

    if (module_info = fdo_sv_info_modules_init(), module_info == NULL) {
        LOG(LOG_WARN, "no service modules loaded");
    }

    if (sdk_status = fdo_sdk_init(fdo_error_callback, FDO_MAX_MODULES, module_info), sdk_status != FDO_SUCCESS) {
        LOG(LOG_ERROR, "initialize fdo sdk, status=%d", sdk_status);
        goto end;
    }

    if (setup_wifi_roaming() != 0) {
        LOG(LOG_ERROR, "can not setup wpa_supplicant");
        return -1;
    }

    sdk_device_status = fdo_sdk_get_status();

    switch (sdk_device_status) {
    case FDO_STATE_PRE_DI:
        LOG(LOG_INFO, "device is ready for di");
        break;
    case FDO_STATE_RESALE:
        ATTRIBUTE_FALLTHROUGH;
    case FDO_STATE_PRE_TO1:
        LOG(LOG_INFO, "device is ready for ownership transfer");
        break;
    case FDO_STATE_IDLE:
        LOG(LOG_INFO, "device ownership transfer done");
        ret = -1;
        goto end;
    case FDO_STATE_ERROR:
        LOG(LOG_ERROR, "error in getting device status");
        ret = -1;
        goto end;
    default:
        ret = -1;
        goto end;
    }

    if (sdk_device_status == FDO_STATE_RESALE && resale == false) {
        ret = -1;
        goto end;
    }

    if (sdk_status = fdo_sdk_run(), sdk_status != FDO_SUCCESS) {
        LOG(LOG_ERROR, "run fdo sdk, status=%d", sdk_status);
        ret = -1;
        goto end;
    }

    ret = 0;

end:
    if (module_info) {
        free(module_info);
        module_info = NULL;
    }

    return ret;
}

static int safe_exec(char DEREF(binary), ...) {
    char    DEREF(cmd)      = fdo_alloc(1024);
    char    DEREF(args[32]) = { 0 };
    char    DEREF(it)       = NULLPTR;
    int     offset          = 0;
    va_list va;

    args[0] = binary;

    va_start(va, binary);
    for (int i = 1; it = va_arg(va, char *), it != NULL; i++) {
        offset += sprintf(cmd + offset, "%s ", it);
        args[i] = it;
    }
    va_end(va);

    args[32] = NULLPTR;

    LOG(LOG_INFO, "safe_exec: %s %s", binary, cmd);
    fdo_free(cmd);

    pid_t pid = fork();

    if (pid == 0) {
        execv(binary, args);
        return 0;
    }

    int status = -1;
    if (wait(REF(status)) >= 0 && WIFEXITED(status)) {
        goto exit;
    }

    LOG(LOG_ERROR, "safe_exec failed");

exit:
    return WEXITSTATUS(status);
}

#ifdef TARGET_FDO_MODEL_CM5
static struct gpiod_line_request *request_output_line(const char *chip_path, unsigned int offset,
                                                      enum gpiod_line_value value, const char *consumer) {
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request   *request = NULL;
    struct gpiod_line_settings  *settings;
    struct gpiod_line_config    *line_cfg;
    struct gpiod_chip           *chip;
    int                          ret;

    chip = gpiod_chip_open(chip_path);
    if (!chip)
        return NULL;

    settings = gpiod_line_settings_new();
    if (!settings)
        goto close_chip;

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, value);

    line_cfg = gpiod_line_config_new();
    if (!line_cfg)
        goto free_settings;

    ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
    if (ret)
        goto free_line_config;

    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg)
            goto free_line_config;

        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    gpiod_request_config_free(req_cfg);

free_line_config:
    gpiod_line_config_free(line_cfg);

free_settings:
    gpiod_line_settings_free(settings);

close_chip:
    gpiod_chip_close(chip);

    return request;
}

static struct gpiod_line_request *request_input_line(const char *chip_path, unsigned int offset, const char *consumer) {
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request   *request = NULL;
    struct gpiod_line_settings  *settings;
    struct gpiod_line_config    *line_cfg;
    struct gpiod_chip           *chip;
    int                          ret;

    chip = gpiod_chip_open(chip_path);
    if (!chip)
        return NULL;

    settings = gpiod_line_settings_new();
    if (!settings)
        goto close_chip;

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    line_cfg = gpiod_line_config_new();
    if (!line_cfg)
        goto free_settings;

    ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
    if (ret)
        goto free_line_config;

    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg)
            goto free_line_config;

        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    gpiod_request_config_free(req_cfg);

free_line_config:
    gpiod_line_config_free(line_cfg);

free_settings:
    gpiod_line_settings_free(settings);

close_chip:
    gpiod_chip_close(chip);

    return request;
}
#endif

static inline void reset_led() {
#if TARGET_FDO_MODEL_CM5
    gpiod_line_request_set_value(s_gpio_line_led_red, GPIO_LINE_LED_RED, 1);
    gpiod_line_request_set_value(s_gpio_line_led_green, GPIO_LINE_LED_GREEN, 1);
    gpiod_line_request_set_value(s_gpio_line_led_blue, GPIO_LINE_LED_BLUE, 1);
#else
#warning "reset_led() not implemented!"
#endif
}

static void print_usage() {
    LOG(LOG_ERROR, "command is not specified, view supported commands below");
    LOG(LOG_ERROR, "run, execute fdo on this device");
    LOG(LOG_ERROR, "reset, reset fdo on this device");
}

static int reset_command() {
    char DEREF(buff)   = NULLPTR;
    int  indicator     = 0;
    bool b_need_commit = false;

    buff = fdo_alloc(1024);

    reset_led();

#if TARGET_FDO_MODEL_CM5
    pid_t pid = fork();

    // Set red led to bink
    for (int i = 0; pid == 0; i++) {
        gpiod_line_request_set_value(s_gpio_line_led_red, GPIO_LINE_LED_RED, i % 2);
        sleep(1);
    }
#else
#warning "reset_command() running led indicator not implemented!"
#endif

    safe_exec(SH_BIN, "-c", "echo 1 > %s", FDO_RESET_FILE, NULLPTR);
    LOG(LOG_INFO, "device is reset, fdo process will be run in the next boot");

#if TARGET_FDO_MODEL_CM5
    // Set red led from blink to solid
    sleep(6);
    kill(pid, SIGKILL);
    gpiod_line_request_set_value(s_gpio_line_led_red, GPIO_LINE_LED_RED, 0);
#else
#warning "reset_command() success led indicator not implemented!"
#endif

    if (buff != NULLPTR) {
        fdo_free(buff);
    }

    return 0;
}

static int run_command() {
    char  DEREF(buff)   = NULLPTR;
    int   indicator     = 0;
    bool  b_need_commit = false;
    pid_t pid           = 0;

    struct stat st;
    if (stat(FDO_RESET_FILE, &st) == 0) {
        goto application;
    }

    buff = fdo_alloc(1024);

    reset_led();

    LOG(LOG_INFO, "device is not ready, booting fdo...");

#if TARGET_FDO_MODEL_CM5
    pid = fork();

    // Set blue led to bink
    for (int i = 0; pid == 0; i++) {
        gpiod_line_request_set_value(s_gpio_line_led_blue, GPIO_LINE_LED_BLUE, i % 2);
        sleep(1);
    }
#else
#warning "run_command() running led indicator not implemented!"
#endif

    if (pid != 0) {
        sleep(5);
    }

    if (fdo_init(false) != 0) {
#if TARGET_FDO_MODEL_CM5
        kill(pid, SIGKILL);
#endif
        if (buff != NULLPTR) {
            fdo_free(buff);
        }
        return -1;
    }

    LOG(LOG_INFO, "device is ready, rebooting...");
    // safe_exec(REBOOT_BIN, NULLPTR);

    // Set blue led from blink to solid
#if TARGET_FDO_MODEL_CM5
    kill(pid, SIGKILL);
    gpiod_line_request_set_value(s_gpio_line_led_blue, GPIO_LINE_LED_BLUE, 0);
    sleep(4);
#else
#warning "Unimplemented fdo run success indicator"
#endif

    goto end;

application:

    LOG(LOG_INFO, "device is ready, booting application...");

#if TARGET_FDO_MODEL_CM5
    {

        for (int i = 0; i < 6; i++) {
            gpiod_line_request_set_value(s_gpio_line_led_green, GPIO_LINE_LED_GREEN, i % 2);
            sleep(1);
        }

        gpiod_line_request_set_value(s_gpio_line_led_green, GPIO_LINE_LED_GREEN, 0);
        size_t start = 0;
        for (int value = 0;; value = gpiod_line_request_get_value(s_gpio_line_btn_reset, GPIO_LINE_BTN_RESET)) {
            if (value < 0) {
                LOG(LOG_ERROR, "gpiod_line_request_get_value() failed");
            } else if (value == 1) {
                sleep((int)(4 - (get_timestamp() - start)));
                LOG(LOG_INFO, "resetting device...");
                reset_led();
                sleep(4);
                reset_command();
                break;
            }

            sleep(1);
            start = get_timestamp();
        }
    }
#else
#warning "Unimplemented application run indicator"
#endif

end:
    if (buff != NULLPTR) {
        fdo_free(buff);
    }
    return 0;
}

static int setup_gpio() {
#if TARGET_FDO_MODEL_CM5
    s_gpio_line_led_red   = request_output_line(GPIO_CHIP, GPIO_LINE_LED_RED, GPIOD_LINE_VALUE_ACTIVE, GPIO_CONSUMER);
    s_gpio_line_led_green = request_output_line(GPIO_CHIP, GPIO_LINE_LED_GREEN, GPIOD_LINE_VALUE_ACTIVE, GPIO_CONSUMER);
    s_gpio_line_led_blue  = request_output_line(GPIO_CHIP, GPIO_LINE_LED_BLUE, GPIOD_LINE_VALUE_ACTIVE, GPIO_CONSUMER);
    s_gpio_line_btn_reset = request_input_line(GPIO_CHIP, GPIO_LINE_BTN_RESET, GPIO_CONSUMER);
#else
#warning "setup_gpio() not implemented!"
#endif

    return 0;
}

static void release_gpio() {
#if TARGET_FDO_MODEL_CM5
    if (s_gpio_line_led_red != NULLPTR) {
        gpiod_line_request_release(s_gpio_line_led_red);
    }
    if (s_gpio_line_led_green != NULLPTR) {
        gpiod_line_request_release(s_gpio_line_led_green);
    }
    if (s_gpio_line_led_blue != NULLPTR) {
        gpiod_line_request_release(s_gpio_line_led_blue);
    }
    if (s_gpio_line_btn_reset != NULLPTR) {
        gpiod_line_request_release(s_gpio_line_btn_reset);
    }
#else
#warning "release_gpio() not implemented!"
#endif
}

int main(int argc, char **argv) {
    char   DEREF(cmd)   = NULLPTR;
    char   DEREF(buff)  = NULLPTR;
    size_t n_length_cmd = 0;
    int    indicator    = 0;
    int    ret          = 1;

    KLIBC_LOG_INIT(get_timestamp, printf);

    if (argc != 2) {
        goto usage;
    }

    if (setup_gpio() != 0) {
        LOG(LOG_ERROR, "can not setup gpio");
        goto end;
    }

    // Handle commands
    cmd          = argv[1];
    n_length_cmd = strlen(cmd);
    if (strcmp_s(cmd, n_length_cmd, "reset", REF(indicator)) == EOK && indicator == 0) {
        ret = reset_command();
        goto end;
    }
    if (strcmp_s(cmd, n_length_cmd, "run", REF(indicator)) == EOK && indicator == 0) {
        if (ret = run_command(), ret == 0) {
            safe_exec(REBOOT_BIN, NULLPTR);
        }
        goto end;
    }

usage:
    print_usage();
    return 2;

end:
    release_gpio();
    return ret;
}
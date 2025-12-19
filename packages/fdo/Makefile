include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config

PKG_NAME:=fdo
PKG_VERSION:=1.0
PKG_RELEASE:=1

SOURCE_DIR:=$(shell pwd)


# PKG_BUILD_DEPENDS := libmbedtls

include $(INCLUDE_DIR)/version.mk
include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/fdo
	SECTION:=examples
	CATEGORY:=FDO
	DEPENDS:=+libmbedtls
	DEPENDS+=+libgpiod
	TITLE:=FIDO Device Onboard
	MENU:=1
endef

define Package/fdo/config
	source "$(SOURCE)/Config.in"
endef

# Package description; a more verbose description on what our package does
define Package/fdo/description
	FDO Client CLI
endef

# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -r $(SOURCE_DIR)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

TARGET_CFLAGS+=-ggdb3

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable
define Build/Compile

	$(eval TARGET_CFLAGS += -DTARGET_FDO_MODEL_$(TARGET_FDO_MODEL))

	$(warning Building for target $(TARGET_FDO_MODEL) ...)

	$(warning Building klibc lib ...)
	
	$(eval KLIBC_INCLUDE_DIR := -I$(PKG_BUILD_DIR)/third_party/klibc/include)
	$(TARGET_CC) $(TARGET_CFLAGS) $(KLIBC_INCLUDE_DIR) -o $(PKG_BUILD_DIR)/third_party/klibc/src/log.o -c $(PKG_BUILD_DIR)/third_party/klibc/src/log.c

	$(warning Building safestring lib ...)

	$(eval SAFESTRING_INCLUDE_DIR := -I$(PKG_BUILD_DIR)/third_party/safestringlib/include)
	$(eval SAFESTRING_SOURCE_DIR := $(PKG_BUILD_DIR)/third_party/safestringlib/safeclib)
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/safe_str_constraint.o -c $(SAFESTRING_SOURCE_DIR)/safe_str_constraint.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/safe_mem_constraint.o -c $(SAFESTRING_SOURCE_DIR)/safe_mem_constraint.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/ignore_handler_s.o -c $(SAFESTRING_SOURCE_DIR)/ignore_handler_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/mem_primitives_lib.o -c $(SAFESTRING_SOURCE_DIR)/mem_primitives_lib.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strcmp_s.o -c $(SAFESTRING_SOURCE_DIR)/strcmp_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strcat_s.o -c $(SAFESTRING_SOURCE_DIR)/strcat_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strcasecmp_s.o -c $(SAFESTRING_SOURCE_DIR)/strcasecmp_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strnlen_s.o -c $(SAFESTRING_SOURCE_DIR)/strnlen_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/memset_s.o -c $(SAFESTRING_SOURCE_DIR)/memset_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/memcpy_s.o -c $(SAFESTRING_SOURCE_DIR)/memcpy_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/memcmp_s.o -c $(SAFESTRING_SOURCE_DIR)/memcmp_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/memmove_s.o -c $(SAFESTRING_SOURCE_DIR)/memmove_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strcpy_s.o -c $(SAFESTRING_SOURCE_DIR)/strcpy_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strstr_s.o -c $(SAFESTRING_SOURCE_DIR)/strstr_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strncpy_s.o -c $(SAFESTRING_SOURCE_DIR)/strncpy_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/strncat_s.o -c $(SAFESTRING_SOURCE_DIR)/strncat_s.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) -o $(SAFESTRING_SOURCE_DIR)/snprintf_support.o -c $(SAFESTRING_SOURCE_DIR)/snprintf_support.c

	$(warning Building tinycbor ...)

	$(eval TINYCBOR_INCLUDE_DIR := -I$(PKG_BUILD_DIR)/third_party/tinycbor/src)
	$(eval TINYCBOR_SOURCE_DIR := $(PKG_BUILD_DIR)/third_party/tinycbor/src)
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborerrorstrings.o -c $(TINYCBOR_SOURCE_DIR)/cborerrorstrings.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborencoder.o -c $(TINYCBOR_SOURCE_DIR)/cborencoder.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborencoder_close_container_checked.o -c $(TINYCBOR_SOURCE_DIR)/cborencoder_close_container_checked.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborencoder_float.o -c $(TINYCBOR_SOURCE_DIR)/cborencoder_float.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborparser.o -c $(TINYCBOR_SOURCE_DIR)/cborparser.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborparser_float.o -c $(TINYCBOR_SOURCE_DIR)/cborparser_float.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborpretty.o -c $(TINYCBOR_SOURCE_DIR)/cborpretty.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TINYCBOR_INCLUDE_DIR) -o $(TINYCBOR_SOURCE_DIR)/cborvalidation.o -c $(TINYCBOR_SOURCE_DIR)/cborvalidation.c

	$(warning Building FDO SDK ...)

	$(eval FDO_SDK_INCLUDE_DIR := -I$(PKG_BUILD_DIR))
	$(eval FDO_SDK_SOURCE_DIR := $(PKG_BUILD_DIR)/third_party/client-sdk-fidoiot)

	$(eval FDO_SDK_INCLUDE_DIR += $(KLIBC_INCLUDE_DIR))
	$(eval FDO_SDK_INCLUDE_DIR += $(SAFESTRING_INCLUDE_DIR))
	$(eval FDO_SDK_INCLUDE_DIR += $(TINYCBOR_INCLUDE_DIR))
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/include)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/lib/include)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/network/include)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/storage/include)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/crypto/include)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/crypto/ecdsa)
	$(eval FDO_SDK_INCLUDE_DIR += -I$(FDO_SDK_SOURCE_DIR)/app/include)


	$(eval FDO_SDK_CFLAGS := -DTARGET_OS_LINUX=1)
	$(eval FDO_SDK_CFLAGS += -DTARGET_OS_OPENWRT=1)
	$(eval FDO_SDK_CFLAGS += -DARCH='"$(ARCH)"')
	$(eval FDO_SDK_CFLAGS += -DBIN_TYPE='"$(ARCH)"')
	$(eval FDO_SDK_CFLAGS += -DOS_VERSION='"$(VERSION_NUMBER)$(VERSION_CODE)"')
	$(eval FDO_SDK_CFLAGS += -DFDO_LOG_LEVEL_ALL=1)
	$(if $(CONFIG_FDO_LOG_AES_DEBUG),$(eval FDO_SDK_CFLAGS += -DAES_DEBUG=1))

	$(eval FDO_SDK_CFLAGS += -DAES_MODE_GCM_ENABLED=1)
	$(eval FDO_SDK_CFLAGS += -DUSE_MBEDTLS=1)
	
# 	$(eval FDO_SDK_CFLAGS += -DMBEDTLS_VERSIONN_2=0)
 	$(eval FDO_SDK_CFLAGS += -DMBEDTLS_ECP_RESTARTABLE=1)
	$(eval FDO_SDK_CFLAGS += -D'MBEDTLS_PRIVATE(member)=member')

	$(eval FDO_SDK_CFLAGS += -DFDO_CRED_NORMAL='$(CONFIG_FDO_RAW_CREDENTIAL)')
	$(eval FDO_SDK_CFLAGS += -DFDO_CRED_SECURE='$(CONFIG_FDO_SECURE_CREDENTIAL)')
	$(eval FDO_SDK_CFLAGS += -DSERIAL_FILE='$(CONFIG_FDO_SERIAL_NUMBER)')
	$(eval FDO_SDK_CFLAGS += -DMODEL_FILE='$(CONFIG_FDO_MODEL_NUMBER)')
	$(eval FDO_SDK_CFLAGS += -DMAX_SERVICEINFO_SZ_FILE='$(CONFIG_FDO_DEVICE_MAX_SERVICEINFO)')
	$(eval FDO_SDK_CFLAGS += -DPLATFORM_IV='$(CONFIG_FDO_DEVICE_IV)')
	$(eval FDO_SDK_CFLAGS += -DPLATFORM_HMAC_KEY='$(CONFIG_FDO_DEVICE_HMAC)')
	$(eval FDO_SDK_CFLAGS += -DPLATFORM_AES_KEY='$(CONFIG_FDO_DEVICE_AES)')
	$(eval FDO_SDK_CFLAGS += -DECDSA_PRIVKEY='$(CONFIG_FDO_DEVICE_ESCDA)')
	$(eval FDO_SDK_CFLAGS += -DMANUFACTURER_ADDR='"https://127.0.0.1:8009/fdo/di"')
	$(eval FDO_SDK_CFLAGS += -DRESALE_SUPPORTED=1)
	$(eval FDO_SDK_CFLAGS += -DREUSE_SUPPORTED=1)
	$(eval FDO_SDK_CFLAGS += -DECDSA384_DA=1)
	$(eval FDO_SDK_CFLAGS += -DECDSA_PEM=$(CONFIG_FDO_ECDSA_PRIVATE_KEY_ENCODING_PEM))
	
	$(eval FDO_SDK_CFLAGS += $(FDO_SDK_INCLUDE_DIR))

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/credentials_from_file.o -c $(FDO_SDK_SOURCE_DIR)/lib/credentials_from_file.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/crypto_utils.o -c $(FDO_SDK_SOURCE_DIR)/lib/crypto_utils.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdo.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdo.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdoblockio.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdoblockio.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdocred.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdocred.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdonet.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdonet.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdoprot.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdoprot.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdoprotctx.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdoprotctx.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/fdotypes.o -c $(FDO_SDK_SOURCE_DIR)/lib/fdotypes.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/m-string.o -c $(FDO_SDK_SOURCE_DIR)/lib/m-string.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoAes.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoAes.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoCryptoCommon.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoCryptoCommon.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevAttest.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevAttest.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevSign.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevSign.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoHmac.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoHmac.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoKeyExchange.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoKeyExchange.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoOvVerify.o -c $(FDO_SDK_SOURCE_DIR)/crypto/common/fdoOvVerify.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_AESRoutines.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_AESRoutines.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_cryptoSupport.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_cryptoSupport.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_DERRoutines.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_DERRoutines.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ec_csr.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ec_csr.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSASignRoutines.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSASignRoutines.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSAVerifyRoutines.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSAVerifyRoutines.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_key_exchange_ecdh.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_key_exchange_ecdh.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_random.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_random.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_SSLRoutines.o -c $(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_SSLRoutines.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/crypto/ecdsa/ecdsa_privkey.o -c $(FDO_SDK_SOURCE_DIR)/crypto/ecdsa/ecdsa_privkey.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/storage/linux/storage_if_linux.o -c $(FDO_SDK_SOURCE_DIR)/storage/linux/storage_if_linux.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/storage/linux/platform_utils_if_linux.o -c $(FDO_SDK_SOURCE_DIR)/storage/linux/platform_utils_if_linux.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/network/network_if_linux.o -c $(FDO_SDK_SOURCE_DIR)/network/network_if_linux.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/network/rest_interface.o -c $(FDO_SDK_SOURCE_DIR)/network/rest_interface.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg10.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg10.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg11.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg11.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg12.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg12.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg13.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg13.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg30.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg30.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg31.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg31.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg32.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg32.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg33.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg33.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg60.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg60.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg61.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg61.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg62.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg62.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg63.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg63.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg64.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg64.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg65.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg65.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg66.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg66.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg67.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg67.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg68.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg68.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg69.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg69.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg70.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg70.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg71.o -c $(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg71.c

	$(TARGET_CC) $(TARGET_CFLAGS) $(FDO_SDK_CFLAGS) -o $(FDO_SDK_SOURCE_DIR)/app/blob.o -c $(FDO_SDK_SOURCE_DIR)/app/blob.c

	@echo Building FDO package ...

	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) $(FDO_SDK_INCLUDE_DIR) -o $(PKG_BUILD_DIR)/main.o -c $(PKG_BUILD_DIR)/main.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(SAFESTRING_INCLUDE_DIR) $(KLIBC_INCLUDE_DIR) -o $(PKG_BUILD_DIR)/util.o -c $(PKG_BUILD_DIR)/util.c

	$(TARGET_CC) $(TARGET_LDFLAGS) -lmbedtls -lmbedx509 -lmbedcrypto -lrt -lgpiod -lm -o $(PKG_BUILD_DIR)/$1 \
		$(PKG_BUILD_DIR)/main.o \
		$(PKG_BUILD_DIR)/util.o \
		\
		$(PKG_BUILD_DIR)/third_party/klibc/src/log.o \
		\
		$(SAFESTRING_SOURCE_DIR)/safe_str_constraint.o \
		$(SAFESTRING_SOURCE_DIR)/safe_mem_constraint.o \
		$(SAFESTRING_SOURCE_DIR)/ignore_handler_s.o \
		$(SAFESTRING_SOURCE_DIR)/mem_primitives_lib.o \
		$(SAFESTRING_SOURCE_DIR)/strcmp_s.o \
		$(SAFESTRING_SOURCE_DIR)/strcat_s.o \
		$(SAFESTRING_SOURCE_DIR)/strcasecmp_s.o \
		$(SAFESTRING_SOURCE_DIR)/strnlen_s.o \
		$(SAFESTRING_SOURCE_DIR)/memset_s.o \
		$(SAFESTRING_SOURCE_DIR)/memcpy_s.o \
		$(SAFESTRING_SOURCE_DIR)/memcmp_s.o \
		$(SAFESTRING_SOURCE_DIR)/memmove_s.o \
		$(SAFESTRING_SOURCE_DIR)/strcpy_s.o \
		$(SAFESTRING_SOURCE_DIR)/strstr_s.o \
		$(SAFESTRING_SOURCE_DIR)/strncpy_s.o \
		$(SAFESTRING_SOURCE_DIR)/strncat_s.o \
		$(SAFESTRING_SOURCE_DIR)/snprintf_support.o \
		\
		$(TINYCBOR_SOURCE_DIR)/cborerrorstrings.o \
		$(TINYCBOR_SOURCE_DIR)/cborencoder.o \
		$(TINYCBOR_SOURCE_DIR)/cborencoder_close_container_checked.o \
		$(TINYCBOR_SOURCE_DIR)/cborencoder_float.o \
		$(TINYCBOR_SOURCE_DIR)/cborparser.o \
		$(TINYCBOR_SOURCE_DIR)/cborparser_float.o \
		$(TINYCBOR_SOURCE_DIR)/cborpretty.o \
		$(TINYCBOR_SOURCE_DIR)/cborvalidation.o \
		\
		$(FDO_SDK_SOURCE_DIR)/lib/credentials_from_file.o \
		$(FDO_SDK_SOURCE_DIR)/lib/crypto_utils.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdo.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdoblockio.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdocred.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdonet.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdoprot.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdoprotctx.o \
		$(FDO_SDK_SOURCE_DIR)/lib/fdotypes.o \
		$(FDO_SDK_SOURCE_DIR)/lib/m-string.o \
		\
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoAes.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoCryptoCommon.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevAttest.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoDevSign.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoHmac.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoKeyExchange.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/common/fdoOvVerify.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_AESRoutines.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_cryptoSupport.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_DERRoutines.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ec_csr.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSASignRoutines.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_ECDSAVerifyRoutines.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_key_exchange_ecdh.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_random.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/mbedtls/mbedtls_SSLRoutines.o \
		$(FDO_SDK_SOURCE_DIR)/crypto/ecdsa/ecdsa_privkey.o \
		\
		$(FDO_SDK_SOURCE_DIR)/storage/linux/storage_if_linux.o \
		$(FDO_SDK_SOURCE_DIR)/storage/linux/platform_utils_if_linux.o \
		\
		$(FDO_SDK_SOURCE_DIR)/network/network_if_linux.o \
		$(FDO_SDK_SOURCE_DIR)/network/rest_interface.o \
		\
		$(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg10.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg11.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg12.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/di/msg13.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg30.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg31.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg32.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to1/msg33.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg60.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg61.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg62.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg63.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg64.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg65.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg66.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg67.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg68.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg69.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg70.o \
		$(FDO_SDK_SOURCE_DIR)/lib/prot/to2/msg71.o \
		\
		$(FDO_SDK_SOURCE_DIR)/app/blob.o

endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/fdo/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fdo $(1)/usr/bin/
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,fdo))
# OpenWRT FDO Package Proof of Concept (PoC)

This document outlines the build process, installation, usage, and configuration for the FDO (FIDO Device Onboarding) OpenWRT package. This PoC is currently targeted and tested on the **Raspberry Pi CM5**.

## Build Guide

This project is structured as an OpenWRT package. To build it, you will need an OpenWRT build environment (SDK or ImageBuilder) compatible with your target hardware (Raspberry Pi CM5).

### 1. Prepare Build Environment
Ensure you have the OpenWRT build system set up.

### 2. Add Package Feed
Add this package to your OpenWRT feeds.

### 3. Apply Patches
Before compilation, apply the necessary patch files included in the repository:

```sh
git apply --check ./packages/fdo/client-sdk-fidoiot.patch
git apply --stat ./packages/fdo/client-sdk-fidoiot.patch
git apply ./packages/fdo/client-sdk-fidoiot.patch
```

### 4. Compile
Run the build command for the package:
```bash
make package/fdo/compile
```

## Installation & Service Setup

The package installs the `fdo` binary. To run it automatically, set it up as a system service.

### Service Configuration
Create an init script at `/etc/init.d/fdo` to manage the service.

```sh
#!/bin/sh /etc/rc.common

START=99
USE_PROCD=1

start_service() {
    procd_open_instance
    procd_set_param command /usr/bin/fdo run
    procd_set_param respawn
    procd_close_instance
}
```

Enable and start the service:
```bash
/etc/init.d/fdo enable
/etc/init.d/fdo start
```

## Usage

The `fdo` binary supports two primary commands:

### 1. Run FDO Process
Starts the FDO process. This is typically handled by the service but can be run manually.
```bash
fdo run
```

### 2. Reset Device
Resets the device state (e.g., triggers a factory reset logic or FDO reset). This can be mapped to a physical button interrupt.
```bash
fdo reset
```

## Hardware & LED Indicators (Raspberry Pi CM5)

This package includes hardware integration for LED status indicators and a physical reset button.

**GPIO Mapping (CM5):**
- **Chip**: `/dev/gpiochip0`
- **Red LED**: GPIO 25
- **Green LED**: GPIO 8
- **Blue LED**: GPIO 7
- **Reset Button**: GPIO 12

**LED Status Indicators:**
- **FDO Running**: Blue LED blinks.
- **FDO Success & Waiting For Reboot**: Blue LED turns solid.
- **Device Running Main Application**: Green LED blinks (~6s) then turns solid.
- **FDO Reseting**: Red LED blinks during reset, then turns solid upon completion.

**Physical Reset:**
Holding the **Reset Button (GPIO 12)** will trigger the `fdo reset` command.

## Device Info

The main function that processes the `devinfo` is [`setup_wifi_roaming()`](packages/fdo/main.c).

### Format
The data is a **CBOR** encoded array, then **Base64** encoded.

**Structure:**
```python
[
    'domain_suffix_match',  # String: Domain to match
    'rcoi1,rcoi2',          # String: Comma-separated RCOIs
    # ... additional config items
]
```

### Encoding Example (Python)
Use the following Python snippet to generate the encoded `devinfo` string:

```python
import cbor2
import base64

# Define your device info
devinfo_data = [
    'example.com',       # domain_suffix_match
    'rcoi1,rcoi2',       # RCOIs
    # Add more device info items as needed
]

# Encode to CBOR
cbor_data = cbor2.dumps(devinfo_data)

# Encode to Base64 (UTF-8 string)
encoded_devinfo = base64.b64encode(cbor_data).decode('utf-8')

print(encoded_devinfo)
```

### EAP-TLS & Certificates
Currently, the EAP-TLS client certificate and private key are placed alongside the device credential files for ease of debugging.
*Note: In the future, these certificates can be encoded directly into the `devinfo` blob.*

### WiFi Roaming Config
Extra configuration parameters can be added to the `devinfo` array to setup WiFi roaming profiles. This area is under active discussion and improvement.

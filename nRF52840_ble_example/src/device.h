#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include "../../ndn-lite/app-support/bootstrapping/secure-sign-on/sign-on-basic-client-consts.h"
#include "../../ndn-lite/app-support/bootstrapping/secure-sign-on/variants/ecc_256/sign-on-basic-ecc-256-consts.h"

#define BOARD_1
//#define BOARD_2

extern const uint8_t DEVICE_IDENTIFIER[SIGN_ON_BASIC_CLIENT_DEVICE_IDENTIFIER_MAX_LENGTH];

extern const uint8_t DEVICE_CAPABILITIES[SIGN_ON_BASIC_CLIENT_DEVICE_CAPABILITIES_MAX_LENGTH];

// these are the raw 32 bytes of the bootstrapping ecc private key (raw format is the format
// used and output by the micro-ecc library)
extern const uint8_t BOOTSTRAP_ECC_PRIVATE[32];

// these are the raw key bytes of the ecc public key without
// the point identifier
extern const uint8_t BOOTSTRAP_ECC_PUBLIC_NO_POINT_IDENTIFIER[64];

extern const uint8_t SECURE_SIGN_ON_CODE[SIGN_ON_BASIC_ECC_256_SECURE_SIGN_ON_CODE_LENGTH];

#ifdef BOARD_1
static char CMD_CHANGE_POLICY[] = "/NDN-IoT/TrustChange/Board1";
static char CMD_LED[] = "/NDN-IoT/Board1";
static char CMD_LED_BLINK[] = "/NDN-IoT/Board2/LED/BLINK";
#endif
#ifdef BOARD_2
static char CMD_CHANGE_POLICY[] = "/NDN-IoT/TrustChange/Board2";
static char CMD_LED[] = "/NDN-IoT/Board2";
static char CMD_LED_BLINK[] = "/NDN-IoT/Board1/LED/BLINK";
#endif


#endif

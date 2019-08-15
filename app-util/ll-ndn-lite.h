#ifndef LL_NDN_LITE_H
#define LL_NDN_LITE_H

#include "../ndn-lite/encode/data.h"
#include "../ndn-lite/encode/encoder.h"
#include "../ndn-lite/encode/interest.h"
#include "../ndn-lite/face/direct-face.h"
#include "../ndn-lite/face/ndn-nrf-ble-face.h"
#include "../ndn-lite/forwarder/forwarder.h"

#include "../ndn-lite/security/ndn-lite-sec-utils.h"
#include "../../ndn-lite/app-support/bootstrapping.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define APP_LOG(...) printf(__VA_ARGS__)

void APP_LOG_HEX(const char *msg, const uint8_t *buf, size_t buf_len) {
  APP_LOG(msg);
  APP_LOG("\n");
  int i;
  for (i = 0; i < buf_len; i++)
  {
    //if (i > 0) APP_LOG(":");
    APP_LOG("%02X", buf[i]);
  }
  APP_LOG("\n");
}

#define MAX_COMMAND_STR_LEN 256

// defines for ndn standalone library
ndn_direct_face_t *m_face;
uint16_t m_face_id_direct = 2;
uint16_t m_face_id_ble = 3;
ndn_nrf_ble_face_t *m_ndn_nrf_ble_face;

// Callback for when sign on has completed.
void m_on_sign_on_completed_callback(int result_code) {
  APP_LOG("in main, m_on_sign_on_completed_callback got called.\n");
  blink_led(5);

  if (result_code == NDN_SUCCESS) {
    APP_LOG("Sign on completed succesfully.\n");

    APP_LOG("Value of KD pri after completing sign on:\n");
    for (int i = 0; i < get_sign_on_basic_client_nrf_sdk_ble_instance()->KD_pri_len; i++) {
      APP_LOG("%02x", get_sign_on_basic_client_nrf_sdk_ble_instance()->KD_pri_p[i]);
    }
    APP_LOG("\n");

    APP_LOG("Value of KD pub cert after completing sign on:\n");
    for (int i = 0; i < get_sign_on_basic_client_nrf_sdk_ble_instance()->KD_pub_cert_len; i++) {
      APP_LOG("%02x", get_sign_on_basic_client_nrf_sdk_ble_instance()->KD_pub_cert_p[i]);
    }
    APP_LOG("\n");

    APP_LOG("Value of trust anchor cert after completing sign on:\n");
    for (int i = 0; i < get_sign_on_basic_client_nrf_sdk_ble_instance()->trust_anchor_cert_len; i++) {
      APP_LOG("%02x", get_sign_on_basic_client_nrf_sdk_ble_instance()->trust_anchor_cert_p[i]);
    }
    APP_LOG("\n");

  } else {
    APP_LOG("Sign on failed, error code: %d\n", result_code);
  }
}

void ndn_lite_init() {
    ndn_security_init();

    blink_led(3); // start sign-on
  // Initialize the sign on client.
  sign_on_basic_client_nrf_sdk_ble_construct(
      SIGN_ON_BASIC_VARIANT_ECC_256,
      DEVICE_IDENTIFIER, sizeof(DEVICE_IDENTIFIER), //this is array
      DEVICE_CAPABILITIES, sizeof(DEVICE_CAPABILITIES),// 0x05
      SECURE_SIGN_ON_CODE,
      BOOTSTRAP_ECC_PUBLIC_NO_POINT_IDENTIFIER, sizeof(BOOTSTRAP_ECC_PUBLIC_NO_POINT_IDENTIFIER),
      BOOTSTRAP_ECC_PRIVATE, sizeof(BOOTSTRAP_ECC_PRIVATE),
      m_on_sign_on_completed_callback);

  // Initialize the ndn lite forwarder
  ndn_forwarder_init();

  // Create a direct face, which we will use to send the interest for our certificate after sign on.
  m_face = ndn_direct_face_construct(m_face_id_direct);
}

#define NDN_CONSTRUCT_NRF_BLE_FACE(_face) {			\
	_face = ndn_nrf_ble_face_construct(m_face_id_ble);	\
	_face->intf.state = NDN_FACE_STATE_UP;			\
    }

#define NDN_FIB_INSERT(_name, _string, _face) {			\
	ndn_name_from_string(&_name, _string, strlen(_string));\
	ndn_forwarder_fib_insert(&_name, &_face->intf, 0);     \
    }

#define NDN_REGISTER_PREFIX(_prefix, _string, _cb) {			\
	ndn_name_from_string(&_prefix, _string, strlen(_string));\
	ndn_direct_face_register_prefix(&_prefix, _cb);		 \
    }

#define NDN_MAKE_AND_SEND_COMMAND(_name, _face, _dcb, _tcb) {		\
	ndn_interest_t interest;					\
	ndn_interest_init(&interest);					\
	ndn_name_from_string(&interest.name, _name, strlen(_name));	\
	uint8_t interest_block[256] = {0};				\
	ndn_encoder_t encoder;						\
	encoder_init(&encoder, interest_block, 256);			\
	ndn_interest_tlv_encode(&encoder, &interest);			\
	ndn_direct_face_express_interest(&interest.name,		\
					 interest_block, encoder.offset, \
					 _dcb, _tcb);			\
	ndn_face_send(&_face->intf, &interest.name,			\
		      interest_block, encoder.offset);			\
	nrf_delay_ms(100);						\
    }

int ndn_determine_command_operation(ndn_interest_t* di, const char* prefix,
				    char ops[][MAX_COMMAND_STR_LEN], int num) {
    int op = 0;
    for (int i = 0; i < num; ++ i) {
	char str[MAX_COMMAND_STR_LEN]; int k = 0; int len = strlen(prefix);
	for (int j = 0; j < len; ++ j) str[k ++] = prefix[j];

	len = strlen(ops[i]);
	for (int j = 0; j < len; ++ j) str[k ++] = ops[i][j];
	
	str[k] = 0;

	ndn_name_t name;
	ndn_name_from_string(&name, str, strlen(str));
	if (ndn_name_compare(&di->name, &name) == 0) {
	    op = i + 1;
	}	  
    }
    return op;
}

int ndn_validate_command(ndn_interest_t* interest, const char* prefix,
			 int trust_controller_only) {

    if (trust_controller_only == 0) return 1;
    
    char controller_ops [][MAX_COMMAND_STR_LEN] = {"/LED/ON", "/LED/OFF",
						   "/ControllerOnly", "/AllNode"};
    int op = ndn_determine_command_operation(interest, prefix, controller_ops, 4);
    return op > 0;
}

#endif

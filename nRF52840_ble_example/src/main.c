#include "device.h"
#include "app-util/ll-ndn-lite.h"
#include "app-util/ll-nrf.h"

static uint8_t trust_controller_only = 0;

int on_policy_command(const uint8_t *interest, uint32_t interest_size)
{
  APP_LOG("Get into on_policy_command... Start to decode received Interest\n");
  ndn_interest_t decoded_interest;
  int ret = ndn_interest_from_block(&decoded_interest, interest, interest_size);

  if (ndn_validate_command(&decoded_interest, CMD_CHANGE_POLICY, trust_controller_only)) {
      char ops[2][128] = {"/ControllerOnly", "/AllNode"};
      int op = ndn_determine_command_operation(&decoded_interest, CMD_CHANGE_POLICY, ops, 2);
      if (op == 1) {
	  trust_controller_only = 1;
	  blink_led(4);      
      } else if (op == 2) {
	  trust_controller_only = 0;
	  blink_led(4);
      }
  }

  return ret;
}

int on_led_command(const uint8_t *interest, uint32_t interest_size)
{
  APP_LOG("Get on_led_command... Start to decode received Interest\n");
  ndn_interest_t decoded_interest;
  int ret = ndn_interest_from_block(&decoded_interest, interest, interest_size);
  
  if (ndn_validate_command(&decoded_interest, CMD_LED, trust_controller_only)) {
      char ops[][MAX_COMMAND_STR_LEN] = {"/LED/BLINK", "/LED/ON", "/LED/OFF"};
      int op = ndn_determine_command_operation(&decoded_interest, CMD_LED, ops, 3);
      APP_LOG("op = %d\n", op);
      
      if (op == 1) {
	  blink_led(1);
      } else if (op == 2) {
	  on_led(1);
      } else if (op == 3) {
	  off_led(1);
      }
  }

  return ret;
}

// timeout: blink the led
int on_interest_timeout_callback(const uint8_t *interest, uint32_t interest_size) {
    APP_LOG("on_timeout_callback");
    blink_led(2);
    return 0;
}
// data back: do nothing
int on_data_callback(const uint8_t *data, uint32_t data_size) {
    APP_LOG("on_data_callback");
    return 0;
}

int main(void) {
    // initialize the system
    nrf_board_init(); // inlcuding crpto sub-system
    ndn_lite_init(); // including security bootstrapping

    // declare all the names would be used
    ndn_name_t cert_fetch_prefix;
    ndn_name_t general_request_prefix;
    ndn_name_t policy_cmd_name;
    ndn_name_t led_cmd_name;

    // construct a ble face
    NDN_CONSTRUCT_NRF_BLE_FACE(m_ndn_nrf_ble_face);

    // consumer section: add routes to push outgoing requests to the ble face
    NDN_FIB_INSERT(cert_fetch_prefix, "/sign-on/cert", m_ndn_nrf_ble_face);
    NDN_FIB_INSERT(general_request_prefix, "/NDN-IoT", m_ndn_nrf_ble_face);

    // producer section: add routes to pull incoming requests to the callback function
    NDN_REGISTER_PREFIX(policy_cmd_name, CMD_CHANGE_POLICY, on_policy_command);
    NDN_REGISTER_PREFIX(led_cmd_name, CMD_LED, on_led_command);

    // the main loop: wait for device operations
    for (;;) {
	// if button 3 is pressed send a command to light the LED on another board
	if (nrf_gpio_pin_read(BUTTON_3) == 0)
	    NDN_MAKE_AND_SEND_COMMAND(CMD_LED_BLINK, m_ndn_nrf_ble_face,
				      on_data_callback, on_interest_timeout_callback);
    }

    return 0;
}

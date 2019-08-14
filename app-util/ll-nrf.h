#ifndef LL_NRF_H
#define LL_NRF_H

#include "nrf_delay.h" //For basic delaying functions
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "pca10056.h" //GPIO definitions for the nRF52840-DK (aka pca10056)

// Corresponds to LED2 on the development kit
#define LIGHTBULB_LED                   BSP_BOARD_LED_1                         /**< led to be toggled with the help of the secure_sign_on Button Service. */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "fds.h"
#include "sensorsim.h"

#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "bsp_btn_ble.h"

//
//// Includes I added for the secure-sign-on implementation
////**************************************//
//#include "transport/nrf-sdk-ble-ndn-lite-ble-unicast-transport/nrf-sdk-ble-ndn-lite-ble-unicast-service.h"
//#include <uECC.h>
////**************************************//

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();// app_timer_init() this function define in app_timer.c in nRF_Libray
    APP_ERROR_CHECK(err_code);

    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       ret_code_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

static const uint32_t led = NRF_GPIO_PIN_MAP(0, 13);

static const nrfx_gpiote_out_config_t led_config = {
    .init_state = GPIOTE_CONFIG_OUTINIT_Low,
    .task_pin = false};

static void nop(void) {
  __asm__ __volatile__("nop" ::
                           :);
}

// led blink function
// param i: led number , for example, i=1 ,so led1 in board will blink
static void blink_led(int i) {
  const uint32_t pin = NRF_GPIO_PIN_MAP(0, 12 + i); // LED
  nrf_gpio_cfg_output(pin);

  int counter = 0;
  while (counter < 6) {
    nrf_gpio_pin_toggle(pin);
    for (uint32_t i = 0; i < 0x320000; ++i)
      nop();
    counter++;
  }
}

// add led1 on code
static void on_led(int i){
 const uint32_t pin = NRF_GPIO_PIN_MAP(0, 12 + i);
 nrf_gpio_cfg_output(pin);
 nrf_gpio_pin_write(pin, 0);
}

// add led1 off code
static void off_led(int i){
 const uint32_t pin = NRF_GPIO_PIN_MAP(0, 12 + i);
 nrf_gpio_cfg_output(pin);
 nrf_gpio_pin_write(pin, 1);
}

static void nrf_board_init(void) {
  //initialize the button and LED
  nrf_gpio_cfg_output(BSP_LED_0);                    //BSP_LED_0 is pin 13 in the nRF52840-DK. Configure pin 13 as standard output.
  nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP); // Configure pin 11 as standard input with a pull up resister.
  nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP); // Configure pin 12 as standard input with a pull up resister.
  nrf_gpio_cfg_input(BUTTON_3, NRF_GPIO_PIN_PULLUP); // Configure pin 12 as standard input with a pull up resister.
  nrf_gpio_pin_write(BSP_LED_0, 1);                  // Turn off LED1 (Active Low)

  // Initialize the log.
  log_init();

  // Initialize timers.
  timers_init();

  // Initialize power management.
  power_management_init();

  ret_code_t err_code;

  // Initialize the crypto subsystem
  err_code = nrf_crypto_init();
  APP_ERROR_CHECK(err_code);
}

#endif

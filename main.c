/*
 ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*
 * Blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static __attribute__((noreturn)) THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_OPENDRAIN);
  while (true) {
    systime_t time = 250;
    //palClearPad(GPIOB, GPIOB_LED);
    palClearPad(GPIOC, 13);
    chThdSleepMilliseconds(time);
    palSetPad(GPIOC, 13);
    chThdSleepMilliseconds(time);
  }
}

/**
 * Serial Thread
 * Perform echo functionality
 */

THD_WORKING_AREA(waThread2, 128);
static __attribute__((noreturn)) THD_FUNCTION(Thread2,arg) {
  (void)arg;
  while (true) {
    if (SD1.state == SD_READY) {
      sdPut(&SD1, sdGet(&SD1));
    }
    chThdSleep(5);
  }
}

/**
 * Serial Device config
 */
static SerialConfig sconf = {.speed = 115200, .cr1 = 0, .cr2 = 0, .cr3 = 0};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);

//configure the GPIO pins UART1 (Serial Device 1)
  palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL); //PA9 is the TX pin
  palSetPadMode(GPIOA, 10, PAL_MODE_INPUT_PULLUP);            //PA10 is the RX

  sdStart(&SD1, &sconf);

  /*
   * Normal main() thread activity, spawning shells.
   */
  while (true) {
    chThdSleepMilliseconds(1000);
  }
}

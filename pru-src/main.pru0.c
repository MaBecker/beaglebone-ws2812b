// Programmable Real-time Unit Industrial Control SubSystem (PRU-ICSS) 
// /home/jonathan/beaglebone-compile/pru/bin/clpru
// #include <stdint.h>

// /sys/devices/platform/ocp/4a326004.pruss-soc-bus/4a300000.pruss/
// "P8_27 P8_28 P8_29 P8_30 P8_39 P8_40 P8_41 P8_42"
//
// /sys/devices/platform/ocp/ocp\:P9_27_pinmux/subsystem/devices/44e07000.gpio/driver/481ae000.gpio/gpiochip3/gpio/gpio105/value
// /sys/devices/platform/ocp/ocp\:P9_27_pinmux/
// echo 105 > ./sys/class/gpio/export
// echo out > /sys/devices/platform/ocp/ocp\:P9_27_pinmux/subsystem/devices/44e07000.gpio/driver/481ae000.gpio/gpiochip3/gpio/gpio105/direction
// echo 1 > /sys/class/gpio/gpio105/value
//
// P9_27_pinmux - pin 105 - pru 31_5
//    echo $pin
//    config-pin $pin pruout
//    config-pin -q $pin

#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "share.h"
#include "prugpio.h"

volatile register unsigned int __R30;
volatile register unsigned int __R31;

void main(void) 
{
  uint8_t   bit_num = 0;
  uint32_t  led_num = 0;
  uint32_t  led_count = 0;
  uint32_t  *gpio1 = (uint32_t *)GPIO1;
  uint32_t  *shared_mem = (uint32_t *)AM33XX_PRUSS_SHAREDRAM_BASE;
  // AM33XX_PRUSS_SHAREDRAM_BASE = 0x4a310000
  // PRU_SHAREDMEM   : org = 0x00010000 len = 0x00003000 CREGISTER=28 /* 12kB Shared RAM */

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // 9_14
  while (1)
  {
    if (shared_mem[SHARED_MEM_LED_BEGIN_WRITE_OFFSET] > 0)
    {
      led_count = shared_mem[SHARED_MEM_LED_COUNT_OFFSET];

      // loop over led colors
      for (led_num = 0; led_num < led_count; led_num++)
      {
        // loop over color bits for this led
        for (bit_num = 0; bit_num < WS2812_LED_BIT_COUNT; bit_num++)
        {
          if (shared_mem[led_num] & (1 << bit_num))
          {
            // delay_time set 1
            __R30 = P9_30;
            __delay_cycles(180);
            __R30 = 0;
            __delay_cycles(70);
          }
          else
          {
            // delay_time set 0
            __R30 = P9_30;
            __delay_cycles(70);
            __R30 = 0;
            __delay_cycles(180);
            //        
          }
        }
      }

      // reset begin write trigger
      shared_mem[SHARED_MEM_LED_BEGIN_WRITE_OFFSET] = 0;
    }

    __R30 = 0;
    __delay_cycles(100000/5);    // Wait 1/2 second
  }

    //        __R30 = 0;
    //        __delay_cycles(100000000/5);    // Wait 1/2 second
    //        // gpio1[GPIO_SETDATAOUT]   = USR0 | USR1 | USR2 | USR3;			// led test
    //
    //        __R30 = P9_30;
    //
    //        // P9_30 (GPIO_112) ( pruout?)
    //
    //        i = 1;
    //      }
    //      else
    //      {
    //        // gpio1[GPIO_CLEARDATAOUT] = USR0 | USR1 | USR2 | USR3 | P9_14;			// The the USR3 LED on
    //
    //        __R30 = 0;
    //
    //        i = 0;
    //      }

    // test
    // echo "pruout" > /sys/devices/platform/ocp/ocp\:P9_30_pinmux/state
    // __delay_cycles(100);    // 2 mhz (500 ns)

    // __R30 = P9_30;
    // __delay_cycles(180);    // 2 mhz (500 ns)
    // __R30 = 0;
    // __delay_cycles(70);    // 2 mhz (500 ns)

  __halt();
}

//  "/sys/class/gpio/export\0 105 \0" \
// Turns off triggers
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
	"/sys/class/leds/beaglebone:green:usr0/trigger\0none\0" \
	"/sys/class/leds/beaglebone:green:usr1/trigger\0none\0" \
	"/sys/class/leds/beaglebone:green:usr2/trigger\0none\0" \
	"/sys/class/leds/beaglebone:green:usr3/trigger\0none\0" \
  "/sys/devices/platform/ocp/ocp\:P9_30_pinmux/state\0 pruout \0" \
	"\0\0";


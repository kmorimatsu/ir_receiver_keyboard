/*
 * The source code in this file is provided by Katsumi as MIT license.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "input.h"

#define IRPORT 0
#define TVAL 562

volatile static unsigned int DAT=0;
volatile static int g_repeat=0;
volatile uint64_t expired_time=0;

bool check_button(int button_num){
	// Check the received IR code
	switch(button_num){
		case 0:
			if  (DAT!=0xa25dc837) return false;
			break;
		case 1:
			if  (DAT!=0xa25da857) return false;
			break;
		case 2:
			if  (DAT!=0xa25d9867) return false;
			break;
		default:
			return false;
	}
	// Return true only if not expired
	return time_us_64()<expired_time;
}

void ihandler(uint gpio, uint32_t event) {
	static uint64_t  T1=0,T2=0;
	static int COM=0, bits=0;
	int T3;
	
	// Time handling
	T1=time_us_64();        // Current time
	T3=(T1-T2+TVAL/2)/TVAL; // Difference from previous time
	T2=T1;                  // Store as the previous time for next timing
	
	// Check if signal from IR controller is still comming
	if (expired_time<T1) {
		g_repeat=0;
		DAT=0;
		COM=0;
		expired_time=0xffffffffffffffff;
	}
	
	switch(event){
		case GPIO_IRQ_EDGE_RISE:
			switch(T3){
				case 17:
				case 16:
				case 15:
					// Beginning of IR signal or repeating signal
					COM=160;
					break;
				case 0:
				case 1:
				case 2:
					// Data area in IR signal
					break;
				default:
					// Invalid
					COM=0;
					break;
			}
			break;
		case GPIO_IRQ_EDGE_FALL:
			switch(COM){
				case 160:
					if (8==T3) {
						// Beginning of IR signal
						COM=168;
						DAT=0;
						bits=0;
					} else if (4==T3) {
						// Repeat signal was detected
						g_repeat++;
						COM=0;
						expired_time=T1 + 110000;
					}
					break;
				case 168:
					// Data area in IR signal
					DAT<<=1;
					if (1==T3) {
						DAT+=0;
					} else if (3==T3) {
						DAT+=1;
					}
					bits++;
					if (32==bits) {
						// All data were received
						if (((DAT>>8)&0xff00ff) == ((DAT^0xff00ff)&0xff00ff)) {
							// The data is valid
							expired_time=T1 + 110000;
						}
						COM=0;
					}
					break;
				default:
					// Invalid
					COM=0;
					break;
			}
			break;
		default:
			break;
	}
}

void set_ihandler_in_core1(void){
	sleep_ms(10);
	// Set interrupt
	gpio_set_irq_enabled_with_callback(IRPORT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, ihandler);
	// Wait forever for interrupt
	while(true) asm("wfi");
}

void input_init(void) {
	// Initialize IR port
	gpio_init(IRPORT);
	gpio_set_dir(IRPORT,GPIO_IN);
	gpio_pull_up(IRPORT);
	// IR conde handler is controlled by Core 1 but not Core 0
    multicore_launch_core1(set_ihandler_in_core1);
	sleep_ms(10);
}

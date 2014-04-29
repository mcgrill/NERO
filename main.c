// ***********************************
// M2 Signal Mixing Code
// NERO -- IPD 501 Final Project
// base by nick mcgill [nmcgill.com]
// ***********************************

// HEADER FILES
#include "saast.h"  	// Includes m_general, math.h, avr/io.h
#include "m_usb.h"  	// USB comm
#include <avr/io.h>
#include <avr/wdt.h> 	// Watchdog

// DEFINES
#define DEBUG 0
#define MOTOR_R_TIMER 0
#define MOTOR_L_TIMER 1
#define MOTOR_R_CHAN 1
#define MOTOR_L_CHAN 1
#define STOPPED_DUTY 0
#define FREQ 50000      	// PWM Output Frequency
#define CH3_PIN 0
#define CH4_PIN 
#define CH5_PIN 

// VARIABLES
int r_duty, l_duty;
bool ch3_int, ch4_int, ch5_int;
bool new_ch3_signal, new_ch4_signal, new_ch5_signal;


// SUBROUTINES
void setup(void);


// ***********************************
//  Main Loop
// ***********************************
int main(void){
	setup();



	while(1){

		wdt_reset(); // Feed the dog!

		// Channel 3 interrupt
		if (ch3_int){
			if( check(PIND, CH3_PIN) ) { 	// Signal went HIGH
				// Start timer!
				ch3_start = STARTTIME; // [us] Start time
			}
			else{							// Signal went low, calc pulse dur
				if ( ch3_start  &&  new_ch3_signal == false ){
					ch3_val = (int)(ch3_end - ch3_start); // [us] Start time
					ch3_start = 0;
					new_ch3_signal = true;
				}
			}
		}



		if(new_ch3_signal || new_ch4_signal){
			// Set PWM to new duty cycle, proportional to direction
			r_duty = 5; // [0-100]
			l_duty = 5; // [0-100]
			m_pwm_duty(MOTOR_R_TIMER, MOTOR_R_CHAN, r_duty);
			m_pwm_duty(MOTOR_L_TIMER, MOTOR_L_CHAN, l_duty);
		}

		// (de)Activate motors based on CH5 kill switch
		if(new_ch5_signal){
			//if switch is activated...
				m_pwm_duty(MOTOR_R_TIMER, MOTOR_R_CHAN, STOPPED_DUTY);
				m_pwm_duty(MOTOR_L_TIMER, MOTOR_L_CHAN, STOPPED_DUTY);
			// Otherwise, set duty to what CH3/CH4 say
				m_pwm_duty(MOTOR_R_TIMER, MOTOR_R_CHAN, r_duty);
				m_pwm_duty(MOTOR_L_TIMER, MOTOR_L_CHAN, l_duty);
		}

	} // end while
} // end main



void setup(void)
{
	m_red(ON);
    m_green(ON);

    // ***********************************
    //  Clock Speed
    // ***********************************
    m_clockdivide(0);   	// Set to 16MHz ('0' = divide by 1)

    // ***********************************
    //  Watchdog
    // ***********************************
	wdt_reset();
	wdt_enable(WDTO_2S);	// Watchdog timer has a 2s interrupt

    // ***********************************
    //  Debugging
    // ***********************************
    if(DEBUG){
        /*  initialize m_usb com */
        m_usb_init();
        while(!m_usb_isconnected());
    }

	// ***********************************
    //  PWM Timer Initialization
    //	Timer 0 - D7 - MOTOR_R_TIMER
    //	Timer 1 - D6 - MOTOR_L_TIMER
    //	Channel 1 - D0 - MOTOR_R_CHAN
    //	Channel 1 - B6 - MOTOR_L_CHAN
    // ***********************************
    m_pwm_timer(MOTOR_R_TIMER, FREQ);						// Configure PWM freq
    m_pwm_output(MOTOR_R_TIMER, MOTOR_R_CHAN, ON);			// Connect timer to output
    m_pwm_duty(MOTOR_R_TIMER, MOTOR_R_CHAN, STOPPED_DUTY);	// Duty cycle & output

    m_pwm_timer(MOTOR_L_TIMER, FREQ);						// ^^^
    m_pwm_output(MOTOR_L_TIMER, MOTOR_L_CHAN, ON);			// ^^^
    m_pwm_duty(MOTOR_L_TIMER, MOTOR_L_CHAN, STOPPED_DUTY);	// ^^^

	sei(); // Enable global interrupts
  	
}


// Interrupt 0 -- Pin D0 -- Throttle -- CH3
ISR(INT0_vect){
	ch3_int = true;
}

// Interrupt 1 -- Pin D1 -- Direction -- CH4
ISR(INT1_vect){
	ch4_int = true;
}

// Interrupt 2 -- Pin D2 -- Kill Switch -- CH 5
ISR(INT2_vect){
	ch5_int = true;
}
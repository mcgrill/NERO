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
#define FREQ 50000      	// PWM Output Frequency
#define CH3_PIN 0
#define CH4_PIN 1
#define CH5_PIN 2

#define ON_OFFSET 2990
#define OFF_OFFSET 3030 // Anything higher than this turns motor on
#define MAX_TH 20       // Highest PWM output you'll see in motor THROTTLE
#define MAX_DIR 100     // Highest PWM output you'll see in motor DIR

#define CH3_LO 2220         // Lowest value for CH3
#define CH4_LO 2100         // Lowest value for CH4
#define CH3_HI 3850         // Highest value for CH3
#define CH4_HI 3700         // Highest value for CH4

#define CH5_LO_THRESH 1950 // Kill switch thresholds
#define CH5_HI_THRESH 4100


// VARIABLES
int ch4_duty, ch3_duty;
volatile bool ch3_int, ch4_int, ch5_int;
volatile unsigned int ch3_start, ch3_val, ch3_val_old;
volatile unsigned int ch4_start, ch4_val, ch4_val_old;
volatile unsigned int ch5_start, ch5_val, ch5_val_old;
volatile bool new_ch3_signal, new_ch4_signal, new_ch5_signal;
double beta = 0.7;
int ch3_range = CH3_HI - CH3_LO;
int ch4_range = CH4_HI - CH4_LO;

int STARTTIME;
int pin_six;
int pin_seven;
int pin_6;
int pin_7;
int pin_six_previous;
int pin_seven_previous;
int left_motor;
int right_motor;
int kill_switch;


// SUBROUTINES
void setup(void);
void ADC_init(void);

// ***********************************
//  Main Loop
// ***********************************
int main(void){
	setup();

	while(1){

        wdt_reset(); // Feed the dog!

        // ***********************************
        //  Debug Output
        // ***********************************
        if(DEBUG){
            m_usb_tx_string("CH3 Filt: ");
            m_usb_tx_uint(ch3_val);
            m_usb_tx_string("\t");

            m_usb_tx_string("CH4 Filt: ");
            m_usb_tx_uint(ch4_val);
            m_usb_tx_string("\t");

            m_usb_tx_string("CH5 Filt: ");
            m_usb_tx_uint(ch5_val);
            m_usb_tx_string("\t");

            m_usb_tx_string("Dir Duty: ");
            m_usb_tx_uint(ch4_duty);
            m_usb_tx_string("\t");

            m_usb_tx_string("Th Duty: ");
            m_usb_tx_uint(ch3_duty);
            

            m_usb_tx_string("\n");
        }


        // ***********************************
        //  Motor PWM
        // ***********************************
        // (de)Activate motors based on CH5 kill switch
        if (ch5_val < CH5_LO_THRESH) {         // If kill switch is activated, zero spin
            m_usb_tx_string("Kill switch activated! \t");
            OCR1B = OFF_OFFSET;             // Mapped onto pin B6
            OCR1C = OFF_OFFSET;             // Mapped onto pin B7
        }
        else{
            // PWM output to motors
            ch4_duty = ((ch4_val - CH4_LO) * MAX_DIR) / ch4_range; // Direction [0-MAX_DIR]
            ch3_duty = ((ch3_val - CH3_LO) * MAX_TH ) / ch3_range; // Throttle [0-MAX_TH]

            // OCR1A counts to 30000
            // Set OCR1B & C to move from 1500 to 3000
            // Left motor is slightly slow.
            // Slightly adjusted for this
            left_motor = ch3_duty + 0.65*(ch4_duty - 50) - 8;
            right_motor = ch3_duty + 0.65*(50 - ch4_duty);

            // Set PWM to new duty cycle, proportional to direction
            OCR1B = ON_OFFSET + left_motor;          // Mapped onto pin B6
            OCR1C = ON_OFFSET + right_motor;         // Mapped onto pin B7
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
    //  Timer 1 Setup
    // ***********************************
    
    // Timer 1 used to control PWM into motor 1 - left
    clear(TCCR1B, CS12);    // Turn Prescaler to /8
    set(TCCR1B, CS11);      // ^
    clear(TCCR1B, CS10);    // ^
    
    set(TCCR1B, WGM13);     // mode 15 - up to 0CR1A
    set(TCCR1B, WGM12);     // ^
    set(TCCR1A, WGM11);     // ^
    set(TCCR1A, WGM10);     // ^
    
    set(TCCR1A, COM1B1);    // clear at OCR1B, set at rollover
    clear(TCCR1A, COM1B0);  // ^
    
    set(TCCR1A, COM1C1);    // clear at OCR1C, set at rollover
    clear(TCCR1A, COM1C0);  // ^
    
    set(DDRB, 6);           // enable B6 as an output pin for PWM
    set(DDRB, 7);           // enable B7 as an output pin for PWM

    OCR1A = FREQ;           // Frequency of PWM


    // ***********************************
    //  Timer 3 Setup
    // ***********************************
    // clear(TCCR3B, CS32);    // Scale by 1
    // clear(TCCR3B, CS31);
    // set(TCCR3B, CS30);

    clear(TCCR3B, CS32);    // Scale by 8
    set(TCCR3B, CS31);
    clear(TCCR3B, CS30);

    clear(TCCR3B, WGM33);     // Timer Mode 0
    clear(TCCR3B, WGM32);
    clear(TCCR3A, WGM31);
    clear(TCCR3A, WGM30);


    // ***********************************
    //  External Interrupts
    // ***********************************
    // PCINT0 = D0 = CH3, throttle
    // PCINT1 = D1 = CH4, direction
    // PCINT2 = D2 = CH5, kill switch

    clear(EIMSK,INT0);	// PCINT0
    clear(EIMSK,INT1);	// PCINT1
    clear(EIMSK,INT2);	// PCINT2

	// PCINT0: Interrupt on either falling or rising edge
	clear(EICRA,ISC01);	
    set(EICRA,ISC00);

    // PCINT1: Interrupt on either falling or rising edge
    clear(EICRA,ISC11);	
    set(EICRA,ISC10);

    // PCINT2: Interrupt on either falling or rising edge
    clear(EICRA,ISC21);	
    set(EICRA,ISC20);

    // Remove masks for the corresponding interrupt 
    set(EIMSK,INT0);	// PCINT0
    set(EIMSK,INT1);	// PCINT1
    set(EIMSK,INT2);	// PCINT2

    sei(); // Enable global interrupts
  	
	m_red(OFF);
    m_green(OFF);
}






// Interrupt 0 -- Pin D0 -- Throttle -- CH3
ISR(INT0_vect){
    if( check(PIND, CH3_PIN) ){ // Signal went HIGH
        ch3_start = TCNT3;      // [counts] Start time
    }
    else{                       // Signal went low, calc pulse dur
        ch3_val_old = ch3_val;
        ch3_val = (int)(TCNT3 - ch3_start); // [counts] Start time
        ch3_start = 0;
        ch3_val = ch3_val*beta + ch3_val_old*(1-beta);
    }
}

// Interrupt 1 -- Pin D1 -- Direction -- CH4
ISR(INT1_vect){
    if( check(PIND, CH4_PIN) ){ // Signal went HIGH
        ch4_start = TCNT3;      // [counts] Start time
    }
    else{                       // Signal went low, calc pulse dur
        ch4_val_old = ch4_val;
        ch4_val = (int)(TCNT3 - ch4_start); // [counts] Start time
        ch4_start = 0;
        ch4_val = ch4_val*beta + ch4_val_old*(1-beta);
    }
}

// Interrupt 2 -- Pin D2 -- Kill Switch -- CH 5
ISR(INT2_vect){
    if( check(PIND, CH5_PIN) ){ // Signal went HIGH
        ch5_start = TCNT3;      // [counts] Start time
    }
    else{                       // Signal went low, calc pulse dur
        ch5_val_old = ch5_val;
        ch5_val = (int)(TCNT3 - ch5_start); // [counts] Start time
        ch5_start = 0;
        ch5_val = ch5_val*beta + ch5_val_old*(1-beta);
    }
}
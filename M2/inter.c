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
#define DEBUG 1
#define MOTOR_R_TIMER 0
#define MOTOR_L_TIMER 1
#define MOTOR_R_CHAN 1
#define MOTOR_L_CHAN 1
#define STOPPED_DUTY 0
#define FREQ 50000      	// PWM Output Frequency
#define CH3_PIN 0
#define CH4_PIN 1
#define CH5_PIN 2
#define MA_LEN 10

// VARIABLES
int r_duty, l_duty;
volatile bool ch3_int, ch4_int, ch5_int;
volatile unsigned int ch3_start, ch3_val, ch3_val_old;
volatile unsigned int ch4_start, ch4_val, ch4_val_old;
volatile unsigned int ch5_start, ch5_val, ch5_val_old;
volatile bool new_ch3_signal, new_ch4_signal, new_ch5_signal;
double beta = 0.7;

// SUBROUTINES
void setup(void);
// void MoveAvg_CH3(int NewValue3);
// void MoveAvg_CH4(int NewValue4);
// void MoveAvg_CH5(int NewValue5);

// ***********************************
//  Main Loop
// ***********************************
int main(void){
	setup();

	while(1){

        wdt_reset(); // Feed the dog!

        if(DEBUG){

            m_usb_tx_string("CH3 Filt: ");
            m_usb_tx_uint(ch3_val);
            m_usb_tx_string("\t");

            m_usb_tx_string("CH4 Filt: ");
            m_usb_tx_uint(ch4_val);
            m_usb_tx_string("\t");

            m_usb_tx_string("CH5 Filt: ");
            m_usb_tx_uint(ch5_val);
            m_usb_tx_string("\n");
        }


        if(new_ch3_signal || new_ch4_signal){
            new_ch3_signal = false;
            new_ch4_signal = false;
            // m_usb_tx_string("New CH3 or CH4 Signal \t");
        }

        // (de)Activate motors based on CH5 kill switch
        if(new_ch5_signal){
            new_ch5_signal = false;
            // m_usb_tx_string("New CH5 Signal \t");
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
    //  Timer 3 Setup
    // ***********************************
    // clear(TCCR3B, CS32);    // Scale by 1
    // clear(TCCR3B, CS31);
    // set(TCCR3B, CS30);

    // clear(TCCR3B, CS32);    // Scale by 8
    // set(TCCR3B, CS31);
    // clear(TCCR3B, CS30);

    clear(TCCR3B, CS32);    // Scale by 64
    set(TCCR3B, CS31);
    set(TCCR3B, CS30);

    // set(TCCR3B, CS32);    // Scale by 64
    // clear(TCCR3B, CS31);
    // clear(TCCR3B, CS30);

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



// // Subroutine for moving average
// void MoveAvg_CH3(int NewValue3){ // Moving Average, 8
//     RunSum3 = 0; int i;
//     for (i = 0; i < MA_LEN-2; i++) {
//         CH3_RunSum_Buffer[i] = CH3_RunSum_Buffer[i+1];
//         RunSum3 = RunSum3 + CH3_RunSum_Buffer[i];
//     }
//     CH3_RunSum_Buffer[MA_LEN-1] = NewValue3;
//     RunSum3 = (RunSum3 + NewValue3) / MA_LEN;
// }

// // Subroutine for moving average
// void MoveAvg_CH4(int NewValue4){ // Moving Average, 8
//     RunSum4 = 0; int i;
//     for (i = 0; i < MA_LEN-2; i++) {
//         CH4_RunSum_Buffer[i] = CH4_RunSum_Buffer[i+1];
//         RunSum4 = RunSum4 + CH4_RunSum_Buffer[i];
//     }
//     CH4_RunSum_Buffer[MA_LEN-1] = NewValue4;
//     RunSum4 = (RunSum4 + NewValue4) / MA_LEN;
// }

// // Subroutine for moving average
// void MoveAvg_CH5(int NewValue5){ // Moving Average, 8
//     RunSum5 = 0; int i;
//     for (i = 0; i < MA_LEN-2; i++) {
//         CH5_RunSum_Buffer[i] = CH5_RunSum_Buffer[i+1];
//         RunSum5 = RunSum5 + CH5_RunSum_Buffer[i];
//     }
//     CH5_RunSum_Buffer[MA_LEN-1] = NewValue5;
//     RunSum5 = (RunSum5 + NewValue5) / MA_LEN;
// }



// Interrupt 0 -- Pin D0 -- Throttle -- CH3
ISR(INT0_vect){
    if( check(PIND, CH3_PIN) ){ // Signal went HIGH
        ch3_start = TCNT3;      // [counts] Start time
    }
    else{                       // Signal went low, calc pulse dur
        ch3_val_old = ch3_val;
        ch3_val = (int)(TCNT3 - ch3_start); // [counts] Start time
        ch3_start = 0; new_ch3_signal = true;
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
        ch4_start = 0; new_ch4_signal = true;
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
        ch5_start = 0; new_ch5_signal = true;
        ch5_val = ch5_val*beta + ch5_val_old*(1-beta);
    }
}
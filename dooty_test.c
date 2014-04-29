// ***********************************
// PWM Motor Testing
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
#define CH4_PIN 
#define CH5_PIN 

// subroutines
void setup(void);
void set_ADC(void);
void update_ADC(void);

// global variables
int f0val = 0;
int dooty = 50;


int main(void){

	char rx_buffer; //computer interactions
	setup();
	set_ADC();
  
	while(1){
	
		update_ADC();
		while(!m_usb_rx_available());  	//wait for an indication from the computer
		rx_buffer = m_usb_rx_char();  	//grab the computer packet

		m_usb_rx_flush();  				//clear buffer		

		if(rx_buffer == 1) {  			//computer wants ir data
			//write ir data as concatenated hex:  f0
			m_usb_tx_hex(dooty);	// Elbow
			m_usb_tx_char('\n');  //MATLAB serial command reads 1 line at a time
		}

		dooty = (f0val*100)/900;
		m_pwm_duty(MOTOR_R_TIMER, MOTOR_R_CHAN, dooty);
	}
}

void setup(void)
{
	m_red(ON);
    m_green(ON);

    // ***********************************
    //  Clock Speed
    // ***********************************
    m_clockdivide(0);   	// Set to 16MHz ('0' = divide by 1)

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

	m_red(OFF);
	m_green(OFF);
}


//_______________________________________ Subroutine for setting ADCs
void set_ADC(void){
	//****************** set ADC values
	clear(ADMUX, REFS1); // voltage Reference - set to VCC
	set(ADMUX, REFS0);   // ^

	set(ADCSRA, ADPS2); // set the ADC clock prescaler, divide 16 MHz by 128 (set, set, set)
	set(ADCSRA, ADPS1); // ^
	set(ADCSRA, ADPS0); // ^
	
	set(DIDR0, ADC0D); // disable the f0 digital input
}



//_______________________________________ Subroutine for updating ADCs
void update_ADC(){ 		//update to current ADC values, set to ADC_F0, B4

	//-------------------> set pin F0 to read ADC values
	clear(ADCSRB, MUX5); // single-ended channel selection
	clear(ADMUX, MUX2); // ^
	clear(ADMUX, MUX1); // ^
	clear(ADMUX, MUX0); // ^
	
	set(ADCSRA, ADEN); // start conversion process
	set(ADCSRA, ADSC); // ^
	while(!check(ADCSRA,ADIF));


	f0val = ADC;
	set(ADCSRA, ADIF); // sets flag after conversion

}


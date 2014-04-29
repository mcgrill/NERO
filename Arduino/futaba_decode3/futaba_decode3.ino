#include <avr/interrupt.h>
                              //
void setup(void)
{
    pinMode(2, INPUT);
    digitalWrite(2, HIGH);    // Enable pullup resistor
    sei();                    // Enable global interrupts
    EIMSK |= (1 << INT0);     // Enable external interrupt INT0
    EICRA |= (1 << ISC01);    // Trigger INT0 on falling edge
    
    Serial.begin(9600); 
}
                              //
void loop(void)
{
                              //
}
                              //
// Interrupt Service Routine attached to INT0 vector
ISR(EXT_INT0_vect)
{
}

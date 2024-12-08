
#include <avr/io.h>
#include <avr/interrupt.h>

// init ADC
void initADC()
{
    ADMUX |= (1 << REFS0); // set voltage refference to AVcc
    ADMUX |= (1 << ADLAR); // set to 8 Bit mode
    ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0)); // set to freerunning
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //set prescaler 128
    ADCSRA |= (1 << ADIE); // enable interupt
    ADCSRA |= (1 << ADEN); // enable the digital to analog converter
    ADCSRA |= (1 << ADSC); // start first ADC conversion
}

// init timer0 used for the the frequency of the the buzzer. it is manipulated by the LDR
void initTimer0()
{
    TIMSK0 |= (1 << OCIE0A); // enable interupts
    TCCR0A |= (1 << WGM01); // enable CTC mode.
    TCCR0B |= (1 << CS02); // set the prescaler to 256.
}
// init timer2 used for volume of the buzzer.  
void initTimer2()
{
    TCCR2A |= (1 << WGM20) | (1 << WGM21); // set to fast PWM mode
    TCCR2A |= (1 << COM2A1); //Clear on OC2A Compare match, set OC2A at BOTTOM, (non-inverting mode)
    TCCR2B |= (1 << CS20); // no prescaler
    TCNT2 = 0; // set counter to 0
}

void initBuzzer(){
    DDRB |= (1<<DDB3); // set buzzer (pin 11) to output
}

ISR(TIMER0_COMPA_vect) // timer 0 creates the frequency of the buzzer
{
    TCCR2A ^= (1 << COM2A1); // Toggled the OC2A pin between non-inverting mode and off.
    PORTB ^= (1 << PB3);     // flip pin 11
}

ISR(ADC_vect) // toggles between reading the potentiometer
{
    if (ADMUX & (1 << MUX0)) // if true write ADCH value to OCR0A else to OCR2A
    {
        OCR0A = ADCH; // ADCH is LDR value
    }
    else
    {
        OCR2A = ADCH; // ADCH is potentiometer value
    }

    ADMUX ^= (1<<MUX0); //flip between ADC0 and ADC1
    ADCSRA |= (1<<ADSC); //start new ADC conversion
}

int main()
{
    sei();

    initBuzzer();
    initADC();
    initTimer0();
    initTimer2();

    while (true); // infinite loop

    return 0;
}
// 'Theremin' assignment by Mariëlle Groen(s1185336) and Emma Hendriksen(s1199466)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <HardwareSerial.h>

void init_ADC(){
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3)); //input chanel A0
    ADMUX |= (1<<REFS0); //reference voltage on AVCC
    ADMUX |= (1<<ADLAR); //make ADCH an 8 bit regitser
    ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0)); //freerunning
    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC clock prescaler 128
    ADCSRA |= (1<<ADIE); //enable interrupt
    ADCSRA |= (1<<ADEN); //enable ADC
    ADCSRA |= (1<<ADSC); //start first ADC conversion
}

ISR(ADC_vect){
    if(ADMUX & (1<<MUX0)){
        OCR0A = ADCH; //change OCR0A to ADCH value
    }else if(~(ADMUX & (1<<MUX0))){
        OCR2A = ADCH; //change OCR2A to ADCH value
    }

    ADMUX ^= (1<<MUX0); //flip between ADC0 and ADC1
    ADCSRA |= (1<<ADSC); //start new ADC conversion
}

void init_TC0(){ //timer 0
    TCCR0A = 0; //set TCCR0A register to 0
    TCCR0B |= (1<<WGM01)|(1<<CS01)|(1<<CS00); //CTC mode/prescaler 64
    TCNT0  = 0;//initialize counter value to 0
    TIMSK0 |= (1 << OCIE0A); //enable timer compare interupts
}

ISR(TIMER0_COMPA_vect){
    TCCR2A ^= (1<<COM2A1); //flip PWM mode, non inverting/OC2A off
    PORTB ^= (1 << PB3); //flip pin 11
}

void init_TC2(){ //timer 2
    TCCR2A = (1<<WGM20)|(1<<WGM21);//fast PWM mode
    TCCR2A |= (1<<COM2A1); //PWM mode, non inverting
    TCCR2B |= (1<<CS20); //no prescaler
    TCNT2  = 0;//initialize counter value to 0
}

int main(){
    sei(); //enable interupts

    DDRB |= (1<<DDB3); //set arduino pin 11 to output for the buzzer

    init_ADC(); //initialize ADC hardware block
    init_TC0(); //initialize timer counter 0
    init_TC2(); //initialize timer counter 2

    while (1){  
        //endless loop
    }
    

    return 0; //never reached
}

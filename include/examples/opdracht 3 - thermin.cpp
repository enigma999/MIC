// Vera van Veenhuizen - S1188887
// Rob Drost - S1192221

#include <avr/interrupt.h>
#include <avr/io.h>

ISR(TIMER0_COMPA_vect)
{
  // interupt from timer0
  TCCR2A ^= (1 << COM2A1); // toggle OC2A between non-inverting mode and off
}

ISR(ADC_vect)
{
  // interupt form ADC
  if (ADMUX & (1 << MUX0))
  {
    OCR2A = ADCH; // write ADCH value to OCR2A
  }
  else
  {
    OCR0A = ADCH; // write ADCH value to OCR0A
  }
  ADMUX ^= (1 << MUX0); // toggle ADMUX between ldr and potentiometer
}

void init_ADC()
{
  // initialize ADC
  ADMUX |= (1 << REFS0);                                   // voltage reference
  ADMUX |= (1 << ADLAR);                                   // left adjust result
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // set prescaler 128
  ADCSRB &= ~((1 << ADTS0) | (1 << ADTS1) | (1 << ADTS2)); // enable freerunning mode
  ADCSRA |= (1 << ADIE);                                   // enable interrupts
  ADCSRA |= (1 << ADATE);                                  // enable auto trigger
  ADCSRA |= (1 << ADEN);                                   // start prescaler counting
  ADCSRA |= (1 << ADSC);                                   // start first conversion
}

void init_timer0()
{
  // initialize timer0
  TIMSK0 |= (1 << OCIE0A); // enable interrupts
  TCCR0A |= (1 << WGM01);  // enable CTC mode
  TCCR0B |= (1 << CS02);   // set prescaler 256
}

void init_timer2()
{
  // initialize timer2
  TCCR2A = (1 << WGM22) | (1 << WGM21) | (1 << WGM20); // set fast PWM mode
  TCCR2B |= (1 << CS22) | (1 << CS21);                 // set prescaler 256
}

void init_buzzer()
{
  // initialize buzzer
  DDRB |= (1 << DDB3); // set buzzer output
}

void init()
{
  // initialize everything
  init_ADC();
  init_timer0();
  init_timer2();
  init_buzzer();
  sei();
}

int main()
{
  init();

  while (true)
  {
    // infinite loop
  }

  return 0;
}

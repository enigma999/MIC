#include <Arduino.h>

#define RESET 0

enum bstate
{
    pressed,
    released
};

bstate buttonState = released; // default is released.
uint8_t volatile centibeats = 0;

void initPins(void){
  DDRB |= (1 << DDB1);
  PORTB |= (1 << PORTB1);
  PORTC |= (1 << PORT1) | (1 << PORT4) | (1 << PORT5);
  PORTD |= (1 << PORT2);                               
  PCICR |= (1 << PCIE2);                              
  PCMSK2 |= (1 << PCINT18);      
}

void initTimers(void)
{
  //timer 0
  TCCR0A |= (1 << WGM01);
  OCR0A = 0000;
  TCCR0B |= (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);
  TCNT0 = 0;

  //timer 1
  TCCR1A |= (1 << COM1A0);
  OCR1A = 00000000000000000000;
  TIMSK1 |= (1 << OCIE1A);
  TCNT1 = 0;
}

void setup()
{
    initPins();
    initTimers();
    sei();
}


// ISR
ISR(PCINT2_vect)
{
    if (!(PIND & (1 << PIND2))) //
    {
        buttonState = pressed;
    }
    else
    {
        buttonState = released;
    }
    TCCR0B |= (1 << CS01) | (1 << CS00);
}

// ISR timer 0 voor debounce
ISR(TIMER0_COMPA_vect)
{
    sei(); 
    TCCR0B &= ~((1 << CS01) | (1 << CS00)); //stop timer
    TCNT0 = RESET;
}
// ISR timer 1 voor tellen
ISR(TIMER1_COMPA_vect)
{
  if(centibeats >= 15) //Timer overflow check
  {
    centibeats = 0;
  }else{
    ++centibeats;
  }

}

void resetCounter()
{
}

void display()
{
}
int main()
{
    setup();

    while (true)
    {
        if (buttonState == pressed)
        {
            PORTB = (1 << PORTB1);
        }
        else
        {
            PORTB = 0 & (1 << PORTB1);
        }
    }
}
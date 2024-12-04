#include <Arduino.h>

enum bstate
{
    pressed,
    released
};

bstate buttonState = released; // default is released.

void setup()
{
    // pins
    DDRB |= (1 << DDB1);
    PORTB |= (1 << PORTB1);
    PORTC |= (1 << PORT1) | (1 << PORT4) | (1 << PORT5);
    PORTD |= (1 << PORT2);                               
    PCICR |= (1 << PCIE2);                              
    PCMSK2 |= (1 << PCINT18);                            
    // timer
    // interupt
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
}

// ISR timer 0 voor debounce
ISR(TIMER0_COMPA_vect)
{
}
// ISR timer 1 voor tellen
ISR(TIMER1_COMPA_vect)
{
}

void startTimer()
{
}

void stopTimer()
{
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
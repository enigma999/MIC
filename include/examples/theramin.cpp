#include <Arduino.h>
#include <Wire.h>

#define RESET 0


enum bstate
{
    pressed,
    released
};

const uint8_t segmentValues[16] = {
  0xBF, //0
  0x86, //1
  0xBB, //2
  0xAF, //3
  0xE6, //4
  0xED, //5
  0xFD, //6
  0x17, //7
  0xFF, //8
  0xEF, //9
  0xF7, //A
  0xFC, //B
  0xB9, //C
  0xDE, //D
  0xF9, //E
  0xF1  //F
};

bstate buttonState = released; // default is released.
uint8_t volatile amountOfCentibeats = 0;

void initPins(void){
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
    Wire.begin();
    sei();
}

void displayCentibeats(uint8_t centibeats)
{
  Wire.beginTransmission(0x21);
  Wire.write(~(segmentValues[centibeats]));
  Wire.endTransmission();
}

// ISR
ISR(PCINT2_vect)
{
    if (!(PIND & (1 << PIND2))) //
    {
        amountOfCentibeats = RESET;      
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
    cli();
    TCNT0 = RESET;
}
// ISR timer 1 voor tellen
ISR(TIMER1_COMPA_vect)
{
  if(amountOfCentibeats >= 15) //Timer overflow check
  {
    amountOfCentibeats = 0;
  }else{
    ++amountOfCentibeats;
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
            TCCR1B |= (1 << CS12) | (1 << WGM12);
        }
        else
        {
            TCCR1B &= ~((1 << CS12));
        }
        //displayCentibeats(amountOfCentibeats) ; 

        if((int)displayCentibeats % 2 == 0){
          TCCR1B |= (1 << CS12) | (1 << WGM12);
          Wire.beginTransmission(0x21);
          Wire.write(~(segmentValues[5]));
          Wire.endTransmission();
        }else{
          Wire.beginTransmission(0x21);
          Wire.write(~(segmentValues[1]));
          Wire.endTransmission();
        }
    }
}
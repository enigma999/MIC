#include <Arduino.h>
#include <Wire.h>

#define TIMERRESET 0
#define SEGMENTADDRESS 0x21
#define MINCENTIBEATS 0
#define MAXCENTIBEATS 15

#define TIMER0COMPARE 234
#define TIMER1COMPARE 53999
enum bstate
{
    pressed,
    released
};

const uint8_t segmentValues[16] = {
  0xBF, //0
  0x86, //1
  0xDB, //2
  0xCF, //3
  0xE6, //4
  0xED, //5
  0xFD, //6
  0x87, //7
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
uint8_t volatile amountOfCentibeats = MINCENTIBEATS;

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
  OCR0A = TIMER0COMPARE;
  TCCR0B |= (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);
  TCNT0 = TIMERRESET;

  //timer 1
  TCCR1A |= (1 << COM1A0);
  OCR1A = TIMER1COMPARE;
  TIMSK1 |= (1 << OCIE1A);
  TCNT1 = TIMERRESET;
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
  Wire.beginTransmission(SEGMENTADDRESS);
  Wire.write(~(segmentValues[centibeats]));
  Wire.endTransmission();
}

// ISR
ISR(PCINT2_vect)
{
    if (!(PIND & (1 << PIND2))) //
    {
        amountOfCentibeats = MINCENTIBEATS;      
        buttonState = pressed;
    }
    else
    {
        buttonState = released;
    }
    TCCR0B |= (1 << CS01) | (1 << CS00); //start timer 0
}

// ISR timer 0 voor debounce
ISR(TIMER0_COMPA_vect)
{
    sei(); 
    TCCR0B &= ~((1 << CS01) | (1 << CS00)); //stop timer 0
    cli();
    TCNT0 = TIMERRESET;
}
// ISR timer 1 voor tellen
ISR(TIMER1_COMPA_vect)
{
  if(amountOfCentibeats >= MAXCENTIBEATS) //Timer overflow check
  {
    amountOfCentibeats = MINCENTIBEATS;
  }else{
    ++amountOfCentibeats;
  }
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
        displayCentibeats(amountOfCentibeats) ; 
    }
    return 0;
}
#include <Arduino.h>
#define BAUDRATE 9600
#define DEBOUNCETIME 50

enum bstate
{
  pressed,
  released
};

int prevButtonState;

void display_counter(uint8_t counter)
{
  PORTC = counter;
}

void init()
{
  DDRC = 0x0f;
  PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
  DDRD = 0x00;
  PORTD |= (1 << PORTD2);
  prevButtonState = released;
}

enum bstate readButton(void)
{
  if (!((PIND & 0b00000100) == 0b0000100))
  {
    return pressed;
  }
  return released;
}

enum bstate button_state(void)
{
  enum bstate currentState = readButton();
  if (currentState != prevButtonState)
  {
    delay(DEBOUNCETIME);
  }
  // als anders wachten
  return currentState;
}

int main()
{
  //initialize
  init();
  while(true){
  prevButtonState = button_state()
  //check vehicle passed
     //increment counter
 

  //check counter overflow
    //reset counter
  
  //display counter
  }
 

}
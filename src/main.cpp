#include <Arduino.h>
#define BAUDRATE 9600
#define DEBOUNCETIME 50

enum bstate
{
  pressed,
  released
};

int previousButtonState;

void display_counter(uint8_t counter)
{
  PORTC = counter;
}

void initialize()
{
  DDRC = 0x0f;
  PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
  DDRD = 0x00;
  PORTD |= (1 << PORTD2);
  previousButtonState = released;
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
  if (currentState != previousButtonState)
  {
    delay(DEBOUNCETIME);
  }
  return currentState;
}

  bool vehicle_passed(void)
  {
    bstate currentButtonState = button_state();
    if(currentButtonState == released && previousButtonState == pressed)
    {
      //increment;
    }
    previousButtonState = currentButtonState;
  }

int main()
{
  //initialize
  initialize();
  while(true){
  previousButtonState = button_state();
  //check vehicle passed
     //increment counter
 

  //check counter overflow
    //reset counter
  
  //display counter
  }
 

}
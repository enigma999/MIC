#include <Arduino.h>
#define BAUDRATE 9600

#define LedPinsIO 0x0F
#define ButtonPinIO 0x00


#define DEBOUNCETIME 50
#define COUNTERSTARTVALUE 0

enum bstate
{
  pressed,
  released
};

int previousButtonState;
uint8_t counter = COUNTERSTARTVALUE;

void display_counter(uint8_t counter)
{
  PORTC = counter;
}

void init_pins()
{
  DDRC = LedPinsIO;
  PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);

  DDRD = ButtonPinIO;
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
  bstate currentButtonState = button_state(); //get current state
  if (currentButtonState == released && previousButtonState == pressed) //if the button has been pushed return true
  {
    return true;
  }
  previousButtonState = currentButtonState;
  return false;
}

int main()
{
  // initialize
  init_pins();
  while (true)
  {
    previousButtonState = button_state();
    
    if(vehicle_passed)
    {
      if(counter < 15)
      {
        counter++;
      }else
      {
        counter = COUNTERSTARTVALUE;
      }
    }
    display_counter(0x02);
  }
}
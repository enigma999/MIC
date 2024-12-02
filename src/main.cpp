#include <Arduino.h>

#define DEBOUNCETIME 50
#define COUNTERRESTARTVALUE 0
#define MAXCOUNTERVALUE 15

enum bstate
{
  pressed,
  released
};

int previousButtonState = released;
uint8_t counterValue = COUNTERRESTARTVALUE;
bool hasBeenPressed = false;

void display_counter(uint8_t counter)
{
  PORTC = counter;
}


void init_pins(){
  //initialize pins for LED's
  DDRC = (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3);
  PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
  //initialize pins for BUTTON
  DDRD |= (1 << DDD2);
  PORTD |= (1 << PORTD2);
}

enum bstate readButton(void)
{
  if (!(PIND & (1<< PIND2)))
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
  previousButtonState = currentState;
  return currentState;
}

bool vehicle_passed(void)
{
  bstate state = button_state();
  bool returnValue = false;
  if(state == pressed)
  {
    hasBeenPressed = true;
  }else if(hasBeenPressed == true){
    returnValue = true;
    hasBeenPressed = false;
  }
  return returnValue;
}

int main()
{
  init();
  // initialize
  init_pins();

  while (true)
  {
    if(vehicle_passed())
    {
      if(counterValue < MAXCOUNTERVALUE)
        {
          counterValue++;
        }else
        {
          counterValue = COUNTERRESTARTVALUE;
        }
      }
      display_counter(counterValue);
  }
  return 0;
}
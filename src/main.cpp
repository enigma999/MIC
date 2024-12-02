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
bool hasBeenPressed = false;

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
  previousButtonState = button_state;
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
  // initialize
  init_pins();
  Serial.begin(9600);
  while (true)
  {
    //previousButtonState = button_state();
    Serial.println(counter);
    if(!vehicle_passed)
    {
      if(counter < 15)
      {
        counter++;
      }else
      {
        counter = COUNTERSTARTVALUE;
      }
    }
    display_counter(counter);
  }
  return 0;
}
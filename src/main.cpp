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
uint8_t counterValue = COUNTERSTARTVALUE;
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
  sei();

  // initialize
  init_pins();
  Serial.begin(9600);

  while (true)
  {
    if(vehicle_passed())
    {
      if(counterValue < 15)
        {
          counterValue++;
        }else
        {
          counterValue = COUNTERSTARTVALUE;
        }
      }
      display_counter(counterValue);
      Serial.println(counterValue);
  }
  return 0;
}
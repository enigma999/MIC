#include <Arduino.h>
#define BAUDRATE 9600

enum bstate
{
  pressed,
  released
};

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
}

bool readButton()
{
  return !((PIND & 0b00000100) == 0b0000100);
}

enum bstate button_state(void)
{
}

int main()
{
  init();
  while (true)
  {
    if (readButton())
    {
      display_counter(0b00001111);
    }
    else
    {
      display_counter(0b00000000);
    }
  }
}
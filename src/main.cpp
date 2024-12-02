#include <Arduino.h>
#define BAUDRATE 9600


void display_counter(uint8_t counter){
    PORTC = counter;
}

int main(){
    DDRC |= (1 << PORTC0) |(1 << PORTC1) |(1 << PORTC2) |(1 << PORTC3);;

    while(true)
    {
        display_counter(0b00000000);
    }
}
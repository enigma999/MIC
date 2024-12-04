// 'Internet time stopwatch' assignment by Mariëlle Groen(s1185336) and Emma Hendriksen(s1199466)
// Calculations for the timers:
/* 
Timer0 uses a prescaler of 64 to create a debounce delay of 15 ms.
The Arduino clock is 16 MHz, so after prescaling:
Timer Clock = 16 MHz / 64 = 250,000 Hz
We need a delay of 15 ms (0.015 seconds).
Using CTC mode, the compare match value is calculated as:
Compare Value = (Desired Time * Timer Clock) - 1
              = (0.015 * 250,000) - 1
              = 3750 - 1
              = 3749
This creates a debounce delay of 15 ms.

Timer1 uses a prescaler of 256 to generate a time base for centibeats (0.864 seconds).
The Arduino clock is 16 MHz, so after prescaling:
Timer Clock = 16 MHz / 256 = 62,500 Hz
The desired time is 0.864 seconds.
Using CTC mode, the compare match value is calculated as:
Compare Value = (Desired Time * Timer Clock) - 1
              = (0.864 * 62,500) - 1
              = 54,000 - 1
              = 53,999
This generates an interrupt every 0.864 seconds.
*/


#define ADDRESS 0x21  // address: PCF8574

// Included libraries
#include <Arduino.h> // Arduino library
#include <avr/io.h> // Used for the IO definitions
#include <avr/interrupt.h> // Used for interrupts for the timers and button
#include <Wire.h> // Used for the seven-segment display

const int NUMBER_OF_CONFIGURATIONS = 16; // Number of display configurations for 0 to F
const uint8_t COUNTER_MINIMUM = 0x0; // Lowest value for the counter
const uint8_t COUNTER_MAXIMUM = 0xF; // Highest value for the counter
const int TIMER1_COMPARE_VALUE = 53999; // Compare value for Timer 1
const int TIMER0_COMPARE_VALUE = 234; // Compare value for Timer 0 (15 ms debounce)
uint8_t volatile centibeatCount = COUNTER_MINIMUM; // Count of centibeats

const uint8_t CONFIGURATION[NUMBER_OF_CONFIGURATIONS] = {
  // Corresponding order of LEDs is: DP, G, F, E, D, C, B, A
  0b10111111, // Configuration for the number 0
  0b10000110, // Configuration for the number 1
  0b11011011, // Configuration for the number 2
  0b11001111, // Configuration for the number 3
  0b11100110, // Configuration for the number 4
  0b11101101, // Configuration for the number 5
  0b11111101, // Configuration for the number 6
  0b10000111, // Configuration for the number 7
  0b11111111, // Configuration for the number 8
  0b11101111, // Configuration for the number 9
  0b11110111, // Configuration for the number A
  0b11111100, // Configuration for the number B
  0b10111001, // Configuration for the number C
  0b11011110, // Configuration for the number D
  0b11111001, // Configuration for the number E
  0b11110001  // Configuration for the number F
};

enum ButtonStates {BUTTON_PRESSED, BUTTON_RELEASED}; // States for the button

volatile ButtonStates buttonState = BUTTON_RELEASED;  // Current button state
volatile ButtonStates previousButtonState = BUTTON_RELEASED;  // Previous button state
volatile ButtonStates buttonStateCheck = BUTTON_RELEASED;  // Button state check in main loop

void displayCentibeats(uint8_t centibeats) {
  Wire.beginTransmission(ADDRESS); // Start transmission to PCF8574
  Wire.write(~(CONFIGURATION[centibeats])); // Send the correct display configuration
  Wire.endTransmission(); // End transmission
}

void setupTimer1(void) {
  TCCR1A |= (1 << COM1A0); // Toggle OC1A on compare match
  OCR1A = TIMER1_COMPARE_VALUE; // Set compare value for OCR1A
  TIMSK1 |= (1 << OCIE1A); // Enable compare match interrupt for OCR1A
  TCNT1 = 0; // Initialize counter
  sei(); // Enable global interrupts
}

void setupTimer0_15ms() {
  TCCR0A |= (1 << WGM01); // CTC mode
  OCR0A = TIMER0_COMPARE_VALUE; // Compare match register for 15 ms
  TCCR0B |= (1 << CS01) | (1 << CS00); // Set prescaler to 64
  TIMSK0 |= (1 << OCIE0A); // Enable Timer 0 compare interrupt
  TCNT0 = 0; // Initialize counter
  sei(); // Enable global interrupts
}

ISR(TIMER0_COMPA_vect) {
  sei(); // Re-enable interrupts after debounce period
  TCCR0B &= ~((1 << CS01) | (1 << CS00)); // Stop Timer 0
  TCNT0 = 0; // Reset Timer 0 counter
}

ISR(PCINT2_vect) { // Interrupt for button press/release
  if (PIND & (1 << PD2))  {
    buttonStateCheck = BUTTON_RELEASED; // Button is released, update state for main loop
  } else {
    centibeatCount = COUNTER_MINIMUM; // Reset centibeat count when button is pressed
    buttonStateCheck = BUTTON_PRESSED; // Button is pressed, update state for main loop
  }
  
  TCCR0B |= (1 << CS01) | (1 << CS00); // Start Timer 0 with prescaler 64
}

ISR(TIMER1_COMPA_vect) {
  if (centibeatCount >= COUNTER_MAXIMUM) {
    centibeatCount = COUNTER_MINIMUM; // Reset count if it overflows
  } else {
    centibeatCount++; // Increase count
  }
}

void setup() {
  PORTC |= (1 << PORT1) | (1 << PORT4) | (1 << PORT5); // Pull-up resistance for game shield
  PORTD |= (1 << PORT2); // Pull-up for button
  PCICR |= (1 << PCIE2); // Enable pin change interrupts for D pins
  PCMSK2 |= (1 << PCINT18); // Mask for PD2/PCINT18

  Wire.begin(); // Initialize I2C
  setupTimer1(); // Setup Timer 1 for centibeat counting
  setupTimer0_15ms(); // Setup Timer 0 for debounce timing
  sei(); // Enable interrupts
}

int main() {
  setup(); // Setup pins and timers

  while (1) {
    if (buttonStateCheck == BUTTON_RELEASED) {
        TCCR1B &= ~((1 << CS12)); // Stop Timer 1 if button is released
    } else if (buttonStateCheck == BUTTON_PRESSED) {
        TCCR1B |= (1 << CS12) | (1 << WGM12); // Start Timer 1 if button is pressed
    }

    displayCentibeats(centibeatCount); // Continuously display centibeats
  }
 
  return 0; // Never reached
}
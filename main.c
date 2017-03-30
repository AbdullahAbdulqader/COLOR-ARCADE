// ABDULLAH ABDULQADER MARCH 2017
#include <msp430.h>
#include "ws2812.h"

#define START_BUTTON    BIT4

#define RED     ((char) 0)
#define YELLOW  ((char) 1)
#define BLUE    ((char) 2)
#define GREEN   ((char) 3)

#define LED_OFF   ((signed char) -1)

#define RED_BUTTON      BIT0
#define YELLOW_BUTTON   BIT1
#define BLUE_BUTTON     BIT2
#define GREEN_BUTTON    BIT3

char seed;                         // seed for generating a random numbers
signed char strip_1[NUM_LEDS];        // this will hold the color sequence of strip_1
// signed char strip_2[NUM_LEDS];        // this will hold the color sequence of strip_2
signed char Top_LED_Index;                // Index of top lit LED. Will be decremented down to 0.
signed char Top_LED_Color;                // Color of top lit LED

// WS2812 takes GRB format
typedef struct {
    u_char green;
    u_char red;
    u_char blue;
} LED;

static LED leds[NUM_LEDS] = { { 0, 0, 0 } };

// Initializes everything needed to use this library. This clears the strip.
void initStrip() {
    P1SEL |= OUTPUT_PIN;    // configure output pin as SPI output
    P1SEL2 |= OUTPUT_PIN;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, MSB, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;   // SMCLK source (16 MHz)
    UCB0BR0 = 3;            // 16 MHz / 3 = .1875 us per bit
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;   // Initialize USCI state machine
    clearStrip();           // clear the strip
}

// Sets the color of a certain LED (0 indexed)
void setLEDColor(u_int p, u_char r, u_char g, u_char b) {
	leds[p].green = g;
    leds[p].red = r;
    leds[p].blue = b;
}

// Send colors to the strip and show them. Disables interrupts while processing.
void showStrip() {
    __bic_SR_register(GIE);  // disable interrupts

    // send RGB color for every LED
    unsigned int i, j;
    for (i = 0; i < NUM_LEDS; i++) {
        u_char *rgb = (u_char *)&leds[i]; // get GRB color for this LED

        // send green, then red, then blue
        for (j = 0; j < 3; j++) {
            u_char mask = 0x80;    // b1000000

            // check each of the 8 bits
            while (mask != 0) {
                while (!(IFG2 & UCB0TXIFG))
                    ;    // wait to transmit
                if (rgb[j] & mask) {        // most significant bit first
                    UCB0TXBUF = HIGH_CODE;  // send 1
                } else {
                    UCB0TXBUF = LOW_CODE;   // send 0
                }

                mask >>= 1;  // check next bit
            }
        }
    }

    // send RES code for at least 50 us (800 cycles at 16 MHz)
    __delay_cycles(800);

    __bis_SR_register(GIE);    // enable interrupts
}

// Clear the color of all LEDs (make them black/off)
void clearStrip() {
    fillStrip(0x00, 0x00, 0x00);  // black
}

// Fill the strip with a solid color. This will update the strip.
void fillStrip(u_char r, u_char g, u_char b) {
    int i;
    for (i = 0; i < NUM_LEDS; i++) {
        setLEDColor(i, r, g, b);  // set all LEDs to specified color
    }
    showStrip();  // refresh strip
}

// Generate a random number ranging from 0 to 3.
// This function doesn't truely generate a random number but it is good enough
// for the purpose of this porject.
char random(char x) {
  char number = ((x % 11) + (x % 3)) % 5;

  if (number == 4) {
    return random(x % 13);
  }

  return number;
}

// assign colors to all LEDS
void colorLEDS(signed char strip_x[NUM_LEDS]) {
  for (char i = 0; i < NUM_LEDS; i++) {
    switch (strip_x[i]) {
      case RED:
        setLEDColor(i, 0xFF, 0x00, 0x00);
        break;
      case YELLOW:
        setLEDColor(i, 0xFF, 0xFF, 0x00);
        break;
      case BLUE:
        setLEDColor(i, 0x00, 0x00, 0xFF);
        break;
      case GREEN:
        setLEDColor(i, 0x00, 0xFF, 0x00);
        break;
      case LED_OFF:
        setLEDColor(i, 0x00, 0x00, 0x00);
        break;
    }
  }
}

// penalty when wrong button is pressed
void penalty() {
  fillStrip(0x00, 0x00, 0x00);
  showStrip();
  __delay_cycles(8000000);
  fillStrip(0xFF, 0x00, 0x00);
  showStrip();
  __delay_cycles(8000000);
  fillStrip(0x00, 0x00, 0x00);
  showStrip();
  __delay_cycles(8000000);
  fillStrip(0xFF, 0x00, 0x00);
  showStrip();
  __delay_cycles(8000000);
}


int main(void) {
  WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
  if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
  {
      while(1);              // do not load, trap CPU!!
  }

  // configure clock to 16 MHz
  BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
  DCOCTL = CALDCO_16MHZ;

  // initialize LED strip
  initStrip();

  TACTL = TASSEL_2 | MC_2;  // start timer in up/down mode
  // Set the direction of the start button. Keep initial input low.
  P1DIR &= ~START_BUTTON;
  P1IN  &= ~START_BUTTON;

  // Set the direction of color bits as input to the msp430. Keep initial input zero.
  P1DIR &= ~(RED_BUTTON + YELLOW_BUTTON + BLUE_BUTTON + GREEN_BUTTON);
  P1IN  &= ~(RED_BUTTON + YELLOW_BUTTON + BLUE_BUTTON + GREEN_BUTTON);

  // Initialize top index
  Top_LED_Index = NUM_LEDS-1;

  // Generate random seed
  while ((~P1IN) & START_BUTTON);   // loop while the START_BUTTON is unset
  TACTL = TASSEL_1 | MC_0;          // stop timer once START_BUTTON is pushed
  seed  = TAR;                       // this is the seed for random(int seed)

  // Generate a random sequence of colors for the LED strips 1 and 2
  char randomColor;
  for (char i = 0; i < NUM_LEDS; i++ ) {
    seed = seed + i;
    randomColor = random(seed);
    strip_1[i] = randomColor;
    // strip_2[i] = randomColor;
    // printf("strip_1[%d] = %d\n", i, strip_1[i] );
  }

  // Initialize top LED color
  Top_LED_Color = strip_1[Top_LED_Index];

  // assign colors to LEDs
  colorLEDS(strip_1);

  // Light up the addressable LED strips
  showStrip();

  // Listen to player input. Condition: there are still lit LED(s).
  while(Top_LED_Index >= 0) {
    // loop while all buttons are unset. wait for an input to break out of the loop
    while ((~P1IN & RED_BUTTON) && (~P1IN & YELLOW_BUTTON) && (~P1IN & BLUE_BUTTON) && (~P1IN & GREEN_BUTTON) );

    // If the red button was pushed
    if (P1IN & RED_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == RED) {
        // Turn off the top LED
        strip_1[Top_LED_Index] = LED_OFF;
        // Decrement Top_LED_Index
        Top_LED_Index--;
        // Assign the new top lit LED color to Top_LED_Color
        Top_LED_Color = strip_1[Top_LED_Index];
      } else {
        // Penalty
        penalty();
      }
    }

    // If the yellow button was pushed
    if (P1IN & YELLOW_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == YELLOW) {
        // Turn off the top LED
        strip_1[Top_LED_Index] = LED_OFF;
        // Decrement Top_LED_Index
        Top_LED_Index--;
        // Assign the new top lit LED color to Top_LED_Color
        Top_LED_Color = strip_1[Top_LED_Index];
      } else {
        // Penalty
        penalty();
      }
    }

    // If the blue button was pushed
    if (P1IN & BLUE_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == BLUE) {
        // Turn off the top LED
        strip_1[Top_LED_Index] = LED_OFF;
        // Decrement Top_LED_Index
        Top_LED_Index--;
        // Assign the new top lit LED color to Top_LED_Color
        Top_LED_Color = strip_1[Top_LED_Index];
      } else {
        // Penalty
        penalty();
      }
    }

    // If the green button was pushed
    if (P1IN & GREEN_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == GREEN) {
        // Turn off the top LED
        strip_1[Top_LED_Index] = LED_OFF;
        // Decrement Top_LED_Index
        Top_LED_Index--;
        // Assign the new top lit LED color to Top_LED_Color
        Top_LED_Color = strip_1[Top_LED_Index];
      } else {
        // Penalty
        penalty();
      }
    }

    // Wait for input to be get back to zero
    while ( (P1IN & RED_BUTTON) || (P1IN & YELLOW_BUTTON) || (P1IN & BLUE_BUTTON) || (P1IN & GREEN_BUTTON) );

    // UPDATE STRIP LED LIGHTS HERE (ONLY UPDATE THE STRIP LIGHTS AFTER PLAYER TURNED ALL INPUT BACK TO ZERO)
    colorLEDS(strip_1);
    showStrip();
  }

  return 0;
}

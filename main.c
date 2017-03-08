// ABDULLAH ABDULQADER MARCH 2017
#include <msp430.h>

#define START_BUTTON    BIT4

#define RED     0
#define YELLOW  1
#define BLUE    2
#define GREEN   3

#define BLACK   5

#define RED_BUTTON      BIT0
#define YELLOW_BUTTON   BIT1
#define BLUE_BUTTON     BIT2
#define GREEN_BUTTON    BIT3

int seed; // seed for generating a random numbers
int LED_Strip_1[30];        // this will hold the color sequence of strip_1
int LED_Strip_2[30];        // this will hold the color sequence of strip_2
int Top_LED_Index;          // Index of top lit LED. Will be decremented down to 0.
int Top_LED_Color;          // Color of top lit LED

int main(void) {
  TACTL = TASSEL_2 | MC_2;          // start timer in up/down mode

  // Set the direction of the start button. Keep initial input low.
  P1DIR = ~START_BUTTON;
  P1IN  = ~START_BUTTON;

  // Set the direction of color bits as input to the msp430. Keep initial input zero.
  P1DIR &= ~(RED_BUTTON + YELLOW_BUTTON + BLUE_BUTTON + GREEN_BUTTON);
  P1IN  &= ~(RED_BUTTON + YELLOW_BUTTON + BLUE_BUTTON + GREEN_BUTTON);

  // Initialize top index and color
  Top_LED_Index = 29;
  Top_LED_Color = LED_Strip_1[Top_LED_Index];

  // Generate random seed
  while ((~P1IN) & START_BUTTON);   // loop while the START_BUTTON is unset
  TACTL = TASSEL_1 | MC_0;          // stop timer once START_BUTTON is pushed
  seed  = TAR;                       // this is the seed for random(int seed)

  // Generate a random sequence of colors for the LED strips 1 and 2
  int randomColor;
  for (int i = 0; i < 30; i++ ) {
    randomColor = random(seed);
    LED_Strip_1[i] = randomColor;
    LED_Strip_2[i] = randomColor;
    // printf("LED_Strip_1[%d] = %d\n", i, LED_Strip_1[i] );
  }

  // Light up the addressable LED strips

  // Listen to player input. Condition: there are still lit LED(s).
  while(Top_LED_Index >= 0) {
    // wait for an input
    while ((~P1IN & RED_BUTTON) || (~P1IN & YELLOW_BUTTON) ||
            (~P1IN & BLUE_BUTTON) || (~P1IN & GREEN_BUTTON) );

    // If the red button was pushed
    if (P1IN == RED_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == RED) {
        // Turn off the top LED
      } else {
        // Penalty
      }
    }

    // If the yellow button was pushed
    if (P1IN == YELLOW_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == YELLOW) {
        // Turn off the top LED
      } else {
        // Penalty
      }
    }

    // If the blue button was pushed
    if (P1IN == BLUE_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == BLUE) {
        // Turn off the top LED
      } else {
        // Penalty
      }
    }

    // If the green button was pushed
    if (P1IN == GREEN_BUTTON) {
      //check if the top color is red
      if (Top_LED_Color == GREEN) {
        // Turn off the top LED
      } else {
        // Penalty
      }
    }

    // Decrement Top_LED_Index
    Top_LED_Index--;

    // Assign the new top lit LED color to Top_LED_Color
    Top_LED_Color = LED_Strip_1[Top_LED_Index];

    // Reset P1IN
    P1IN &= ~(RED_BUTTON + YELLOW_BUTTON + BLUE_BUTTON + GREEN_BUTTON);
  }

  return 0;
}

// Generate a random number ranging from MIN_RAND to MAX_RAND.
// This function doesn't truely generate a random number but it is good enough
// for the purpose of this porject.
int random(int x) {
  seed++;
  return ((x % 7) + (x % 3)) % 5;
}

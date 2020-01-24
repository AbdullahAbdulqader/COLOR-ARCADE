# COLOR-ARCADE
A multiplayer arcade game developed using Texas Instruments' MSP430G2553 microcontroller. 

Color is a game played by two players, each player is given an LED strip containing 30 lit LEDs and is able to control it. 
The objective is to turn off all 30 LEDs from the strip’s top to its bottom, the first to meet this objective wins. 

![ScreenShot](/color_arcade_imgs/1.jpg?raw=true)

# LED Strip Lights
The MSP430G2553 launchpad, WS2811 LED strips, and colored momentary
switch buttons are the main equipment of this project. The WS2811 RGB LED strips are
addressable, meaning it includes an IC chip in between each and every LED, making it
possible to control each LED individually. Runs on 5V, about 18W per meter.

![ScreenShot](/color_arcade_imgs/3.jpg?raw=true)

# Controllers
I made controllers by mounting the colored buttons on small hand sized boxes,
as shown in figure 2, so that players can easily control the strips. The pins P1.0 - P.3 on
the launchpad are take the controllers' input.

![ScreenShot](/color_arcade_imgs/4.jpg?raw=true)

# Game Logic
To start the game, both players have to push the start button (mounted on the
breadboard) connected to P1.4. For that to work, I had to make both launchpads communicate with each other: P1.5 was used to send out a signal to the other launchpad,
and P1.6 was used to receive a signal from the other launchpad. P1.5 and P1.6 are
used to communicate tell/know if a player is ready to start the game and if the player
has won or lost the game.

![ScreenShot](/color_arcade_imgs/5.jpg?raw=true)

When a player presses a button that does not match with the color of the top lit
LED, a penalty of approximately 2 seconds takes place on that player’s strip where the
strip will turn red and the player will not be able to race his opponent. Then, the strip return back to its state after the each penalty and the player can continue playing.
Once a player has finished the race, the winner’s strip will display rainbow colors
and the loser’s will display a red color. To replay, players have to press the reset button
on the launch pads.

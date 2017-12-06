# Project 3: Arch Pong
## Introduction

For this assignment we had to create a game using the LCD display on the msp430, and the game that I selected was pong. 

## shape-motion-demo
Here everything would take place in order to create the pong game. First all from the code provide to us I got the shape motion to understand better collision and movement, from there I just alter that code so it would fit the necessities of the game, then in order to make a sound when you score or when the ball hits the pad I reused some code from last assingment to make those sounds.

Lastly as the instructions for the assignment stated I created assembly code to move the players called movementPads.s, in file I basically define 4 global variables that would store either a 1 or 0 (representing on and off) to be use in a series of comparisons (bits comparisons) so if the button is press depending of which it would call the function to move either up and down for both players. 

## Additional information
The actual project is inside the shape-motion-demo folder, where the changes took place to create this Arch pong game. So to run the pong game you must use the shape-motion-demo folder.

## Instructions
To play the game:

- On the main menu S1, S2, S3 let's customize the theme of the game
- Once a theme has been selected just press S4 to start playing 
- S1 moves Player 1 go up
- S2 moves Player 1 down
- S3 moves Player 2 go up
- S4 moves Player 2 down
- The game is up to 8 points, so the first to score 8 wins!
- After the game is over you would be taken back to the main menu to play again

## Game program

To run the the game we would have to go to the shape-motion-demo folder then once inside the folder

To compile:
~~~
$ make
~~~

To load the game into the msp430, try:
~~~
$ make load
~~~

To delete:
~~~
$ make clean
~~~



<img src="images/drac.png" width = 1170 height = 250>

# Background

- This project is based on a real board game called "[The Fury of Dracula](https://en.wikipedia.org/wiki/The_Fury_of_Dracula)," which has been modified to simplify the making of the interface
- The ultimate goal was to write AIs that play The Fury of Dracula by creating:
  - The View: The interface of game, where each character has access to their own View abstract data types (ADT)
  - The Hunt: The implementation of AIs 

# Introduction to the game

The game involves the arch-vampire Dracula, four intrepid vampire hunters and the splendid cities of Victorian Europe. The aim for Dracula is to evade the hunters for long enough to realise his Grand Plan - to release hordes of vampires all over Europe, while the aim for the hunters is to kill Dracula before he can do so.

# How the game works

- The goal of the Dracula is to evade the Hunters and last 366 rounds by avoiding them and leaving traps
- The goal of the Hunter is to work as a team to kill the Dracula by finding him, whilst avoiding his traps
- The Dracula knows where the hunters are at all times, but the Hunters do not know where Dracula is
- However, the Dracula leaves clues, such that the Hunter can deduce where he has been
- The game starts at a score of 366 points, and for each round that pass the score diminishes
- The hunters need to kill Dracula before the score reaches 0

## The Map

![map](images/map2.png)

# The Data

The game engine passes the game history to the View's ADT which process it, build a game state, and then makes part of the game state available to the AIs (i.e. gives a 'view' of the game to the AI). 

![The-data-diagram](images/game-processes.png)


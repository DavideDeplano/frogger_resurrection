# frogger_resurrection  

## Description  

Frogger Resurrection is a modern reimagining of the classic Frogger game, developed as part of a university project. The game is written in C and uses the ncurses library to manage the terminal-based user interface on Linux systems.

## Key Features:  

Two versions of the game:

- One version using separate processes.

- One version using threads.

Implementation of keyboard movement via sockets to control the frog.

Use of audio libraries (SDL2 and SDL2-Mixer) for sound playback.

## Objective:  

The project aimed to explore and implement concepts of concurrency and inter-process communication in a Linux environment, focusing on efficient resource management through processes and threads.

## Technologies Used:  

- Programming Language: C

- Library: ncurses (for terminal management)

- Audio Libraries:

  - libsdl2

  - libsdl2-mixer

- Sockets for frog movement handling

- Linux (operating system)

## How to Run the Game  

1. Clone the repository:
    
   `git clone https://github.com/DavideDeplano/frogger_resurrection.git`

2. Audio Libraries Required:
   If you're using audio, make sure to install the libsdl2 and libsdl2-mixer libraries:
    
   `sudo apt-get install libsdl2 libsdl2-mixer`


3. Choose the version:
   - For the separate processes version, use one of the following commands:  
    `cd versione_processi`

    - For the threads version, use one of the following commands:  
    `cd versione_processi`
   
4. Compile the Project:  
   The project includes a makefile that handles compilation for both versions of the game. To compile, run the following command:
   
   `make`

5. Compile the keybord:
   The project includes a keybord movement via socket to control the frog. To compile, run the following command:
   
   `gcc client.c -o client.out -lncurses`
   

6. Run the Game:
   After compiling, start the game with:

   `./frogger_resurrection.out`

## License
This project is distributed under the MIT License.

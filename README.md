
Pac-Man Game (Raylib - C)

A classic Pac-Man arcade game clone developed in C using the Raylib graphics library.
The game recreates the original Pac-Man mechanics including ghosts with different behaviors, power pellets, fruits, scoring, and multiple levels.

🎮 Features

🟡 Classic Pac-Man gameplay

👻 Four Ghosts with different AI

Blinky (Red)

Pinky (Pink)

Inky (Blue)

Clyde (Orange)

💊 Power Pellets

Ghosts turn frightened

Pac-Man can eat ghosts for extra points

🍒 Bonus Fruits

Appear mid-level

Give extra points

🌈 Rainbow Mode

Maze colors change when a new high score is achieved

🔊 Sound Effects

Chomp sound

Death sound

Fruit sound

Ghost eaten sound

Intro sound

🧠 Ghost AI

Scatter Mode

Chase Mode

Frightened Mode

Respawn Mode

❤️ Lives System

📈 Score & High Score System

🧱 Classic Maze Layout

🎯 Level Progression

🕹️ Controls
Key	Action
⬆️ Arrow Up	Move Up
⬇️ Arrow Down	Move Down
⬅️ Arrow Left	Move Left
➡️ Arrow Right	Move Right
Space	Start / Restart Game
🧩 Game Mechanics
Dots

Small dots give 10 points

Power Pellets

Large glowing pellets give 50 points

Ghosts become frightened

Pac-Man can eat ghosts

Ghost Points

Eating ghosts gives increasing score:

Ghost	Points
1st	200
2nd	400
3rd	800
4th	1600
👻 Ghost Behaviors
🔴 Blinky

Directly chases Pac-Man.

🩷 Pinky

Targets a position ahead of Pac-Man's direction.

🔵 Inky

Uses a vector between Pac-Man and Blinky to calculate target.

🟠 Clyde

Chases Pac-Man when far away but returns to scatter corner when close.

🧱 Maze Tiles
Tile	Meaning
0	Empty path
1	Wall
2	Dot
3	Power Pellet
4	Ghost House
🍒 Fruit System

Fruits appear when half the dots are eaten.

Level	Points
1	100
2	300
3	500
4	700
5	1000
6	2000
7	3000
8	5000
🛠️ Technologies Used

C Programming Language

Raylib Graphics Library

Raylib Audio System

Game Loop Architecture

📂 Project Structure
pacman/
│
├── main.c
├── spritesheet.png
├── chomp.wav
├── death.wav
├── eat_fruit.wav
├── eat_ghost.wav
├── ghost_running_away.wav
├── Intro.wav
├── power_siren.wav
├── ghost_turn_blue.wav
├── high_score.wav
└── README.md
⚙️ How to Run
1️⃣ Install Raylib

Linux (Ubuntu example)

sudo apt install libraylib-dev

Mac (Homebrew)

brew install raylib

Windows
Download Raylib from:

https://www.raylib.com/

2️⃣ Compile

Example using gcc

gcc main.c -o pacman -lraylib -lopengl32 -lgdi32 -lwinmm
3️⃣ Run
./pacman
📸 Gameplay

The game includes:

Animated Pac-Man

Intelligent ghosts

Dynamic maze coloring

Retro arcade style gameplay

📚 Learning Goals

This project demonstrates:

Game loops

AI pathfinding logic

State machines

Collision detection

Sprite and sound handling

Structured C programming

🚀 Future Improvements

Possible upgrades:

Add sprite animations

Add menu system

Add ghost house door animation

Add scoreboard file saving

Add joystick/controller support

Add better ghost pathfinding (A*)

👩‍💻 Author

Prerana Gupta

Computer Science Student
Khulna University

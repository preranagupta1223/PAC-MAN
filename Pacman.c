#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define SCREEN_WIDTH 672
#define SCREEN_HEIGHT 864
#define TILE_SIZE 24
#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define GHOST_COUNT 4
#define MAX_LIVES 3

typedef enum {
    STATE_INTRO,
    STATE_READY,
    STATE_PLAYING,
    STATE_DEATH,
    STATE_LEVEL_COMPLETE,
    STATE_GAME_OVER
} GameState;

typedef enum {
    GHOST_BLINKY = 0,
    GHOST_PINKY = 1,
    GHOST_INKY = 2,
    GHOST_CLYDE = 3
} GhostType;

typedef enum {
    MODE_SCATTER,
    MODE_CHASE,
    MODE_FRIGHTENED,
    MODE_EATEN,
    MODE_RESPAWNING  
} GhostMode;

typedef enum {
    DIR_NONE = 0,
    DIR_UP = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3,
    DIR_RIGHT = 4
} Direction;

typedef struct {
    float x, y;
    Direction direction;
    Direction nextDirection;
    float speed;
    int tileX, tileY;
} Entity;

typedef struct {
    Entity entity;
    GhostType type;
    GhostMode mode;
    int scatterX, scatterY;
    float modeTimer;
    float frightenedTimer;
    Color color;
    float releaseTimer;
    bool released;
    bool inHouse;
    float homeX, homeY;       
    float respawnTimer;       
} Ghost;

typedef struct {
    Entity pacman;
    Ghost ghosts[GHOST_COUNT];
    int maze[MAZE_HEIGHT][MAZE_WIDTH];
    int score;
    int highScore;
    int lives;
    int level;
    int dotsEaten;
    int totalDots;
    int powerPelletActive;
    float powerPelletTimer;
    int ghostsEaten;
    float ghostScoreTimer;
    int ghostScoreValue;
    float ghostScoreX, ghostScoreY;
    GameState state;
    float stateTimer;
    int fruitSpawned;
    int fruitEaten;
    float fruitX, fruitY;
    float fruitTimer;
    bool cruiseElroy;
    float sirenTimer;
    bool rainbowMode;          
    float rainbowTimer;        
} Game;

Game game;
Texture2D spritesheet;
Sound soundChomp;
Sound soundDeath;
Sound soundEatFruit;
Sound soundEatGhost;
Sound soundGhostRunning;
Sound soundIntro;
Sound soundPowerSiren;
Sound soundGhostTurnBlue;
Sound soundHighScore;

int mazeTemplate[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,3,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,3,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,4,4,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,1,4,4,4,4,4,4,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,0,2,0,0,0,1,4,4,4,4,4,4,1,0,0,0,2,0,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,4,4,4,4,4,4,1,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,3,2,2,1,1,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,1,1,2,2,3,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int fruitPoints[] = { 100, 300, 500, 700, 1000, 2000, 3000, 5000 };

void InitMaze(void);
void ResetLevel(void);
void ResetPositions(void);
bool CanMove(int x, int y, Direction dir);
bool CanPacmanMove(int x, int y, Direction dir);
bool IsWall(int x, int y);
Direction GetBestDirection(int currentX, int currentY, int targetX, int targetY, Direction currentDir);


Color GetMazeColor(int level, bool isOuter) {

    int colorIndex = (level - 1) % 6;

    Color outerColors[] = {
        {40, 60, 180, 180},    // Level 1: Dark Blue
        {40, 160, 60, 180},    // Level 2: Dark Green
        {180, 40, 40, 180},    // Level 3: Dark Red
        {180, 100, 40, 180},   // Level 4: Dark Orange
        {140, 40, 180, 180},   // Level 5: Dark Purple
        {180, 160, 40, 180}    // Level 6: Dark Yellow
    };

    Color innerColors[] = {
        {80, 150, 255, 255},   // Level 1: Bright Blue
        {80, 255, 120, 255},   // Level 2: Bright Green
        {255, 80, 80, 255},    // Level 3: Bright Red
        {255, 180, 80, 255},   // Level 4: Bright Orange
        {220, 80, 255, 255},   // Level 5: Bright Purple
        {255, 240, 80, 255}    // Level 6: Bright Yellow
    };

    return isOuter ? outerColors[colorIndex] : innerColors[colorIndex];
}


Color GetRainbowColor(float time) {
    float hue = fmodf(time * 2.0f, 6.0f);
    int section = (int)hue;
    float fraction = hue - section;

    switch (section) {
    case 0: return (Color) { 255, (int)(fraction * 255), 0, 255 };        // Red to Yellow
    case 1: return (Color) { (int)((1 - fraction) * 255), 255, 0, 255 };    // Yellow to Green
    case 2: return (Color) { 0, 255, (int)(fraction * 255), 255 };        // Green to Cyan
    case 3: return (Color) { 0, (int)((1 - fraction) * 255), 255, 255 };    // Cyan to Blue
    case 4: return (Color) { (int)(fraction * 255), 0, 255, 255 };        // Blue to Magenta
    default: return (Color) { 255, 0, (int)((1 - fraction) * 255), 255 };   // Magenta to Red
    }
}

void InitGame(void) {
    int savedHighScore = game.highScore;
    game.score = 0;
    game.highScore = savedHighScore;
    game.lives = MAX_LIVES;
    game.level = 1;
    game.state = STATE_INTRO;
    game.stateTimer = 0;
    game.sirenTimer = 0;
    game.rainbowMode = false;      
    game.rainbowTimer = 0.0f;      
    InitMaze();
    ResetLevel();
    PlaySound(soundIntro);
}

void InitMaze(void) {
    game.totalDots = 0;
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            game.maze[y][x] = mazeTemplate[y][x];
            if (game.maze[y][x] == 2 || game.maze[y][x] == 3) {
                game.totalDots++;
            }
        }
    }
}

void ResetLevel(void) {
    game.dotsEaten = 0;
    game.powerPelletActive = 0;
    game.ghostsEaten = 0;
    game.fruitSpawned = 0;
    game.fruitEaten = 0;
    game.cruiseElroy = false;
    ResetPositions();
}

void ResetPositions(void) {
    game.pacman.tileX = 13;
    game.pacman.tileY = 23;
    game.pacman.x = (float)(game.pacman.tileX * TILE_SIZE);
    game.pacman.y = (float)(game.pacman.tileY * TILE_SIZE);
    game.pacman.direction = DIR_NONE;
    game.pacman.nextDirection = DIR_NONE;
    game.pacman.speed = 2.0f;

    int ghostStartTiles[4][2] = { {13, 11}, {13, 14}, {11, 14}, {15, 14} };
    Color ghostColors[4] = { RED, PINK, SKYBLUE, ORANGE };
    int scatterTargets[4][2] = { {24, 1}, {3, 1}, {24, 29}, {3, 29} };
    float releaseTimes[4] = { 0.0f, 2.0f, 4.0f, 6.0f };

    for (int i = 0; i < GHOST_COUNT; i++) {
        game.ghosts[i].entity.tileX = ghostStartTiles[i][0];
        game.ghosts[i].entity.tileY = ghostStartTiles[i][1];
        game.ghosts[i].entity.x = (float)(game.ghosts[i].entity.tileX * TILE_SIZE);
        game.ghosts[i].entity.y = (float)(game.ghosts[i].entity.tileY * TILE_SIZE);
        game.ghosts[i].entity.direction = DIR_LEFT;
        game.ghosts[i].entity.speed = 1.5f;
        game.ghosts[i].type = (GhostType)i;
        game.ghosts[i].mode = MODE_SCATTER;
        game.ghosts[i].modeTimer = 7.0f;
        game.ghosts[i].scatterX = scatterTargets[i][0];
        game.ghosts[i].scatterY = scatterTargets[i][1];
        game.ghosts[i].color = ghostColors[i];
        game.ghosts[i].releaseTimer = releaseTimes[i];
        game.ghosts[i].released = (i == 0);
        game.ghosts[i].inHouse = (i != 0);


        game.ghosts[i].homeX = 13.0f * TILE_SIZE;
        game.ghosts[i].homeY = 14.0f * TILE_SIZE;
        game.ghosts[i].respawnTimer = 0.0f;
    }
}

bool IsWall(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) return false;
    return (game.maze[y][x] == 1);
}

bool CanMove(int x, int y, Direction dir) {
    int nextX = x;
    int nextY = y;

    if (dir == DIR_UP) nextY--;
    else if (dir == DIR_DOWN) nextY++;
    else if (dir == DIR_LEFT) nextX--;
    else if (dir == DIR_RIGHT) nextX++;
    else return false;

    if (nextY == 14 && (nextX < 0 || nextX >= MAZE_WIDTH)) return true;
    if (nextX < 0 || nextX >= MAZE_WIDTH || nextY < 0 || nextY >= MAZE_HEIGHT) return false;

    return !IsWall(nextX, nextY);
}

bool CanPacmanMove(int x, int y, Direction dir) {
    int nextX = x;
    int nextY = y;

    if (dir == DIR_UP) nextY--;
    else if (dir == DIR_DOWN) nextY++;
    else if (dir == DIR_LEFT) nextX--;
    else if (dir == DIR_RIGHT) nextX++;
    else return false;

    if (nextY == 14 && (nextX < 0 || nextX >= MAZE_WIDTH)) return true;
    if (nextX < 0 || nextX >= MAZE_WIDTH || nextY < 0 || nextY >= MAZE_HEIGHT) return false;

    int tile = game.maze[nextY][nextX];
    return (tile != 1 && tile != 4);
}

Direction GetBestDirection(int currentX, int currentY, int targetX, int targetY, Direction currentDir) {
    Direction dirs[4] = { DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT };
    Direction opposite[5] = { DIR_NONE, DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

    float bestDist = 999999.0f;
    Direction bestDir = currentDir;

    for (int i = 0; i < 4; i++) {
        if (dirs[i] == opposite[currentDir]) continue;

        if (CanMove(currentX, currentY, dirs[i])) {
            int nextX = currentX;
            int nextY = currentY;

            if (dirs[i] == DIR_UP) nextY--;
            else if (dirs[i] == DIR_DOWN) nextY++;
            else if (dirs[i] == DIR_LEFT) nextX--;
            else if (dirs[i] == DIR_RIGHT) nextX++;

            int dx = nextX - targetX;
            int dy = nextY - targetY;
            float dist = sqrtf((float)(dx * dx + dy * dy));

            if (dist < bestDist || (dist == bestDist && dirs[i] == DIR_UP)) {
                bestDist = dist;
                bestDir = dirs[i];
            }
        }
    }

    return bestDir;
}

void UpdatePacman(void) {
    if (IsKeyDown(KEY_UP)) game.pacman.nextDirection = DIR_UP;
    else if (IsKeyDown(KEY_DOWN)) game.pacman.nextDirection = DIR_DOWN;
    else if (IsKeyDown(KEY_LEFT)) game.pacman.nextDirection = DIR_LEFT;
    else if (IsKeyDown(KEY_RIGHT)) game.pacman.nextDirection = DIR_RIGHT;

    int centerX = (int)(game.pacman.x + TILE_SIZE / 2);
    int centerY = (int)(game.pacman.y + TILE_SIZE / 2);
    game.pacman.tileX = centerX / TILE_SIZE;
    game.pacman.tileY = centerY / TILE_SIZE;

    bool alignedX = ((int)game.pacman.x % TILE_SIZE) == 0;
    bool alignedY = ((int)game.pacman.y % TILE_SIZE) == 0;
    bool aligned = alignedX && alignedY;

    if (aligned && game.pacman.nextDirection != DIR_NONE) {
        if (CanPacmanMove(game.pacman.tileX, game.pacman.tileY, game.pacman.nextDirection)) {
            game.pacman.direction = game.pacman.nextDirection;
        }
    }

    if (!aligned || CanPacmanMove(game.pacman.tileX, game.pacman.tileY, game.pacman.direction)) {
        if (game.pacman.direction == DIR_UP) game.pacman.y -= game.pacman.speed;
        else if (game.pacman.direction == DIR_DOWN) game.pacman.y += game.pacman.speed;
        else if (game.pacman.direction == DIR_LEFT) game.pacman.x -= game.pacman.speed;
        else if (game.pacman.direction == DIR_RIGHT) game.pacman.x += game.pacman.speed;
    }
    else {
        game.pacman.x = (float)(game.pacman.tileX * TILE_SIZE);
        game.pacman.y = (float)(game.pacman.tileY * TILE_SIZE);
        game.pacman.direction = DIR_NONE;
    }

    if (game.pacman.tileY == 14) {
        if (game.pacman.x < 0) {
            game.pacman.x = (float)((MAZE_WIDTH - 1) * TILE_SIZE);
            game.pacman.tileX = MAZE_WIDTH - 1;
        }
        else if (game.pacman.x >= MAZE_WIDTH * TILE_SIZE) {
            game.pacman.x = 0;
            game.pacman.tileX = 0;
        }
    }

    centerX = (int)(game.pacman.x + TILE_SIZE / 2);
    centerY = (int)(game.pacman.y + TILE_SIZE / 2);
    game.pacman.tileX = centerX / TILE_SIZE;
    game.pacman.tileY = centerY / TILE_SIZE;

    if (game.pacman.tileX < 0) game.pacman.tileX = 0;
    if (game.pacman.tileX >= MAZE_WIDTH) game.pacman.tileX = MAZE_WIDTH - 1;
    if (game.pacman.tileY < 0) game.pacman.tileY = 0;
    if (game.pacman.tileY >= MAZE_HEIGHT) game.pacman.tileY = MAZE_HEIGHT - 1;

    if (game.maze[game.pacman.tileY][game.pacman.tileX] == 2) {
        game.maze[game.pacman.tileY][game.pacman.tileX] = 0;
        game.score += 10;


        if (game.score > game.highScore && game.score - 10 <= game.highScore) {
            game.rainbowMode = true;
            game.rainbowTimer = 0.0f;
        }

        game.dotsEaten++;
        PlaySound(soundChomp);
    }
    else if (game.maze[game.pacman.tileY][game.pacman.tileX] == 3) {
        game.maze[game.pacman.tileY][game.pacman.tileX] = 0;
        game.score += 50;


        if (game.score > game.highScore && game.score - 50 <= game.highScore) {
            game.rainbowMode = true;
            game.rainbowTimer = 0.0f;
        }

        game.dotsEaten++;
        game.powerPelletActive = 1;
        game.powerPelletTimer = 6.0f;
        game.ghostsEaten = 0;

        PlaySound(soundGhostTurnBlue);
        StopSound(soundPowerSiren);

        for (int i = 0; i < GHOST_COUNT; i++) {
            if (game.ghosts[i].mode != MODE_EATEN &&
                game.ghosts[i].mode != MODE_RESPAWNING &&
                game.ghosts[i].released && !game.ghosts[i].inHouse) {
                game.ghosts[i].mode = MODE_FRIGHTENED;
                game.ghosts[i].frightenedTimer = 6.0f;
            }
        }
    }

    if (game.fruitSpawned && !game.fruitEaten) {
        if (game.pacman.tileX == (int)game.fruitX && game.pacman.tileY == (int)game.fruitY) {
            int fruitIndex = (game.level - 1) % 8;
            int points = fruitPoints[fruitIndex];
            game.score += points;


            if (game.score > game.highScore && game.score - points <= game.highScore) {
                game.rainbowMode = true;
                game.rainbowTimer = 0.0f;
            }

            game.fruitEaten = 1;
            PlaySound(soundEatFruit);
        }
    }

    for (int i = 0; i < GHOST_COUNT; i++) {
        float dist = sqrtf(powf(game.pacman.x - game.ghosts[i].entity.x, 2) +
            powf(game.pacman.y - game.ghosts[i].entity.y, 2));
        if (dist < TILE_SIZE * 0.7f) {
            if (game.ghosts[i].mode == MODE_FRIGHTENED) {
                int points[] = { 200, 400, 800, 1600 };
                int earnedPoints = points[game.ghostsEaten];
                game.score += earnedPoints;

                if (game.score > game.highScore && game.score - earnedPoints <= game.highScore) {
                    game.rainbowMode = true;
                    game.rainbowTimer = 0.0f;
                }

                game.ghostScoreValue = earnedPoints;
                game.ghostScoreX = game.ghosts[i].entity.x;
                game.ghostScoreY = game.ghosts[i].entity.y;
                game.ghostScoreTimer = 1.0f;
                game.ghostsEaten++;


                game.ghosts[i].mode = MODE_EATEN;
                PlaySound(soundEatGhost);
            }
            else if (game.ghosts[i].mode != MODE_EATEN && game.ghosts[i].mode != MODE_RESPAWNING) {
                game.state = STATE_DEATH;
                game.stateTimer = 0;
                PlaySound(soundDeath);
                StopSound(soundPowerSiren);
                return;
            }
        }
    }
}

void UpdateGhost(Ghost* ghost) {

    if (ghost->mode == MODE_RESPAWNING) {
        ghost->respawnTimer -= GetFrameTime();
        if (ghost->respawnTimer <= 0) {

            ghost->mode = MODE_SCATTER;
            ghost->modeTimer = 7.0f;
            ghost->inHouse = false;
            ghost->released = true;
        }
        return;
    }


    if (ghost->mode == MODE_EATEN) {

        float dirX = ghost->homeX - ghost->entity.x;
        float dirY = ghost->homeY - ghost->entity.y;
        float distance = sqrtf(dirX * dirX + dirY * dirY);

        if (distance < 5.0f) {

            ghost->entity.x = ghost->homeX;
            ghost->entity.y = ghost->homeY;


            ghost->mode = MODE_RESPAWNING;
            ghost->respawnTimer = 3.0f;
            ghost->inHouse = true;
            return;
        }


        float speed = 4.0f;
        if (distance > 0) {
            dirX /= distance; 
            dirY /= distance;
        }

        ghost->entity.x += dirX * speed;
        ghost->entity.y += dirY * speed;


        ghost->entity.tileX = (int)(ghost->entity.x / TILE_SIZE);
        ghost->entity.tileY = (int)(ghost->entity.y / TILE_SIZE);

        return;
    }

    if (ghost->mode == MODE_SCATTER || ghost->mode == MODE_CHASE) {
        ghost->modeTimer -= GetFrameTime();
        if (ghost->modeTimer <= 0) {
            ghost->mode = (ghost->mode == MODE_SCATTER) ? MODE_CHASE : MODE_SCATTER;
            ghost->modeTimer = (ghost->mode == MODE_SCATTER) ? 7.0f : 20.0f;
        }
    }

    if (ghost->mode == MODE_FRIGHTENED) {
        ghost->frightenedTimer -= GetFrameTime();
        if (ghost->frightenedTimer <= 0) {
            ghost->mode = MODE_SCATTER;
            ghost->modeTimer = 7.0f;
        }
    }

    if (ghost->entity.y < 0) ghost->entity.y = 0;
    if (ghost->entity.y >= (MAZE_HEIGHT - 1) * TILE_SIZE) {
        ghost->entity.y = (float)((MAZE_HEIGHT - 2) * TILE_SIZE);
    }
    if (ghost->entity.x < -TILE_SIZE && ghost->entity.tileY != 14) ghost->entity.x = 0;
    if (ghost->entity.x >= MAZE_WIDTH * TILE_SIZE && ghost->entity.tileY != 14) {
        ghost->entity.x = (float)((MAZE_WIDTH - 1) * TILE_SIZE);
    }

    ghost->entity.tileX = (int)(ghost->entity.x / TILE_SIZE);
    ghost->entity.tileY = (int)(ghost->entity.y / TILE_SIZE);

    if (ghost->entity.tileX < 0) ghost->entity.tileX = 0;
    if (ghost->entity.tileX >= MAZE_WIDTH) ghost->entity.tileX = MAZE_WIDTH - 1;
    if (ghost->entity.tileY < 0) ghost->entity.tileY = 0;
    if (ghost->entity.tileY >= MAZE_HEIGHT) ghost->entity.tileY = MAZE_HEIGHT - 1;

    bool alignedX = ((int)ghost->entity.x % TILE_SIZE) == 0;
    bool alignedY = ((int)ghost->entity.y % TILE_SIZE) == 0;

    if (alignedX && alignedY) {
        int targetX = 13;
        int targetY = 11;

        if (ghost->inHouse) {
            targetX = 13;
            targetY = 11;

            if (ghost->entity.tileX == 13 && ghost->entity.tileY == 11) {
                ghost->inHouse = false;
            }
        }
        else {
            int dx = ghost->entity.tileX - game.pacman.tileX;
            int dy = ghost->entity.tileY - game.pacman.tileY;
            float dist = sqrtf((float)(dx * dx + dy * dy));

            if (ghost->mode == MODE_FRIGHTENED) {
                targetX = game.pacman.tileX < MAZE_WIDTH / 2 ? MAZE_WIDTH - 1 : 0;
                targetY = game.pacman.tileY < MAZE_HEIGHT / 2 ? MAZE_HEIGHT - 1 : 0;
            }
            else if (dist < 10.0f) {
                if (ghost->type == GHOST_BLINKY) {
                    targetX = game.pacman.tileX;
                    targetY = game.pacman.tileY;
                }
                else if (ghost->type == GHOST_PINKY) {
                    targetX = game.pacman.tileX;
                    targetY = game.pacman.tileY;
                    if (game.pacman.direction == DIR_UP) { targetY -= 4; targetX -= 4; }
                    else if (game.pacman.direction == DIR_DOWN) targetY += 4;
                    else if (game.pacman.direction == DIR_LEFT) targetX -= 4;
                    else if (game.pacman.direction == DIR_RIGHT) targetX += 4;
                }
                else if (ghost->type == GHOST_INKY) {
                    int pivotX = game.pacman.tileX;
                    int pivotY = game.pacman.tileY;
                    if (game.pacman.direction == DIR_UP) { pivotY -= 2; pivotX -= 2; }
                    else if (game.pacman.direction == DIR_DOWN) pivotY += 2;
                    else if (game.pacman.direction == DIR_LEFT) pivotX -= 2;
                    else if (game.pacman.direction == DIR_RIGHT) pivotX += 2;

                    int blinkyX = game.ghosts[GHOST_BLINKY].entity.tileX;
                    int blinkyY = game.ghosts[GHOST_BLINKY].entity.tileY;
                    targetX = pivotX + (pivotX - blinkyX);
                    targetY = pivotY + (pivotY - blinkyY);
                }
                else if (ghost->type == GHOST_CLYDE) {
                    if (dist > 4.0f) {
                        targetX = game.pacman.tileX;
                        targetY = game.pacman.tileY;
                    }
                    else {
                        targetX = ghost->scatterX;
                        targetY = ghost->scatterY;
                    }
                }
            }
            else {
                targetX = ghost->scatterX;
                targetY = ghost->scatterY;
            }
        }

        if (targetX < 0) targetX = 0;
        if (targetX >= MAZE_WIDTH) targetX = MAZE_WIDTH - 1;
        if (targetY < 0) targetY = 0;
        if (targetY >= MAZE_HEIGHT) targetY = MAZE_HEIGHT - 1;

        ghost->entity.direction = GetBestDirection(ghost->entity.tileX, ghost->entity.tileY,
            targetX, targetY, ghost->entity.direction);
    }

    float speed = ghost->entity.speed;
    if (ghost->mode == MODE_FRIGHTENED) speed *= 0.5f;

    if (ghost->entity.direction == DIR_UP) ghost->entity.y -= speed;
    else if (ghost->entity.direction == DIR_DOWN) ghost->entity.y += speed;
    else if (ghost->entity.direction == DIR_LEFT) ghost->entity.x -= speed;
    else if (ghost->entity.direction == DIR_RIGHT) ghost->entity.x += speed;

    if (ghost->entity.tileY == 14) {
        if (ghost->entity.x < 0) {
            ghost->entity.x = (float)((MAZE_WIDTH - 1) * TILE_SIZE);
        }
        else if (ghost->entity.x >= MAZE_WIDTH * TILE_SIZE) {
            ghost->entity.x = TILE_SIZE;
        }
    }

    if (ghost->entity.y < 0) ghost->entity.y = 0;
    if (ghost->entity.y >= (MAZE_HEIGHT - 1) * TILE_SIZE) {
        ghost->entity.y = (float)((MAZE_HEIGHT - 2) * TILE_SIZE);
    }
}

void UpdateGhosts(void) {
    bool anyHunting = false;
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (game.ghosts[i].released && !game.ghosts[i].inHouse &&
            game.ghosts[i].mode != MODE_FRIGHTENED &&
            game.ghosts[i].mode != MODE_EATEN &&
            game.ghosts[i].mode != MODE_RESPAWNING) {
            anyHunting = true;
            break;
        }
    }

    if (anyHunting) {
        game.sirenTimer += GetFrameTime();
        if (game.sirenTimer > 3.0f && !IsSoundPlaying(soundPowerSiren)) {
            PlaySound(soundPowerSiren);
            game.sirenTimer = 0;
        }
    }
    else {
        if (IsSoundPlaying(soundPowerSiren)) {
            StopSound(soundPowerSiren);
        }
        game.sirenTimer = 0;
    }

    for (int i = 0; i < GHOST_COUNT; i++) {
        if (!game.ghosts[i].released) {
            game.ghosts[i].releaseTimer -= GetFrameTime();
            if (game.ghosts[i].releaseTimer <= 0) {
                game.ghosts[i].released = true;
                game.ghosts[i].inHouse = true;
            }
            else {
                continue;
            }
        }
        UpdateGhost(&game.ghosts[i]);
    }
}

void UpdateGame(void) {
    game.stateTimer += GetFrameTime();

    if (game.state == STATE_INTRO) {
      
        if (!IsSoundPlaying(soundIntro) || IsKeyPressed(KEY_SPACE)) {
            game.state = STATE_READY;
            game.stateTimer = 0;
            StopSound(soundIntro);
        }
    }

    else if (game.state == STATE_READY) {
        if (game.stateTimer > 2.0f) {
            game.state = STATE_PLAYING;
            game.stateTimer = 0;
        }
    }
    else if (game.state == STATE_PLAYING) {
        UpdatePacman();
        UpdateGhosts();

        if (game.rainbowMode) {
            game.rainbowTimer += GetFrameTime();
            if (game.rainbowTimer > 5.0f) {
                game.rainbowMode = false;
            }
        }

        if (game.dotsEaten >= game.totalDots) {
            game.state = STATE_LEVEL_COMPLETE;
            game.stateTimer = 0;
            StopSound(soundPowerSiren);
        }

        if (!game.fruitSpawned && game.dotsEaten >= game.totalDots / 2) {
            game.fruitSpawned = 1;
            game.fruitX = 13.0f;
            game.fruitY = 17.0f;
            game.fruitTimer = 10.0f;
        }

        if (game.fruitSpawned && !game.fruitEaten) {
            game.fruitTimer -= GetFrameTime();
            if (game.fruitTimer <= 0) game.fruitSpawned = 0;
        }

        if (game.powerPelletActive) {
            game.powerPelletTimer -= GetFrameTime();
            if (game.powerPelletTimer <= 0) {
                game.powerPelletActive = 0;
                game.ghostsEaten = 0;
            }
        }

        if (game.ghostScoreTimer > 0) {
            game.ghostScoreTimer -= GetFrameTime();
        }
    }
    else if (game.state == STATE_DEATH) {
        if (game.stateTimer > 2.0f) {
            game.lives--;
            if (game.lives <= 0) {
                game.state = STATE_GAME_OVER;
                if (game.score > game.highScore) {
                    game.highScore = game.score;
                    PlaySound(soundHighScore);
                }
            }
            else {
                ResetPositions();
                game.state = STATE_READY;
            }
            game.stateTimer = 0;
        }
    }
    else if (game.state == STATE_LEVEL_COMPLETE) {
        if (game.stateTimer > 2.0f) {
            game.level++;
            InitMaze();
            ResetLevel();
            game.state = STATE_READY;
            game.stateTimer = 0;
        }
    }
    else if (game.state == STATE_GAME_OVER) {
        if (game.stateTimer > 3.0f && IsKeyPressed(KEY_SPACE)) {
            InitGame();
        }
    }
}

void DrawMaze(void) {
    Color outerColor, innerColor;

    if (game.rainbowMode) {
        outerColor = GetRainbowColor(game.rainbowTimer);
        innerColor = GetRainbowColor(game.rainbowTimer + 1.0f);
    }
    else {
        outerColor = GetMazeColor(game.level, true);
        innerColor = GetMazeColor(game.level, false);
    }


    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            int tile = game.maze[y][x];

            if (tile == 1) {
                int posX = x * TILE_SIZE;
                int posY = y * TILE_SIZE;

                DrawRectangleLinesEx((Rectangle) { posX, posY, TILE_SIZE, TILE_SIZE }, 3, outerColor);
                DrawRectangleLinesEx((Rectangle) { posX + 1, posY + 1, TILE_SIZE - 2, TILE_SIZE - 2 }, 2, innerColor);

            }
            else if (tile == 2) {
                DrawCircle(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2, 3, (Color) { 255, 255, 255, 100 });
                DrawCircle(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2, 2, WHITE);

            }
            else if (tile == 3) {
                float pulse = (sinf((float)GetTime() * 8.0f) + 1.0f) / 2.0f;
                int glowRadius = 8 + (int)(pulse * 4);
                int coreRadius = 4 + (int)(pulse * 2);

                DrawCircle(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                    glowRadius, (Color) { 255, 255, 200, 60 });
  
                DrawCircle(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                    coreRadius, WHITE);
            }
        }
    }
}

void DrawGame(void) {
    DrawMaze();

    if (game.fruitSpawned && !game.fruitEaten) {
        DrawCircle((int)(game.fruitX * TILE_SIZE + TILE_SIZE / 2),
            (int)(game.fruitY * TILE_SIZE + TILE_SIZE / 2),
            (float)(TILE_SIZE / 3), YELLOW);
    }

    for (int i = 0; i < GHOST_COUNT; i++) {
        // 👀 If ghost is eaten - only draw eyes
        if (game.ghosts[i].mode == MODE_EATEN) {
            int cx = (int)(game.ghosts[i].entity.x + TILE_SIZE / 2);
            int cy = (int)(game.ghosts[i].entity.y + TILE_SIZE / 2);

            // White eye circles
            DrawCircle(cx - 6, cy - 4, 5, WHITE);
            DrawCircle(cx + 6, cy - 4, 5, WHITE);

            // Blue pupils
            DrawCircle(cx - 6, cy - 4, 3, BLUE);
            DrawCircle(cx + 6, cy - 4, 3, BLUE);

            continue;  // Don't draw body
        }

        // 😴 Don't draw ghost if it's respawning (waiting invisible at home)
        if (game.ghosts[i].mode == MODE_RESPAWNING) {
            continue;
        }

        // Normal ghost rendering
        Color color = game.ghosts[i].color;

        if (game.ghosts[i].mode == MODE_FRIGHTENED) {
            if (game.ghosts[i].frightenedTimer < 2.0f && ((int)(game.ghosts[i].frightenedTimer * 8) % 2 == 0)) {
                color = WHITE;
            }
            else {
                color = DARKBLUE;
            }
        }

        int cx = (int)(game.ghosts[i].entity.x + TILE_SIZE / 2);
        int cy = (int)(game.ghosts[i].entity.y + TILE_SIZE / 2);

        DrawCircle(cx, cy, (float)(TILE_SIZE / 2), color);
        DrawRectangle((int)game.ghosts[i].entity.x, (int)(game.ghosts[i].entity.y + TILE_SIZE / 2),
            TILE_SIZE, TILE_SIZE / 2, color);

        for (int j = 0; j < 3; j++) {
            DrawCircle((int)(game.ghosts[i].entity.x + (j + 0.5f) * TILE_SIZE / 3),
                (int)(game.ghosts[i].entity.y + TILE_SIZE - 2),
                TILE_SIZE / 6, color);
        }

        // Draw eyes (both normal and frightened ghosts have eyes)
        DrawCircle((int)(game.ghosts[i].entity.x + TILE_SIZE / 3),
            (int)(game.ghosts[i].entity.y + TILE_SIZE / 3), 3, WHITE);
        DrawCircle((int)(game.ghosts[i].entity.x + 2 * TILE_SIZE / 3),
            (int)(game.ghosts[i].entity.y + TILE_SIZE / 3), 3, WHITE);

        // Pupils look down when frightened (scared look), normal direction otherwise
        if (game.ghosts[i].mode == MODE_FRIGHTENED) {
            // Scared eyes - pupils looking down
            DrawCircle((int)(game.ghosts[i].entity.x + TILE_SIZE / 3),
                (int)(game.ghosts[i].entity.y + TILE_SIZE / 3 + 2), 1, PINK);
            DrawCircle((int)(game.ghosts[i].entity.x + 2 * TILE_SIZE / 3),
                (int)(game.ghosts[i].entity.y + TILE_SIZE / 3 + 2), 1, PINK);

            // 😢 Sad frown mouth - wavy downward curve
            int mouthY = (int)(game.ghosts[i].entity.y + 2 * TILE_SIZE / 3);
            int mouthCenterX = (int)(game.ghosts[i].entity.x + TILE_SIZE / 2);

            // Draw a sad wavy mouth (multiple small arcs)
            for (int j = -4; j <= 4; j++) {
                int x = mouthCenterX + j * 2;
                int y = mouthY + abs(j) / 2;  // Creates downward curve (frown)
                DrawPixel(x, y, WHITE);
            }
        }
        else {
            // Normal eyes
            DrawCircle((int)(game.ghosts[i].entity.x + TILE_SIZE / 3),
                (int)(game.ghosts[i].entity.y + TILE_SIZE / 3), 2, BLUE);
            DrawCircle((int)(game.ghosts[i].entity.x + 2 * TILE_SIZE / 3),
                (int)(game.ghosts[i].entity.y + TILE_SIZE / 3), 2, BLUE);
        }
    }

    if (game.state != STATE_DEATH || ((int)(game.stateTimer * 10) % 2 == 0)) {
        float angle = 0;
        if (game.pacman.direction == DIR_RIGHT) angle = 0;
        else if (game.pacman.direction == DIR_DOWN) angle = 90;
        else if (game.pacman.direction == DIR_LEFT) angle = 180;
        else if (game.pacman.direction == DIR_UP) angle = 270;

        int animFrame = (int)(GetTime() * 10) % 4;
        float mouthAngle = (animFrame % 2 == 0) ? 30.0f : 0.0f;
        DrawCircleSector((Vector2) { game.pacman.x + TILE_SIZE / 2.0f, game.pacman.y + TILE_SIZE / 2.0f },
            (float)(TILE_SIZE / 2), angle + mouthAngle, angle + 360 - mouthAngle, 20, YELLOW);
    }

    if (game.ghostScoreTimer > 0) {
        char scoreText[10];
        sprintf_s(scoreText, sizeof(scoreText), "%d", game.ghostScoreValue);
        DrawText(scoreText, (int)game.ghostScoreX, (int)game.ghostScoreY, 20, YELLOW);
    }

    char scoreText[50];
    sprintf_s(scoreText, sizeof(scoreText), "SCORE: %d", game.score);
    DrawText(scoreText, 10, 10, 20, WHITE);

    sprintf_s(scoreText, sizeof(scoreText), "HIGH SCORE: %d", game.highScore);
    DrawText(scoreText, SCREEN_WIDTH - 250, 10, 20, WHITE);

    for (int i = 0; i < game.lives; i++) {
        DrawCircle(10 + i * 30, SCREEN_HEIGHT - 20, 10, YELLOW);
    }

    sprintf_s(scoreText, sizeof(scoreText), "LEVEL: %d", game.level);
    DrawText(scoreText, SCREEN_WIDTH / 2 - 50, 10, 20, WHITE);

    if (game.state == STATE_INTRO) {
        DrawText("PAC-MAN", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 40, YELLOW);
        DrawText("PRESS SPACE TO START", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
    }
    else if (game.state == STATE_READY) {
        DrawText("READY!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 30, YELLOW);
    }
    else if (game.state == STATE_GAME_OVER) {
        DrawText("GAME OVER", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 30, RED);
        DrawText("PRESS SPACE TO RESTART", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 50, 20, WHITE);
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PAC-MAN");
    InitAudioDevice();
    SetTargetFPS(60);

    spritesheet = LoadTexture("spritesheet.png");
    soundChomp = LoadSound("chomp.wav");
    soundDeath = LoadSound("death.wav");
    soundEatFruit = LoadSound("eat_fruit.wav");
    soundEatGhost = LoadSound("eat_ghost.wav");
    soundGhostRunning = LoadSound("ghost_running_away.wav");
    soundIntro = LoadSound("Intro.wav");
    soundPowerSiren = LoadSound("power_siren.wav");
    soundGhostTurnBlue = LoadSound("ghost_turn_blue.wav");
    soundHighScore = LoadSound("high_score.wav");

    SetMasterVolume(1.0f);

    InitGame();

    while (!WindowShouldClose()) {
        UpdateGame();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawGame();
        EndDrawing();
    }

    UnloadTexture(spritesheet);
    UnloadSound(soundChomp);
    UnloadSound(soundDeath);
    UnloadSound(soundEatFruit);
    UnloadSound(soundEatGhost);
    UnloadSound(soundGhostRunning);
    UnloadSound(soundIntro);
    UnloadSound(soundPowerSiren);
    UnloadSound(soundGhostTurnBlue);
    UnloadSound(soundHighScore);

    CloseAudioDevice();
    CloseWindow();
	//this is a comment to test git changes
    return 0;
}

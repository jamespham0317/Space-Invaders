#pragma once
#include "spaceship.h"
#include "obstacle.h"
#include "alien.h"
#include "mysteryship.h"

class Game{
    public:
    Game();
        ~Game();
        void Draw();
        void Update();
        void HandleInput();
        bool run;
        int lives;
        int score;
        int highscore;
        int level;
    private:
    Spaceship spaceship;
        std::vector<Obstacle> obstacles;
        std::vector<Alien> aliens;
        MysteryShip mysteryship;
        int aliensDirection;
        std::vector<Laser> alienLasers;

        void DeleteInactiveLasers();
        std::vector<Obstacle> CreateObstacles();
        std::vector<Alien> CreateAliens();
        void MoveAliens();
        void MoveDownAliens(int distance);
        void AlienShootLaser();
        void CheckForCollisions();
        void GameOver();
        void Reset();
        void InitGame();
        void checkForHighScore();
        void saveHighscoreToFile(int highscore);
        int loadHighscoreFromFile();
        constexpr static float alienLaserShootInterval = 0.3;
        float timeLastAlienFired;
        float mysteryShipSpawnInterval;
        float timeLastSpawn;
        bool levelCleared;
};
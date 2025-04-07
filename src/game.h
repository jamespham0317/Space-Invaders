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
    private:
        void DeleteInactiveLasers();
        std::vector<Obstacle> CreateObstacles();
        std::vector<Alien> CreateAliens();
        void MoveAliens();
        void MoveDownAliens(int distance);
        void AlienShootLaser();
        void CheckForCollisions();
        Spaceship spaceship;
        std::vector<Obstacle> obstacles;
        std::vector<Alien> aliens;
        int aliensDirection;
        std::vector<Laser> alienLasers;
        constexpr static float alienLaserShootInterval = 0.3;
        float timeLastAlienFired;
        MysteryShip mysteryship;
        float mysteryShipSpawnInterval;
        float timeLastSpawn;
};
#include "game.h"
#include <iostream>
#include <fstream>


Game::Game()
{
    InitGame();
}

Game::~Game()
{
    Alien::UnloadImages();
}

void Game::Update()
{
    if (run) {
        double currentTime = GetTime();

        if (currentTime - timeLastSpawn > mysteryShipSpawnInterval && !gameWon) {
            mysteryship.Spawn();
            timeLastSpawn = GetTime();
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }

        for (auto& laser: spaceship.lasers) {
            laser.Update();
        }

        MoveAliens();
        AlienShootLaser();

        for (auto& laser: alienLasers) {
            laser.Update();
        }

        if (levelCleared) {
            if (level == 10) {
                gameWon = true;
                alienLasers.clear();
            } else {
                level++;
                aliens = CreateAliens();
                aliensDirection = 1;
                timeLastAlienFired = 0.0;
                spaceship.lasers.clear();
                alienLasers.clear();
                levelCleared = false;
            }
        }
        
        DeleteInactiveLasers();
        mysteryship.Update();
        CheckForCollisions();
    } 
    if (!run || gameWon) {
        if (IsKeyDown(KEY_ENTER)) {
            Reset();
            InitGame();
        }
    }
}

void Game::Draw()
{
    spaceship.Draw();
    mysteryship.Draw();

    for (auto& laser: spaceship.lasers) {
        laser.Draw();
    }

    for (auto& obstacle: obstacles) {
        obstacle.Draw();
    }

    for (auto& alien: aliens) {
        alien.Draw();
    }

    for (auto& laser: alienLasers) {
        laser.Draw();
    }
}

void Game::HandleInput()
{
    if (run) {
        if (IsKeyDown(KEY_LEFT)) {
            spaceship.MoveLeft();
        } else if (IsKeyDown(KEY_RIGHT)) {
            spaceship.MoveRight();
        } else if (IsKeyDown(KEY_SPACE)) {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers()
{
    for (auto it = spaceship.lasers.begin(); it != spaceship.lasers.end(); ) {
        if (!it->active) {
            it = spaceship.lasers.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = alienLasers.begin(); it != alienLasers.end(); ) {
        if (!it->active) {
            it = alienLasers.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

    for (int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }

    return obstacles;
}

std::vector<Alien> Game::CreateAliens() {
    std::vector<Alien> aliens;

    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 11; column++) {
            int alienType = 1;
            float x = 75 + column * 55;
            float y = 110 + row * 55;

            switch (level) {
                case 1: 
                    alienType = (row == 0) ? 3 : (row < 3) ? 2 : 1;
                    break;

                case 2: 
                    alienType = (column == 0 || column == 10) ? 3 : (column < 3 || column > 7) ? 2 : 1;
                    break;

                case 3: 
                    if ((row + column) % 2 == 0) alienType = 3;
                    else alienType = 1;
                    break;

                case 4:
                    if (column >= row && column < 11 - row) alienType = 2 + (row == 0);
                    else continue;
                    break;

                case 5: 
                    if (row == 0 || row == 4 || column == 0 || column == 10) alienType = 2;
                    else continue;
                    break;

                case 6: 
                    if (column >= 4 && column <= 6) alienType = row + 1;
                    else continue;
                    break;

                case 7: 
                    if (row == 1 || row == 3) alienType = (row == 1) ? 3 : 2;
                    else continue;
                    break;

                case 8:
                    if (column == row || column == 10 - row) alienType = 3;
                    else continue;
                    break;

                case 9: 
                    if ((row + column) % 3 == 0) continue;
                    alienType = (row + column) % 3 + 1;
                    break;

                case 10: 
                    alienType = 1 + rand() % 3;
                    break;

                default:
                    alienType = 1;
                    break;
            }

            aliens.push_back(Alien(alienType, {x, y}));
        }
    }

    return aliens;
}

void Game::MoveAliens() {
    for (auto& alien: aliens) {
        if (alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if (alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }
        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for (auto& alien: aliens) {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();

    if (currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2, 
                                        alien.position.y + alien.alienImages[alien.type - 1].height / 2}, 6));

        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions()
{
    // Spaceship Lasers

    for (auto& laser: spaceship.lasers) {
        auto it = aliens.begin();
        while (it != aliens.end()) {
            if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                if (it->type == 1) {
                    score += 100;
                } else if (it->type == 2) {
                    score += 200;
                } else if (it->type == 3) {
                    score += 300;
                }
                checkForHighScore();
                it = aliens.erase(it);
                laser.active = false;
                if (aliens.empty()) {
                    levelCleared = true;
                }
            } else {
                ++it;
            }
        }

        for (auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        } 

        if (CheckCollisionRecs(mysteryship.getRect(), laser.getRect())) {
            mysteryship.alive = false;
            laser.active = false;
            score += 500;
            checkForHighScore();
        }
    }

    // Alien Lasers

    for (auto& laser: alienLasers) {
        if (CheckCollisionRecs(laser.getRect(), spaceship.getRect())) {
            laser.active = false;
            lives--;
            if (lives == 0) {
                GameOver();
            }
        } 

        for (auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        } 
    }

    // Alien Collision with Obstacle

    for (auto& alien: aliens) {
        for (auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), alien.getRect())) {
                    it = obstacle.blocks.erase(it);
                } else {
                    it++;
                }
            }
        }

        if (CheckCollisionRecs(alien.getRect(), spaceship.getRect())) {
            GameOver();
        }
    }
}

void Game::GameOver()
{
    run = false;
}

void Game::Reset()
{
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::InitGame()
{
    obstacles = CreateObstacles();
    level = 1;
    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    lives = 3;
    score = 0;
    highscore = loadHighscoreFromFile();
    run = true;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    levelCleared = false;
    gameWon = false;
}

void Game::checkForHighScore()
{
    if (score > highscore) {
        highscore = score;
        saveHighscoreToFile(highscore);
    }
}

void Game::saveHighscoreToFile(int highscore)
{
    std::ofstream highscoreFile("highscore.txt");
    if (highscoreFile.is_open()) {
        highscoreFile << highscore;
        highscoreFile.close();
    } else {
        std::cerr <<"Failed to save";
    }
}

int Game::loadHighscoreFromFile()
{
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if (highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    } else {
        std::cerr <<"Failed to load highscore from file" << std::endl;
    }
    return loadedHighscore;
}

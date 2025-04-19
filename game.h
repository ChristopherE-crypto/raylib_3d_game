#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <vector>

struct Obstacle {
    Vector3 position;
    Vector3 size;
    Color color;
    Vector3 minBounds;
    Vector3 maxBounds;
    Model model;
};

struct Player {
    Vector3 position;
    Vector3 size; // x: width, y: height, z: length
    Color color;
    Vector3 minBounds;
    Vector3 maxBounds;
    
    void UpdateBounds() {
        minBounds = {
            position.x - size.x/2,
            position.y - size.y/2,
            position.z - size.z/2
        };
        maxBounds = {
            position.x + size.x/2,
            position.y + size.y/2,
            position.z + size.z/2
        };
    }
};

struct GameState {
    bool running = true;
    int score = 0;
    float scoreTimer = 0.0f;
    float gameOverTimer = 0.0f;
    float currentSpeed = 5.0f;
    
    void Reset() {
        running = true;
        score = 0;
        currentSpeed = 5.0f;
    }
};

struct Game {
  std::vector<Obstacle> obstaclePool;
  int activeObstacles = 0;
  const int MAX_OBSTACLES = 100;
  std::vector<Model> obstacleModels;
};

#endif

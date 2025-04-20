#include <raylib.h>
#include <raymath.h>
#include <cstdio>
#include "game.h"

/*
 * CAR 3D MODEL FOR PLAYER COMES FROM: Red Car by J-Toastie [CC-BY] (https://creativecommons.org/licenses/by/3.0/) via Poly Pizza (https://poly.pizza/m/dVLJ5CjB0h)
 * CAR 3D MODELS COME FROM:
 * Car by Poly by Google [CC-BY] (https://creativecommons.org/licenses/by/3.0/) via Poly Pizza (https://poly.pizza/m/75h3mi6uHuC),
 * Sports Car by Quaternius (https://poly.pizza/m/OyqKvX9xNh),
 * Mazda RX-7 by IvOfficial [CC-BY] (https://creativecommons.org/licenses/by/3.0/) via Poly Pizza (https://poly.pizza/m/SnIoWlh7S2),
 * Sports Car by Quaternius (https://poly.pizza/m/1mkmFkAz5v),
 * Car by Quaternius (https://poly.pizza/m/unqqkULtRU),
 * Police Car by Quaternius (https://poly.pizza/m/BwwnUrWGmV)
 *
 * BACKGROUND MUSIC FROM SUNO: https://suno.com/song/e1c1af2a-ee3e-4c9e-9ee2-4a41f3136048 (Endless Ride)
 */

void drawGround(float groundWidth, float groundHeight, float groundLength, Color groundColor, int numSegments, float groundStartZ)
{
  // p, w, h, l, c
  for(int i = 0; i < numSegments; i++)
  {
    float segmentZ = groundStartZ + i * groundLength;
    Vector3 groundPos = {0.0f, -groundHeight/2, segmentZ + groundLength/2};
    DrawCube(groundPos, groundWidth, groundHeight, groundLength, groundColor);
    DrawCubeWires(groundPos, groundWidth, groundHeight, groundLength, groundColor);
  }

  // street line variables
  float lineHeight = groundHeight * 1.1f;
  float lineWidth = 0.5f;
  float lineLength = 2.0f;
  float lineSpacing = 5.0f;
  Color lineColor = WHITE;

  // calculate total length to cover
  float totalLength = numSegments * groundLength;

  // draw dashed lines in the center
  for(float z = groundStartZ; z < groundStartZ + totalLength; z += lineSpacing)
  {
    Vector3 linePos = {0.0f, lineHeight, z + lineLength/2};
    DrawCube(linePos, lineWidth, groundHeight * 0.1f, lineLength, lineColor);
  }

  // draw line markers (side lines)
  float laneWidth = groundWidth * 0.45f;
  for(float z = groundStartZ; z < groundStartZ + totalLength; z += 0.1f)
  {
    // left lane marker
    Vector3 leftLinePos = {-laneWidth, lineHeight, z};
    DrawCube(leftLinePos, 0.2f, groundHeight * 0.1f, 0.1f, lineColor);

    // right lane marker
    Vector3 rightLinePos = {laneWidth, lineHeight, z};
    DrawCube(rightLinePos, 0.2f, groundHeight * 0.1f, 0.1f, lineColor);
  }

}

// function used for detecting collision (Bounding boxes)
void updatePlayerBounds(Vector3 playerPos, Vector3 playerSize, Vector3& playerMin, Vector3& playerMax)
{
  // playerMin and playerMax are references so that this function can update the player struct fields
  playerMin = (Vector3)
  {
    playerPos.x - playerSize.x/2,
    playerPos.y - playerSize.y/2,
    playerPos.z - playerSize.z/2
  };
  playerMax = (Vector3)
  {
    playerPos.x + playerSize.x/2,
    playerPos.y + playerSize.y/2,
    playerPos.z + playerSize.z/2
  };

}

void updateObstacleBounds(Vector3 obstaclePos, Vector3 obstacleSize, Vector3& obstacleMin, Vector3& obstacleMax)
{
  obstacleMin = (Vector3)
  {
    obstaclePos.x - obstacleSize.x/2,
    obstaclePos.y - obstacleSize.y/2,
    obstaclePos.z - obstacleSize.z/2
  };
  obstacleMax = (Vector3)
  {
    obstaclePos.x + obstacleSize.x/2,
    obstaclePos.y + obstacleSize.y/2,
    obstaclePos.z + obstacleSize.z/2
  };
}

bool checkCollisionPlayerObstacle(Vector3 playerMin, Vector3 playerMax, Vector3 obstaclePos, Vector3 obstacleSize, Vector3 obstacleMin, Vector3 obstacleMax)
{

  BoundingBox playerBox = {playerMin, playerMax};

  updateObstacleBounds(obstaclePos, obstacleSize, obstacleMin, obstacleMax);
  
  BoundingBox obstacleBox = {obstacleMin, obstacleMax};
  
  return CheckCollisionBoxes(playerBox, obstacleBox);

}

void handleCollisions(Vector3& playerPos, Vector3 playerSize, float& jumpVelocity, std::vector<Obstacle>& obstacles, float groundLevel, bool& isGrounded)
{

  Vector3 playerMin, playerMax;
  updatePlayerBounds(playerPos, playerSize, playerMin, playerMax);

  isGrounded = false;

  for(auto& obstacle : obstacles)
  {
    if(checkCollisionPlayerObstacle(playerMin, playerMax, obstacle.position, obstacle.size, obstacle.minBounds, obstacle.maxBounds) )
    {
      float penetrationY = playerMax.y - (obstacle.position.y - obstacle.size.y/2);

      if(jumpVelocity <= 0 && penetrationY > 0)
      {
        playerPos.y = obstacle.position.y + obstacle.size.y/2 + playerSize.y/2;
        isGrounded = true;
      }
      else if(jumpVelocity > 0)
      {
        playerPos.y = obstacle.position.y - obstacle.size.y/2 - playerSize.y/2;
      }

      jumpVelocity = 0;
      break;
    }
  }

  if(!isGrounded && playerPos.y < groundLevel)
  {
    playerPos.y = groundLevel;
    isGrounded = true;
    jumpVelocity = 0;
  }
}

void loadCarModel(Model &carModel)
{
  carModel = LoadModel("./assets/Red Car.glb");
}

void unloadCarModel(Model &carModel)
{
  UnloadModel(carModel);
}

Model loadObstacleModel(const char* modelPath)
{
  Model carModel = LoadModel(modelPath);
  return carModel;
}

void loadAllObstacleModels(Game& game)
{
  game.obstacleModels.push_back(loadObstacleModel("./assets/car_1.glb"));
  game.obstacleModels.push_back(loadObstacleModel("./assets/car_2.glb"));
  game.obstacleModels.push_back(loadObstacleModel("./assets/car_4.glb"));
}

void unloadAllObstacleModels(Game& game)
{
  for(auto& model : game.obstacleModels)
  {
    unloadCarModel(model);
  }
  game.obstacleModels.clear();
}

void drawSpeedGauge(float currentSpeed, float maxSpeed, int screenWidth, int screenHeight)
{
  Vector2 center = {(float) screenWidth - 100.0f, (float) screenHeight - 100.0f};
  float radius = 50.0f;

  // draw gauge background
  DrawCircleV(center, radius, ColorAlpha(BLACK, 0.5f));
  DrawCircleLinesV(center, radius, WHITE);

  //calculate needle angle
  float angle = 225 + (270 * (currentSpeed / maxSpeed));

  // draw needle
  Vector2 needleEnd = {
    center.x + radius * cosf(angle * DEG2RAD),
    center.y + radius * sinf(angle * DEG2RAD)
  };

  DrawLineEx(center, needleEnd, 3.0f, RED);

  // draw speed text
  char speedText[32];
  snprintf(speedText, sizeof(speedText), "%.1f", currentSpeed);
  DrawText(speedText, center.x - 15, center.y - 10, 20, WHITE);
}

void loadGameAudio(Game& game)
{
  game.backgroundMusic = LoadMusicStream("./assets/background_song.mp3");

  // start playing background music
  PlayMusicStream(game.backgroundMusic);
  SetMusicVolume(game.backgroundMusic, 0.5f);
}

void unloadGameAudio(Game& game)
{
  if(IsAudioDeviceReady())
  {
    StopMusicStream(game.backgroundMusic);
    UnloadMusicStream(game.backgroundMusic);
  }
}

int main()
{
  // window creation
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Learning 3D");
  // for audio
  InitAudioDevice();

  SetTargetFPS(60);

  Game game;
  game.obstaclePool.resize(game.MAX_OBSTACLES);

  loadGameAudio(game);

  loadAllObstacleModels(game);

  // set up 3D camera
  Camera3D camera = {0};
  camera.position = (Vector3) {0.0f, 10.0f, 10.0f};
  camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // player car model data
  Model carModel;
  Texture2D carTexture;
  Vector3 carModelOffset = {0.0f, -0.5f, 0.0f};
  float carModelScale = 2.0f;

  // player data
  Vector3 playerPos = {0.0f, 0.5f, 0.0f};
  float playerWidth = 1.0f;
  float playerHeight = 1.0f;
  float playerLength = 1.0f;
  Color playerColor = GREEN;

  // use player struct from game.h
  Player player = {
    playerPos,
    (Vector3) {playerWidth, playerHeight, playerLength},
    playerColor,
    {0},
    {0}
  };

  // load car model
  loadCarModel(carModel);

  // obstacle system
  std::vector<Obstacle> obstacles;
  float obstacleSpawnTimer = 0.0f;
  float obstacleSpawnInterval = 2.0f;
  float obstacleZOffset = 80.0f;

  // movement variables
  float forwardSpeed = 5.0f;
  float currentForwardSpeed = forwardSpeed;
  float maxSpeed = 20.0f;
  float speedIncreaseRate = 0.5f;
  float lateralSpeed = 7.0f;
  float boundary = 8.0f;
  float obstacleSpeed = 3.0f;
  float currentObstacleSpeed = 0.0f;
  float lateralSpeedPenalty = 0.05f;
  float minForwardSpeed = 3.0f;
  float decelerationRate = 2.5f;

  // jumping variables
  bool isGrounded = false;
  float jumpVelocity = 0.0f;
  const float jumpForce = 7.25f;
  const float gravity = 20.0f;

  // ground data
  float groundWidth = 20.0f;
  float groundHeight = 0.1f;
  float groundLength = 100.0f;
  Color groundColor = DARKGRAY;
  int numGroundSegments = 5;
  float groundStartZ = -groundLength * (numGroundSegments - 1);
  const float groundLevel = 1.0f;

  // game variables
  bool gameRunning = true;
  int score = 0;
  float scoreTimer = 0.0f;
  float gameOverTimer = 0.0f;
  Vector3 worldOffset = {0};
  const float rebaseThreshold = 100.0f;

  // game loop
  while(!WindowShouldClose())
  {

    float deltaTime = GetFrameTime();

    currentObstacleSpeed = obstacleSpeed * (currentForwardSpeed / forwardSpeed);

    if(gameRunning)
    {
  
      // make the player move forward continuously
      player.position.z -= currentForwardSpeed * deltaTime;
      currentForwardSpeed += speedIncreaseRate * deltaTime;

      // Lateral movement with speed reduction
      if(IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        float targetSpeed = minForwardSpeed;
        currentForwardSpeed = fmaxf(targetSpeed, currentForwardSpeed - decelerationRate * deltaTime);
    
        // Actual movement
        if(IsKeyDown(KEY_A)) player.position.x -= lateralSpeed * deltaTime;
        if(IsKeyDown(KEY_D)) player.position.x += lateralSpeed * deltaTime;
        player.position.x = Clamp(player.position.x, -boundary, boundary);
      }
      else {
        // Speed recovery when not moving laterally
        currentForwardSpeed = fminf(maxSpeed, currentForwardSpeed + speedIncreaseRate * deltaTime);
      }      

      // handle player jumping
      if(IsKeyDown(KEY_SPACE) && isGrounded)
      {
        jumpVelocity = jumpForce;
        isGrounded = false;
      }

      // apply gravity
      jumpVelocity -= gravity * deltaTime;
      player.position.y += jumpVelocity * deltaTime;

      // deal with obstacles shaking after some time
      if(fabsf(player.position.z) > rebaseThreshold)
      {
        worldOffset.z += player.position.z;

        player.position.z = 0;

        for(int i = 0; i < game.activeObstacles; i++)
        {
          game.obstaclePool[i].position.z -= worldOffset.z;
        }

        groundStartZ -= worldOffset.z;
        worldOffset.z = 0;
      }


      // extend the ground for infinite ground effect
      if(player.position.z < groundStartZ + groundLength)
      {
        groundStartZ -= groundLength;
      }

      // collisions
      handleCollisions(player.position, player.size, jumpVelocity, obstacles, groundLevel, isGrounded);

      for(int i = 0; i < game.activeObstacles; i++)
      {
        game.obstaclePool[i].position.z += currentObstacleSpeed * deltaTime;
      }

      // spawn obstacles
      obstacleSpawnTimer += deltaTime;

      if(obstacleSpawnTimer >= obstacleSpawnInterval && game.activeObstacles < game.MAX_OBSTACLES)
      {

        // get obstacle from the pool
        Obstacle& newObstacle = game.obstaclePool[game.activeObstacles];

        if(!game.obstacleModels.empty())
        {
          newObstacle = {
            (Vector3) {
              (float) GetRandomValue(-5, 5),
              0.5f,
              player.position.z - obstacleZOffset
            },
            (Vector3) {2.0f, 1.5f, 4.0f},
            ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f),
            {0}, {0},
            game.obstacleModels[GetRandomValue(0, game.obstacleModels.size() - 1)],
          };

          game.activeObstacles++;
          obstacleSpawnTimer = 0.0f;
          obstacleSpawnInterval = GetRandomValue(1, 3) / (currentForwardSpeed / forwardSpeed);

        }
      }

      // handles obstacle removal
      for(int i = 0; i < game.activeObstacles;)
      {
        if(game.obstaclePool[i].position.z > player.position.z + 30.0f)
        {
          if(i != game.activeObstacles - 1)
          {
            std::swap(game.obstaclePool[i], game.obstaclePool[game.activeObstacles - 1]);
          }
          game.activeObstacles--;
        }
        else {
          i++;
        }
      }

      // update score
      scoreTimer += deltaTime;
      if(scoreTimer >= 1.0f)
      {
        score += (int) currentForwardSpeed;
        scoreTimer = 0.0f;
      }

      updatePlayerBounds(player.position, player.size, player.minBounds, player.maxBounds);

      // check for game over
      for(int i = 0; i < game.activeObstacles; i++)
      {
        auto& currentObstacle = game.obstaclePool[i];
        if(checkCollisionPlayerObstacle(player.minBounds, player.maxBounds, currentObstacle.position, currentObstacle.size, currentObstacle.minBounds, currentObstacle.maxBounds))
        {
          gameRunning = false;
          gameOverTimer = 0.0f;
          break;
        }
      }

    }
    else {
      StopMusicStream(game.backgroundMusic);
      // game over state
      if(IsKeyPressed(KEY_R))
      {
        // reset game
        player.position = {0.0f, 0.1f, 0.0f};
        game.activeObstacles = 0;
        currentForwardSpeed = forwardSpeed;
        score = 0;
        gameRunning = true;
        PlayMusicStream(game.backgroundMusic);
      }

    }



    // update camera position
    camera.target = (Vector3) {player.position.x, player.position.y + 1.0f, player.position.z};
    camera.position = (Vector3) {Lerp(camera.position.x, player.position.x, 0.1f), 5.0f, player.position.z + 10.0f};

    UpdateMusicStream(game.backgroundMusic);

    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    // draw the ground using a cube
    drawGround(groundWidth, groundHeight, groundLength, groundColor, numGroundSegments, groundStartZ);

    // draw the red car for player
    Vector3 modelPos = Vector3Add(player.position, carModelOffset);
    DrawModelEx(carModel, modelPos, (Vector3) {0.0f, 1.0f, 0.0f},
        0.0f,
        (Vector3) {carModelScale, carModelScale, carModelScale},
        WHITE);

    // draw obstacles
    for(int i = 0; i < game.activeObstacles; i++)
    {
      const Obstacle& obstacle = game.obstaclePool[i];
      
      Vector3 modelPos = Vector3Add(obstacle.position, (Vector3) {0.0f, -0.5f, 0.0f});
      DrawModelEx(obstacle.model, modelPos, (Vector3) {0.0f, 1.0f, 0.0f}, 180.0f, (Vector3) {1.5f, 1.5f, 1.5f}, WHITE);
      
      if(checkCollisionPlayerObstacle(player.minBounds, player.maxBounds, obstacle.position, obstacle.size, obstacle.minBounds, obstacle.maxBounds))
      {
        DrawBoundingBox((BoundingBox) {obstacle.minBounds, obstacle.maxBounds}, RED);
      }
    }

    EndMode3D();

    // draw UI
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
    DrawFPS(10, 40);

    // game over screen
    if(!gameRunning)
    {
      float pulse = sin(gameOverTimer * 5.0f) * 0.5f + 0.5f;
      DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.5f));

      DrawText("GAME OVER",
          screenWidth/2 - MeasureText("GAME OVER", 40)/2,
          screenHeight/2 - 40, 40, ColorAlpha(RED, pulse));

      DrawText("Press R to restart",
          screenWidth/2 - MeasureText("Press R to restart", 20)/2,
          screenHeight/2 + 10, 20, WHITE);

      DrawText(TextFormat("Final Score: %d", score),
          screenWidth/2 - MeasureText(TextFormat("Final Score: %d", score), 30)/2,
          screenHeight/2 + 50, 30, YELLOW);

    }
    
    drawSpeedGauge(currentForwardSpeed, maxSpeed, screenWidth, screenHeight);
    EndDrawing();

  }
  unloadAllObstacleModels(game);
  unloadCarModel(carModel);
  unloadGameAudio(game);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}

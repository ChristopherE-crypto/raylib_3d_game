#include <raylib.h>
#include <raymath.h>
#include "game.h"

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

int main()
{
  // window creation
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Learning 3D");
  SetTargetFPS(60);

  Game game;
  game.obstaclePool.resize(game.MAX_OBSTACLES);

  // set up 3D camera
  Camera3D camera = {0};
  camera.position = (Vector3) {0.0f, 10.0f, 10.0f};
  camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

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

  // obstacle system
  std::vector<Obstacle> obstacles;
  float obstacleSpawnTimer = 0.0f;
  float obstacleSpawnInterval = 2.0f;
  float obstacleZOffset = 30.0f;

  // movement variables
  float forwardSpeed = 5.0f;
  float currentForwardSpeed = forwardSpeed;
  float speedIncreaseRate = 0.1f;
  float lateralSpeed = 7.0f;
  float boundary = 8.0f;

  // jumping variables
  bool isGrounded = false;
  float jumpVelocity = 0.0f;
  const float jumpForce = 7.25f;
  const float gravity = 20.0f;

  // ground data
  float groundWidth = 20.0f;
  float groundHeight = 0.1f;
  float groundLength = 100.0f;
  Color groundColor = GRAY;
  int numGroundSegments = 3;
  float groundStartZ = -groundLength * 2;
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

    if(gameRunning)
    {
  
      // make the player move forward continuously
      player.position.z -= currentForwardSpeed * deltaTime;
      currentForwardSpeed += speedIncreaseRate * deltaTime;

      // handle player lateral movement
      if(IsKeyDown(KEY_A)) player.position.x -= lateralSpeed * deltaTime;
      if(IsKeyDown(KEY_D)) player.position.x += lateralSpeed * deltaTime;
      player.position.x = Clamp(player.position.x, -boundary, boundary);

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
      if(player.position.z < groundStartZ + groundLength/2)
      {
        groundStartZ -= groundLength;
      }

      // collisions
      handleCollisions(player.position, player.size, jumpVelocity, obstacles, groundLevel, isGrounded);

      // spawn obstacles
      obstacleSpawnTimer += deltaTime;

      if(obstacleSpawnTimer >= obstacleSpawnInterval && game.activeObstacles < game.MAX_OBSTACLES)
      {

        // get obstacle from the pool
        Obstacle& newObstacle = game.obstaclePool[game.activeObstacles];

        // configure obstacle
        newObstacle = {
          (Vector3)
          {
            (float) GetRandomValue(-5, 5),
            0.5f,
            player.position.z - obstacleZOffset
          },
          (Vector3)
          {
            (float) GetRandomValue(1, 3),
            (float) GetRandomValue(1, 3),
            (float) GetRandomValue(1, 3)
          },
          ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f)
        };

        game.activeObstacles++;
        obstacleSpawnTimer = 0.0f;
        obstacleSpawnInterval = GetRandomValue(1, 3) / (currentForwardSpeed / forwardSpeed);
      }

      // handles obstacle removal
      for(int i = 0; i < game.activeObstacles;)
      {
        if(game.obstaclePool[i].position.z > player.position.z + 10.0f)
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
      // game over state
      if(IsKeyPressed(KEY_R))
      {
        // reset game
        player.position = {0.0f, 0.1f, 0.0f};
        game.activeObstacles = 0;
        currentForwardSpeed = forwardSpeed;
        score = 0;
        gameRunning = true;
      }

    }



    // update camera position
    camera.target = (Vector3) {player.position.x, player.position.y + 1.0f, player.position.z};
    camera.position = (Vector3) {Lerp(camera.position.x, player.position.x, 0.1f), 5.0f, player.position.z + 10.0f};

    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    // draw the ground using a cube
    drawGround(groundWidth, groundHeight, groundLength, groundColor, numGroundSegments, groundStartZ);

    // draw the player as a green cube
    DrawCube(player.position, player.size.x, player.size.y, player.size.z, player.color);
    DrawCubeWires(player.position, player.size.x, player.size.y, player.size.z, DARKGREEN);

    // draw obstacles
    for(int i = 0; i < game.activeObstacles; i++)
    {
      const Obstacle& obstacle = game.obstaclePool[i];
      Color drawColor = obstacle.color;
      
      if(checkCollisionPlayerObstacle(player.minBounds, player.maxBounds, obstacle.position, obstacle.size, obstacle.minBounds, obstacle.maxBounds))
      {
        drawColor = YELLOW;
      }

      DrawCube(obstacle.position, obstacle.size.x, obstacle.size.y, obstacle.size.z, drawColor);
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

    EndDrawing();

  }

  CloseWindow();
  return 0;
}

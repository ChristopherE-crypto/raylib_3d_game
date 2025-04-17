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

bool CheckCollisionPlayerObstacle(Vector3 playerMin, Vector3 playerMax, Vector3 obstacleMin, Vector3 obstacleMax)
{

  BoundingBox playerBox = {playerMin, playerMax};
  
  BoundingBox obstacleBox = {obstacleMin, obstacleMax};
  
  return CheckCollisionBoxes(playerBox, obstacleBox);

}

int main()
{
  // window creation
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Learning 3D");
  SetTargetFPS(60);

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

  // movement variables
  float currentForwardSpeed = 5.0f;
  float speedIncreaseRate = 0.1f;
  float lateralSpeed = 7.0f;
  float boundary = 8.0f;

  // ground data
  float groundWidth = 20.0f;
  float groundHeight = 0.1f;
  float groundLength = 100.0f;
  Color groundColor = GRAY;
  int numGroundSegments = 3;
  float groundStartZ = -groundLength * 2;

  // game loop
  while(!WindowShouldClose())
  {

    float deltaTime = GetFrameTime();
  
    // make the player move forward continuously
    playerPos.z -= currentForwardSpeed * deltaTime;
    currentForwardSpeed += speedIncreaseRate * deltaTime;

    // handle player lateral movement
    if(IsKeyDown(KEY_A)) player.position.x -= lateralSpeed * deltaTime;
    if(IsKeyDown(KEY_D)) player.position.x += lateralSpeed * deltaTime;
    player.position.x = Clamp(player.position.x, -boundary, boundary);


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

    EndMode3D();

    // draw UI
    DrawFPS(10, 40);

    EndDrawing();

  }

  CloseWindow();
  return 0;
}

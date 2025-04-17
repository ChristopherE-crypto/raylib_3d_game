#include <raylib.h>
#include <raymath.h>


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

  // movement variables
  float currentForwardSpeed = 5.0f;
  float speedIncreaseRate = 0.1f;

  // game loop
  while(!WindowShouldClose())
  {

    float deltaTime = GetFrameTime();
  
    // make the player move forward continuously
    playerPos.z -= currentForwardSpeed * deltaTime;
    currentForwardSpeed += speedIncreaseRate * deltaTime;

    // update camera position
    camera.target = (Vector3) {playerPos.x, playerPos.y + 1.0f, playerPos.z};
    camera.position = (Vector3) {Lerp(camera.position.x, playerPos.x, 0.1f), 5.0f, playerPos.z + 10.0f};

    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2) {20.0f, 20.0f}, GRAY);

    DrawCube(playerPos, playerWidth, playerHeight, playerLength, playerColor);
    DrawCubeWires(playerPos, playerWidth, playerHeight, playerLength, DARKGREEN);

    EndMode3D();

    // draw UI
    DrawFPS(10, 40);

    EndDrawing();

  }

  CloseWindow();
  return 0;
}

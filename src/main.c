/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

const Vector2 windowSize = { 1280, 720 };

// Player definition
Vector2 playerPos = { windowSize.x / 2, windowSize.y / 2};
const float playerRadius = 25.0f;
Color playerColor = GREEN;
float playerSpeed = 300.0f;

// Helper to draw text horizontally centered on screen
void CenterText(const char* text, int yPos, int fontSize, Color textColor)
{
    int textSize = MeasureText(text, fontSize);
    DrawText(text, (int)windowSize.x / 2 - textSize / 2, yPos, fontSize, textColor);
}

// Main function
int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(windowSize.x, windowSize.y, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// game loop
	// run the loop until the user presses ESCAPE or presses the Close button on the window
	while (!WindowShouldClose())
	{
	    // update
	    float dt = GetFrameTime();
		Vector2 playerDir = { 0.0f, 0.0f };
		if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerDir.x -= 1.0f;
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerDir.x += 1.0f;
		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) playerDir.y -= 1.0f;
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) playerDir.y += 1.0f;

		playerDir = Vector2Normalize(playerDir);

		playerPos.x += playerDir.x * playerSpeed * dt;
		playerPos.y += playerDir.y * playerSpeed * dt;
		playerPos.x = Clamp(playerPos.x, playerRadius, windowSize.x - playerRadius);
		playerPos.y = Clamp(playerPos.y, playerRadius, windowSize.y - playerRadius);

		// draw
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);


		int font_size = 20;
		CenterText("Hello Raylib, wassup?", 100, font_size, WHITE);
		const char* screenSizeText = TextFormat("Screen Size %ix%i", GetScreenWidth(), GetScreenHeight());
		CenterText(screenSizeText, 220, font_size, WHITE);

		// draw player
		DrawCircleV(playerPos, playerRadius, playerColor);

		// draw debug
		DrawText(TextFormat("Player Position: %i/%i", (int)playerPos.x, (int)playerPos.y), 20, 20, 20, RED);
		DrawText(TextFormat("Player Direction: %.2f/%.2f", playerDir.x, playerDir.y), 20, 40, 20, RED);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

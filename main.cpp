#include "raylib.h"
#include "raymath.h"
#include <iostream>  

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Compute Shader Wave");
    
    Camera3D camera = { 0 };
    camera.position = (Vector3{ 0.0f, 2.0f, 5.0f }); 
    camera.target = (Vector3{ 0.0f, 0.0f, 0.0f });    
    camera.up = (Vector3{ 0.0f, 1.0f, 0.0f });        
    camera.fovy = 45.0f;                              
    
    Shader shader = LoadShader("Vertex.shader", "Fragment.shader");
    
    if (shader.id == 0) {
        std::cout << "Error Load Shader !" << std::endl;
        return -1; 
    }
    
    int timeLoc = GetShaderLocation(shader, "time");
    
    Mesh plane = GenMeshPlane(5.0f, 5.0f, 100, 100);  
    Model water = LoadModelFromMesh(plane);
    water.materials[0].shader = shader;
    
    float time = 0.0f;
    
    SetTargetFPS(60); 
    while (!WindowShouldClose()) 
    {
        time += GetFrameTime();
        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
        
        Matrix model = MatrixIdentity();  
        Matrix view = MatrixLookAt(camera.position, camera.target, camera.up); 
        Matrix projection = MatrixPerspective(camera.fovy, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);  
        
        SetShaderValueMatrix(shader, GetShaderLocation(shader, "model"), model);
        SetShaderValueMatrix(shader, GetShaderLocation(shader, "view"), view);
        SetShaderValueMatrix(shader, GetShaderLocation(shader, "projection"), projection);
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        
        DrawModel(water, (Vector3{ 0.0f, 0.0f, 0.0f }), 1.0f, WHITE);

        EndMode3D();
        EndDrawing();
    }
    
    UnloadShader(shader);
    UnloadModel(water);
    
    CloseWindow();

    return 0;
}

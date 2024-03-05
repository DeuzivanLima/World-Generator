#include <iostream>
#include <raylib.h>
#include <vector>

#include "Config.hpp"

class Entity
{
public:
    Entity(const Texture2D &texture, Vector2 size, Vector2 position, Vector2 source = {0.f, 0.f}) noexcept
    {
        this->texture = texture;
        this->transform = {position.x, position.y, size.x, size.y};
        this->source_transform = {source.x, source.y, (float) (texture.width / (256 / 16)), (float) (texture.height / (256 / 16))};
        this->origin = {0, 0};

    }

    virtual void draw() noexcept
    {
        DrawTexturePro(texture, source_transform, transform, origin, 0, WHITE);
    }

    virtual ~Entity()
    {
    }

    Texture2D texture;
    Rectangle transform, source_transform;
    Vector2 origin;
};

int main(int argc, char *argv[])
{
    srand(time(NULL));
    // unused
    (void)argc;
    (void)argv;

    InitWindow(1280, 720, "World Generator");
    SetTargetFPS(60);
    Texture2D tileset_main_001 = LoadTexture("assets/tileset/main-001.png");
    std::vector<Entity> world;
    Vector2 camera = {0.f, 0.f};

    int y_max = rand() % 20 + 2;
    for(int x = 0; x < 128; x++)
    {
        for(int y = 0; y < y_max; y++)
        {
            Vector2 source = {0.f, 0.f};
            if(y == 0) source = {16.f, 0.f};
            if(y > 5) source = {16.f * 2.f, 0.f};
            world.push_back(Entity(tileset_main_001, {32.f, 32.f}, {32.f * x, 32.f * y + 612 - (32.f * y_max)}, source));
        }
        y_max += rand() % 2 == 0 ? 1 : -1;
        if(y_max < 2)
            y_max = 2;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        for(auto block : world)
        {
            block.transform.x += camera.x;
            block.draw();
        }

        EndDrawing();
        camera.x -= 1.f;
    }

    UnloadTexture(tileset_main_001);
    CloseWindow();

    return EXIT_SUCCESS;
}
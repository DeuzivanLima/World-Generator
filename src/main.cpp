#include <iostream>
#include <raylib.h>
#include <vector>
#include <ctime>

#include "Config.hpp"

class Transform2D
{
public:
    Transform2D(const Rectangle translation = {0.f, 0.f, DEFAULT_RECTANGLE_SIZE, DEFAULT_RECTANGLE_SIZE}, const float rotation = 0.f) noexcept
            : __translation{translation}, __rotation{rotation}
    {
    }

    virtual void setTranslation(const Rectangle translate) noexcept
    {
        this->__translation = translate;
    }
    const Rectangle &getTranslation() const noexcept
    {
        return this->__translation;
    }
    virtual void setRotation(const float rotation) noexcept
    {
        this->__rotation = rotation;
    }
    const float &getRotation() const noexcept
    {
        return this->__rotation;
    }

    virtual ~Transform2D()
    {
    }
private:
    Rectangle __translation;
    float __rotation;
};

class Sprite : public Transform2D
{
public:
    Sprite(Texture2D *texture, const Rectangle translation = {0.f, 0.f, DEFAULT_RECTANGLE_SIZE, DEFAULT_RECTANGLE_SIZE}, const Rectangle texture_translation = {0.f, 0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE}, const float rotation = 0.f) noexcept
            : Transform2D(translation, rotation), __texture_translation{texture_translation}, __texture{texture}, __origin {0.f, 0.f}
    {
        if(rotation > 0)
            this->__origin = {translation.width, translation.height};
    }

    virtual void draw(Camera2D camera = {.offset = {0.f, 0.f}}) const noexcept
    {
        Rectangle translation_relative_on_camera = this->getTranslation();

        translation_relative_on_camera.x += camera.offset.x;
        translation_relative_on_camera.y += camera.offset.y;

        DrawTexturePro(*this->__texture, this->__texture_translation, translation_relative_on_camera, this->__origin, this->getRotation(), WHITE);
    }

    virtual ~Sprite()
    {
        this->__texture = nullptr;
    }
private:
    Rectangle __texture_translation;
    Texture2D *__texture;
    Vector2 __origin;
};

class Chunk final
{
public:
    Chunk(Texture2D *tileset)
            : __tileset{tileset}
    {
        this->generate();
    }

    void draw(Camera2D camera) noexcept
    {
        for(Sprite tile : this->__tiles)
            tile.draw(camera);
    }
    void generate() noexcept
    {
        this->__tiles.clear();
        int max_height = std::rand() % CHUNK_HEIGHT + 10;
        for(size_t x = 0; x < CHUNK_WIDTH; x++)
        {
            for(size_t y = 0; y < static_cast<size_t>(max_height); y++)
            {
                Rectangle texture_rect = {0.f, 0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE};
                if(y == 0)
                    texture_rect.x = TILE_TEXTURE_SIZE;
                else if(y > 5)
                    texture_rect.x = TILE_TEXTURE_SIZE * 2.f;
                this->__tiles.push_back(Sprite(this->__tileset, {TILE_SIZE * x, TILE_SIZE * y + GetScreenHeight() - (max_height * TILE_SIZE), TILE_SIZE, TILE_SIZE}, texture_rect));
            }
            max_height = max_height < 2 ? 2 : max_height + (std::rand() % 2 == 0 ? -1 : 1);
        }
    }

    virtual ~Chunk()
    {
    }
private:
    std::vector<Sprite> __tiles;
    Texture2D *__tileset;
};

int main(int, char *[])
{
    std::srand(std::time(NULL));
    InitWindow(GetScreenWidth(), GetScreenHeight(), "World Generator");
    Texture2D tileset_main = LoadTexture("assets/tileset/world-001.png");
    Camera2D camera = {.offset = {0.f, 0.f}};

    Sprite
            button_menu(&tileset_main, {GetScreenWidth() - 250.f, 100.f, BUTTON_SIZE, BUTTON_SIZE}, {TILE_TEXTURE_SIZE * 2.f, TILE_TEXTURE_SIZE * 2.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE}),
            button_action(&tileset_main, {GetScreenWidth() - 250.f, GetScreenHeight() - 250.f, BUTTON_SIZE, BUTTON_SIZE}, {TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE * 2.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE}),
            button_left(&tileset_main, {100.f + BUTTON_SIZE + HORIZONTAL_BUTTON_GAP, GetScreenHeight() - 250.f, BUTTON_SIZE, BUTTON_SIZE}, {0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE}),
            button_jump(&tileset_main, {GetScreenWidth() - 100.f - BUTTON_SIZE - HORIZONTAL_BUTTON_GAP, GetScreenHeight() - 250.f, BUTTON_SIZE, BUTTON_SIZE}, {0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE}, 270.f),
            button_right(&tileset_main, {100.f, GetScreenHeight() - 250.f, BUTTON_SIZE, BUTTON_SIZE}, {0.f, TILE_TEXTURE_SIZE, -TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE});

    Chunk chunk(&tileset_main);

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        for(int i = 0; i < GetTouchPointCount(); i++)
        {
            if(CheckCollisionPointRec(GetTouchPosition(i), button_left.getTranslation()))
                camera.offset.x -= 10.f;
            if(CheckCollisionPointRec(GetTouchPosition(i), button_right.getTranslation()))
                camera.offset.x += 10.f;
            if(CheckCollisionPointRec(GetTouchPosition(i), button_menu.getTranslation()))
                chunk.generate();
        }


        BeginDrawing();
        ClearBackground(BLACK);

        chunk.draw(camera);

        button_left.draw();
        button_right.draw();
        button_menu.draw();
        button_action.draw();
        button_jump.draw();

        EndDrawing();
    }

    UnloadTexture(tileset_main);
    CloseWindow();
    return EXIT_SUCCESS;
}
#include <iostream>
#include <raylib.h>
#include <vector>
#include <ctime>

#include "Config.hpp"

class Transform2D
{
public:
    Transform2D(const Rectangle translation = {0.f, 0.f, DEFAULT_RECTANGLE_SIZE * SPRITE_SCALE, DEFAULT_RECTANGLE_SIZE * SPRITE_SCALE}, const float rotation = 0.f) noexcept
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
    Sprite(Texture2D *texture, const Rectangle translation = {0.f, 0.f, DEFAULT_RECTANGLE_SIZE, DEFAULT_RECTANGLE_SIZE}, const Rectangle texture_translation = {0.f, 0.f, DEFAULT_RECTANGLE_SIZE, DEFAULT_RECTANGLE_SIZE}, const float rotation = 0.f) noexcept
        : Transform2D(translation, rotation), __texture_translation{texture_translation}, __texture{texture}
    {
    }

    virtual void draw(Camera2D camera) const noexcept
    {
        Rectangle translation_relative_on_camera = this->getTranslation();

        translation_relative_on_camera.x += camera.offset.x;
        translation_relative_on_camera.y += camera.offset.y;

        DrawTexturePro(*this->__texture, this->__texture_translation, this->getTranslation(), {0.f, 0.f}, this->getRotation(), WHITE);
    } 

    virtual ~Sprite()
    {
    }
private:
    Rectangle __texture_translation;
    Texture2D *__texture;
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
        for(const Sprite &tile : this->__tiles)
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
            max_height += std::rand() % 2 == 0 ? -1 : 1;
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
    std::srand(time(NULL));
    InitWindow(GetScreenWidth(), GetScreenHeight(), "World Generator");
    Texture2D tileset_main = LoadTexture("assets/tileset/world-001.png");
    Camera2D camera;

    Chunk chunk(&tileset_main);
    
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            chunk.generate();

        BeginDrawing();
        ClearBackground(BLACK);

        chunk.draw(camera);

        EndDrawing();
    }

    UnloadTexture(tileset_main);
    CloseWindow();
    return EXIT_SUCCESS;
}
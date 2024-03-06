#include <iostream>
#include <raylib.h>
#include <vector>

#include "Config.hpp"

class Transform2D
{
public:
    Transform2D(const Vector2 &position = {0.f, 0.f}, const Vector2 &size = {16.f, 16.f}, const float rotation = 0.f) noexcept
    {
        this->setPosition(position);
        this->setRotation(rotation);
        this->setSize(size);
    }

    virtual void setPosition(const Vector2 &position) noexcept
    {
        this->__position = position;
    }
    const Vector2 &getPosition() const noexcept
    {
        return this->__position;
    }
    virtual void setSize(const Vector2 &size) noexcept
    {
        this->__size = size;
    }
    const Vector2 &getSize() const noexcept
    {
        return this->__size;
    }
    virtual void setRotation(const float rotation) noexcept
    {
        this->__rotation = rotation;
    }
    const float &getRotation() const noexcept
    {
        return this->__rotation;
    }

    virtual ~Transform2D() noexcept
    {
    }
private:
    Vector2 __position, __size;
    float __rotation;
};

class Sprite : public Transform2D
{
public:
    Sprite(const Texture2D &texture, const Vector2 &position = {0.f, 0.f}, const Vector2 &size = {16.f, 16.f}, const float rotation = 0.f)
        : Transform2D(position, size, rotation)
    {
        this->__texture = &texture;
        this->setOrigin({0.f, 0.f});
        this->setTextureRect({0.f, 0.f, this->getSize().x, this->getSize().y});
    }

    virtual void setOrigin(const Vector2 origin) noexcept
    {
        this->__origin = origin;
    }
    const Vector2 &getOrigin() const noexcept
    {
        return this->__origin;
    }
    virtual void setTextureRect(const Rectangle &texture_rect) noexcept
    {
        this->__texture_rect = texture_rect;
    }
    const Rectangle &getTextureRect() const noexcept
    {
        return this->__texture_rect;
    }

    virtual void draw() const noexcept
    {
        DrawTexturePro(*this->__texture,
            this->getTextureRect(),
            {this->getPosition().x, this->getPosition().y, this->getSize().x, this->getSize().y},
            this->__origin, this->getRotation(), WHITE);
    }

    virtual ~Sprite()
    {
    }
private:
    const Texture2D *__texture;
    Rectangle __texture_rect;
    Vector2 __origin;
};

class Tile : public Sprite
{
public:
    Tile(const Texture2D &texture, const Vector2 &position = {0.f, 0.f}, const Rectangle &texture_rect = {0.f, 0.f, 16.f, 16.f}, const Vector2 &size = {16.f, 16.f}, const float resistence = 1.f)
        : Sprite(texture, position, size, 0.f)
    {
        this->setResistence(resistence);
        this->setTextureRect(texture_rect);
    }

    virtual void setResistence(const float resistence) noexcept
    {
        this->__resistence = resistence;
    }
    const float &getResistence() const noexcept
    {
        return this->__resistence;
    }

    virtual ~Tile()
    {
    }
private:
    float __resistence;
};

Texture2D tileset_default_world;

int main(int, char *[])
{
    srand(time(NULL));
    InitWindow(1280, 720, "World Generator");
    SetTargetFPS(60);

    std::vector<Tile> world;
    tileset_default_world = LoadTexture("assets/tileset/world-001.png");

    int max_y = rand() % 25 + 4;
    for(int x = 0; x < 25; x++)
    {
        for(int y = 0; y < max_y; y++)
        {
            Rectangle texture_rect = {0.f, 0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE};
            if(y == 0)
                texture_rect.x = TILE_TEXTURE_SIZE;
            if(y > 5)
                texture_rect.x = TILE_TEXTURE_SIZE * 2.f;

            world.push_back(Tile(tileset_default_world, {TILE_SIZE * x, TILE_SIZE * y - (max_y * TILE_SIZE) + 720.f}, texture_rect, {TILE_SIZE, TILE_SIZE}));
        }
        max_y += rand() % 2 == 0 ? -1 : 1;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        for(Tile block : world)
            block.draw();

        EndDrawing();
    }
    UnloadTexture(tileset_default_world);

    CloseWindow();
    return EXIT_SUCCESS;
}
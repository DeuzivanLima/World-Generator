#include <iostream>
#include <raylib.h>
#include <vector>

#include "Config.hpp"

Texture2D tileset_default_world, button_arrow_texture, button_refresh_texture;

class Transform2D
{
public:
    Transform2D(const Vector2 position = {0.f, 0.f}, const Vector2 size = {16.f, 16.f}, const float rotation = 0.f) noexcept
    {
        this->setPosition(position);
        this->setRotation(rotation);
        this->setSize(size);
    }

    void setPosition(const Vector2 position) noexcept
    {
        this->__position = position;
    }
    const Vector2 &getPosition() const noexcept
    {
        return this->__position;
    }
    void setSize(const Vector2 size) noexcept
    {
        this->__size = size;
    }
    const Vector2 &getSize() const noexcept
    {
        return this->__size;
    }
    void setRotation(const float rotation) noexcept
    {
        this->__rotation = rotation;
    }
    const float &getRotation() const noexcept
    {
        return this->__rotation;
    }

    ~Transform2D() noexcept
    {
    }
private:
    Vector2 __position, __size;
    float __rotation;
};

class Sprite : public Transform2D
{
public:
    Sprite(Texture2D &texture, const Vector2 position = {0.f, 0.f}, const Vector2 size = {16.f, 16.f}, const float rotation = 0.f)
            : Transform2D(position, size, rotation)
    {
        this->__texture = &texture;
        this->setOrigin({0.f, 0.f});
        this->setTextureRect({0.f, 0.f, static_cast<float>(texture.width), static_cast<float>(texture.height)});
    }

    void setOrigin(const Vector2 origin) noexcept
    {
        this->__origin = origin;
    }
    const Vector2 &getOrigin() const noexcept
    {
        return this->__origin;
    }
    void setTextureRect(const Rectangle texture_rect) noexcept
    {
        this->__texture_rect = texture_rect;
    }
    const Rectangle &getTextureRect() const noexcept
    {
        return this->__texture_rect;
    }

    void draw() const noexcept
    {
        DrawTexturePro(*this->__texture,
                       this->getTextureRect(),
                       {this->getPosition().x, this->getPosition().y, this->getSize().x, this->getSize().y},
                       this->__origin, this->getRotation(), WHITE);
    }

    ~Sprite()
    {
    }
private:
    Texture2D *__texture;
    Rectangle __texture_rect;
    Vector2 __origin;
};

class Tile : public Sprite
{
public:
    Tile(Texture2D &texture, const Vector2 position = {0.f, 0.f}, const Rectangle texture_rect = {0.f, 0.f, 16.f, 16.f}, const Vector2 size = {16.f, 16.f}, const float resistence = 1.f)
            : Sprite(texture, position, size, 0.f)
    {
        this->setResistence(resistence);
        this->setTextureRect(texture_rect);
    }

    void setResistence(const float resistence) noexcept
    {
        this->__resistence = resistence;
    }
    const float &getResistence() const noexcept
    {
        return this->__resistence;
    }

    ~Tile()
    {
    }
private:
    float __resistence;
};

class Chunk
{
public:
    Chunk() noexcept
    {
        this->generate();
    }

    void clear() noexcept
    {
        this->__tiles.clear();
    }
    void draw(Vector2 offset = {0.f, 0.f}) const noexcept
    {
        for(Tile tile : this->__tiles)
        {
            tile.setPosition({tile.getPosition().x + offset.x, tile.getPosition().y + offset.y});
            tile.draw();
        }
    }
    void generate() noexcept
    {
        this->clear();
        int max_height = rand() % CHUNK_HEIGHT + 10;
        for(size_t x = 0; x < CHUNK_WIDTH; x++)
        {
            for(size_t y = 0; y < max_height; y++)
            {
                Rectangle texture_rect = {0.f, 0.f, TILE_TEXTURE_SIZE, TILE_TEXTURE_SIZE};
                if(y == 0)
                    texture_rect.x = TILE_TEXTURE_SIZE;
                if(y > 5)
                    texture_rect.x = TILE_TEXTURE_SIZE * 2.f;
                this->__tiles.push_back(Tile(tileset_default_world, {TILE_SIZE * x, TILE_SIZE * y + GetScreenHeight() - (max_height * TILE_SIZE)}, texture_rect, {TILE_SIZE, TILE_SIZE}));
            }
            max_height += rand() % 2 == 0 ? -1 : 1;
        }
    }

    ~Chunk()
    {
        this->__tiles.clear();
    }
private:
    std::vector<Tile> __tiles;
};


int main(int, char *[])
{
    srand(time(NULL));
    InitWindow(GetScreenWidth(), GetScreenHeight(), "World Generator");

    std::vector<Tile> world;
    tileset_default_world = LoadTexture("assets/tileset/world-001.png");
    button_arrow_texture = LoadTexture("assets/sprites/button-arrow.png");
    button_refresh_texture = LoadTexture("assets/sprites/button-refresh.png");

    Chunk chunk;
    Sprite
        button_right(button_arrow_texture, {GetScreenWidth() - 200.f, GetScreenHeight() - 200.f}, {72.f, 72.f}),
        button_left(button_arrow_texture, {GetScreenWidth() - 200.f - 80.f * 2.f, GetScreenHeight() - 200.f}, {72.f, 72.f}, 180.f),
        button_refresh(button_refresh_texture, {GetScreenWidth() - 200.f - 80.f, GetScreenHeight() - 200.f}, {72.f, 72.f});
    
    button_left.setOrigin({72.f, 72.f});

    Camera2D camera;
    camera.offset.x = 400;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), {button_right.getPosition().x, button_right.getPosition().y, button_right.getSize().x, button_right.getSize().y}))
            camera.offset.x -= 10.f;
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), {button_left.getPosition().x, button_left.getPosition().y, button_left.getSize().x, button_left.getSize().y}))
            camera.offset.x += 10.f;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), {button_refresh.getPosition().x, button_refresh.getPosition().y, button_refresh.getSize().x, button_refresh.getSize().y}))
            chunk.generate();

        BeginDrawing();
        ClearBackground(BLACK);

        chunk.draw(camera.offset);
        button_right.draw();
        button_left.draw();
        button_refresh.draw();

        EndDrawing();
    }
    UnloadTexture(tileset_default_world);
    UnloadTexture(button_arrow_texture);
    UnloadTexture(button_refresh_texture);

    CloseWindow();
    return EXIT_SUCCESS;
}
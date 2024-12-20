#include "Level.h"
#include "Utils.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

Level::Level(SDL_Renderer* renderer, b2WorldId worldId, std::string& assetDir, int windowWidth, int windowHeight, int tilesVertically)
    : renderer(renderer), worldId(worldId), assetDir(assetDir), windowWidth(windowWidth), windowHeight(windowHeight), tilesVertically(tilesVertically) {
    scale = 1.0F;
    offsetX = windowWidth / PIXELS_PER_METER / 2.0F;
    offsetY = windowHeight / PIXELS_PER_METER / 2.0F;

    spdlog::debug("Level initialized with scale: {}, offsetX: {}, offsetY: {}", scale, offsetX, offsetY);
}

Level::~Level() = default;

auto Level::loadTilemap(const std::string& filename) -> bool {
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Failed to open tilemap file: {}", filename);
        return false;
    }
    nlohmann::json tilemap;
    file >> tilemap;

    int mapHeight = tilemap["height"];
    tileWidth = tilemap["tilewidth"];
    tileHeight = tilemap["tileheight"];

    for (const auto& layer : tilemap["layers"]) {
        if (layer["type"] == "tilelayer") {
            int width = layer["width"];
            int height = layer["height"];
            const auto& data = layer["data"];

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    int tileId = data[(y * width) + x];
                    if (tileId == 1) {
                        createTile("ground", x * tileWidth, (height - y - 1) * tileHeight, false);
                    }
                    else if (tileId == 2) {
                        createTile("rectangle", x * tileWidth, (height - y - 1) * tileHeight, false);
                    }
                }
            }
        }
    }

    spdlog::info("Tilemap loaded successfully from file: {}", filename);
    return true;
}

void Level::render() {
    for (const auto& tile : tiles) {
        tile->render(scale, offsetX, offsetY, windowWidth, windowHeight);
    }
}

void Level::handleErrors() {
    // Handle errors...
}

void Level::setScale(float newScale) {
    scale = newScale;
    spdlog::debug("Scale set to: {}", scale);
}

void Level::setViewportCenter(float centerX, float centerY) {
    offsetX = centerX;
    offsetY = centerY;
    spdlog::debug("Viewport center set to: ({}, {})", centerX, centerY);
}

auto Level::getScale() const -> float {
    return scale;
}

auto Level::getOffsetX() const -> float {
    return offsetX;
}

auto Level::getOffsetY() const -> float {
    return offsetY;
}

void Level::createTile(const std::string& type, int x, int y, bool isDynamic) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (isDynamic) {
        bodyDef.type = b2_dynamicBody;
    }
    bodyDef.position = b2Vec2{static_cast<float>(x) / PIXELS_PER_METER, static_cast<float>(y) / PIXELS_PER_METER};

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Convert tile size from pixels to meters
    float halfWidth = (tileWidth / 2.0F) / PIXELS_PER_METER;
    float halfHeight = (tileHeight / 2.0F) / PIXELS_PER_METER;
    b2Polygon box = b2MakeBox(halfWidth, halfHeight);
    
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F;
    shapeDef.friction = 0.3F;
    b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);

    // Create and store tiles as shared pointers
    std::shared_ptr<Tile> tile = std::make_shared<Tile>(renderer, type, bodyId, shapeId, tileWidth, tileHeight, assetDir);
    tiles.push_back(tile);

    spdlog::debug("Tile created: type = {}, position = ({}, {}), isDynamic = {}", type, bodyDef.position.x, bodyDef.position.y, isDynamic);
}

void Level::update(float deltaTime, const b2Vec2& characterPosition) {
    // Adjust the camera position based on the character's position
    offsetX = characterPosition.x;
    offsetY = characterPosition.y;

    // Ensure the camera does not move beyond the level boundaries by clamping its position
    float levelWidth = tileWidth * tiles.size() / PIXELS_PER_METER;
    float levelHeight = tileHeight * tiles.size() / PIXELS_PER_METER;

    offsetX = std::clamp(offsetX, windowWidth / (2.0F * PIXELS_PER_METER), levelWidth - (windowWidth / (2.0F * PIXELS_PER_METER)));
    offsetY = std::clamp(offsetY, windowHeight / (2.0F * PIXELS_PER_METER), levelHeight - (windowHeight / (2.0F * PIXELS_PER_METER)));

    spdlog::debug("Level updated: deltaTime = {}, characterPosition = ({}, {})", deltaTime, characterPosition.x, characterPosition.y);
}

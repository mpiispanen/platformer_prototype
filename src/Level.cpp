#include "Level.h"
#include "Utils.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <queue>
#include "Box2DDebugDraw.h"

Level::Level(SDL_Renderer* renderer, b2WorldId worldId, std::string& assetDir, int windowWidth, int windowHeight, int tilesVertically)
    : renderer(renderer), worldId(worldId), assetDir(assetDir), windowWidth(windowWidth), windowHeight(windowHeight), tilesVertically(tilesVertically), showPolygonOutlines(false) {
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
    int mapWidth = tilemap["width"];
    tileWidth = tilemap["tilewidth"];
    tileHeight = tilemap["tileheight"];

    std::vector<std::vector<int>> tileData(mapHeight, std::vector<int>(mapWidth, 0));
    for (const auto& layer : tilemap["layers"]) {
        if (layer["type"] == "tilelayer") {
            const auto& data = layer["data"];
            for (int y = 0; y < mapHeight; ++y) {
                for (int x = 0; x < mapWidth; ++x) {
                    tileData[y][x] = data[(y * mapWidth) + x];
                }
            }
        }
    }

    std::vector<std::vector<bool>> visited(mapHeight, std::vector<bool>(mapWidth, false));
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if ((tileData[y][x] == 1 || tileData[y][x] == 2) && !visited[y][x]) {
                std::vector<b2Vec2> chainPoints;
                std::vector<std::pair<int, int>> chainTiles;
                std::queue<std::pair<int, int>> q;
                q.push({y, x});
                visited[y][x] = true;

                while (!q.empty()) {
                    auto [cy, cx] = q.front();
                    q.pop();

                    float left = static_cast<float>(cx * tileWidth) / PIXELS_PER_METER;
                    float right = static_cast<float>((cx + 1) * tileWidth) / PIXELS_PER_METER;
                    float top = static_cast<float>((mapHeight - cy - 1) * tileHeight) / PIXELS_PER_METER;
                    float bottom = static_cast<float>((mapHeight - cy) * tileHeight) / PIXELS_PER_METER;

                    chainPoints.push_back(b2Vec2{left, top});
                    chainPoints.push_back(b2Vec2{right, top});
                    chainPoints.push_back(b2Vec2{right, bottom});
                    chainPoints.push_back(b2Vec2{left, bottom});

                    chainTiles.push_back({cy, cx});

                    std::string tileType = (tileData[cy][cx] == 1) ? "ground" : "rectangle";
                    createTile(tileType, cx * tileWidth, (mapHeight - cy - 1) * tileHeight, false);

                    // Check adjacent tiles
                    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
                    for (const auto& [dy, dx] : directions) {
                        int ny = cy + dy;
                        int nx = cx + dx;
                        if (ny >= 0 && ny < mapHeight && nx >= 0 && nx < mapWidth && (tileData[ny][nx] == 1 || tileData[ny][nx] == 2) && !visited[ny][nx]) {
                            q.push({ny, nx});
                            visited[ny][nx] = true;
                        }
                    }
                }

                if (!chainPoints.empty()) {
                    createChainForStaticTiles(chainPoints, chainTiles, mapHeight);
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
        if (showPolygonOutlines) {
            tile->renderPolygonOutline(scale, offsetX, offsetY, windowWidth, windowHeight);
        }
    }
}

void Level::handleErrors() {
    // Handle errors...
}

void Level::setScale(float newScale) {
    scale = newScale;
    spdlog::debug("Scale set to: {}", scale);
}

float Level::getScale() const {
    return scale;
}

void Level::setViewportCenter(float centerX, float centerY) {
    offsetX = centerX;
    offsetY = centerY;
    spdlog::debug("Viewport center set to: ({}, {})", centerX, centerY);
}

auto Level::getOffsetX() const -> float {
    return offsetX;
}

auto Level::getOffsetY() const -> float {
    return offsetY;
}

void Level::setShowPolygonOutlines(bool show) {
    showPolygonOutlines = show;
}

void Level::createTile(const std::string& type, int x, int y, bool isDynamic) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position = b2Vec2{static_cast<float>(x) / PIXELS_PER_METER, static_cast<float>(y) / PIXELS_PER_METER};

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // No need to create a rectangular shape for static tiles
    b2ChainId chainId = b2_nullChainId;
    b2ShapeId shapeId = b2_nullShapeId;

    std::shared_ptr<Tile> tile = std::make_shared<Tile>(renderer, type, bodyId, chainId, shapeId, tileWidth, tileHeight, assetDir, bodyDef.position.x, bodyDef.position.y);
    tiles.push_back(tile);

    spdlog::debug("Tile created: type = {}, position = ({}, {}), isDynamic = {}", type, x, y, isDynamic);
}

void Level::createChainForStaticTiles(const std::vector<b2Vec2>& points, const std::vector<std::pair<int, int>>& chainTiles, int mapHeight) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2ChainDef chainDef = b2DefaultChainDef();
    chainDef.points = points.data();
    chainDef.count = points.size();

    b2ChainId chainId = b2CreateChain(bodyId, &chainDef);
    b2ShapeId shapeId = b2_nullShapeId;

    // Update the tiles to reference the chain shape
    for (const auto& [cy, cx] : chainTiles) {
        for (auto& tile : tiles) {
            if (tile->getX() == cx * tileWidth && tile->getY() == (mapHeight - cy - 1) * tileHeight) {
                tile = std::make_shared<Tile>(renderer, tile->getType(), bodyId, chainId, shapeId, tileWidth, tileHeight, assetDir, cx * tileWidth, (mapHeight - cy - 1) * tileHeight);
            }
        }
    }

    spdlog::debug("Chain created with {} points", points.size());
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

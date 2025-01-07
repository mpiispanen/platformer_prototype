#include "Level.h"
#include "Utils.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <set>
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
                    createChainForStaticTiles(chainTiles, mapHeight);
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

bool Level::isSolidTile(int x, int y, const std::vector<std::pair<int, int>>& chainTiles) {
    for (const auto& [cy, cx] : chainTiles) {
        if (cx == x && cy == y) return true;
    }
    return false;
}

std::tuple<int, int, int> Level::findStartingTile(const std::vector<std::pair<int, int>>& chainTiles, const std::vector<std::vector<bool>>& visited) {
    // Directions for checking neighbors (clockwise)
    const std::vector<std::pair<int, int>> directions = {
        {0, 1},  // East
        {1, 0},  // South
        {0, -1}, // West
        {-1, 0}  // North
    };
    
    // Mapping from 4-direction index to 8-direction index
    const std::vector<int> directionMapping = {3, 1, 7, 5};
    
    for (const auto& [cy, cx] : chainTiles) {
        if (!visited[cy][cx]) {
            // Check if the tile is on the border
            for (int i = 0; i < directions.size(); ++i) {
                int ny = cy + directions[i].first;
                int nx = cx + directions[i].second;
                
                // Check if the neighbor is out of bounds or not in the chainTiles list
                if (ny < 0 || ny >= visited.size() || nx < 0 || nx >= visited[0].size() || !isSolidTile(nx, ny, chainTiles)) {
                    // Return the starting tile coordinates and the direction pointing towards the empty tile
                    return {cy, cx, directionMapping[i]}; // Map to the corresponding 8-direction index
                }
            }
        }
    }
    return {-1, -1, -1};
}

std::pair<std::vector<std::pair<int, int>>, std::vector<b2Vec2>> Level::traceBorder(int startX, int startY, int startDir, const std::vector<std::pair<int, int>>& chainTiles, std::vector<std::vector<bool>>& visited, int mapHeight, int tileWidth, int tileHeight) {
    std::vector<std::pair<int, int>> outlineTiles;
    std::vector<b2Vec2> outlineVertices;
    int x = startX, y = startY;
    int dir = startDir; // Start with the initial direction
    
    // Directions for Moore-Neighbor Tracing (counter-clockwise)
    const std::vector<std::pair<int, int>> directions = {
        {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}
    };
    
    std::set<int> firstTileDirections;
    bool isFirstTile = true;
    bool finishedTracing = false;
    
    do {
        visited[y][x] = true;
        outlineTiles.push_back({y, x});
        bool found = false;
        
        for (int i = 0; i < directions.size(); ++i) {
            int newDir = (dir + i) % directions.size();
            int nx = x + directions[newDir].first;
            int ny = y + directions[newDir].second;
            
            if(!isSolidTile(nx, ny, chainTiles))
            {
                if (newDir == 5) { // North
                    outlineVertices.push_back(b2Vec2{static_cast<float>(x + 1.0f), static_cast<float>((mapHeight - y))});
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x)), static_cast<float>((mapHeight - y))});
                } else if (newDir == 3) { // East
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x + 1.0f)), static_cast<float>((mapHeight - y - 1.0f))});
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x + 1.0f)), static_cast<float>((mapHeight - y))});
                } else if (newDir == 1) { // South
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x)), static_cast<float>((mapHeight - y - 1.0f))});
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x + 1.0f)), static_cast<float>((mapHeight - y - 1.0f))});
                } else if (newDir == 7) { // West
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x)), static_cast<float>((mapHeight - y))});
                    outlineVertices.push_back(b2Vec2{static_cast<float>((x)), static_cast<float>((mapHeight - y - 1.0f))});
                }
            }
            else {
                // Add the current tile to the outlineTiles
                outlineTiles.push_back({ny, nx});
                
                // Move to the next solid tile
                x = nx;
                y = ny;
                
                // Change the direction to start rotating from the direction we entered the tile
                dir = (newDir + 6) % directions.size(); // Adjust to point towards the left
                found = true;

                isFirstTile = (x == startX && y == startY);

                if (isFirstTile) {
                    if (firstTileDirections.find(dir) != firstTileDirections.end()) {
                        finishedTracing = true;
                        break;
                    } else {
                        firstTileDirections.insert(dir);
                    }
                }
                break;
            }
            
        }
        
        if (!found) {
            // If no valid neighbor found, turn right
            dir = (dir + 1) % directions.size();
        }
    } while (!finishedTracing);
    
    return {outlineTiles, outlineVertices};
}

void Level::createChainForStaticTiles(const std::vector<std::pair<int, int>>& chainTiles, int mapHeight) {
    if (chainTiles.empty()) return;
    
    // Determine the maximum width of the map
    int mapWidth = 0;
    for (const auto& [cy, cx] : chainTiles) {
        if (cx > mapWidth) mapWidth = cx;
    }
    mapWidth += 1; // Adjust for 0-based indexing
    
    // Initialize visited tiles
    std::vector<std::vector<bool>> visited(mapHeight, std::vector<bool>(mapWidth, false));
    
    // Find and trace all islands
    while (true) {
        auto [startY, startX, startDir] = findStartingTile(chainTiles, visited);
        if (startX == -1 && startY == -1) break; // No more unvisited tiles
        
        spdlog::debug("Tracing border starting at ({}, {})", startX, startY);
        auto [outlineTiles, outlineVertices] = traceBorder(startX, startY, startDir, chainTiles, visited, mapHeight, tileWidth, tileHeight);
        spdlog::debug("Border traced with {} points", outlineVertices.size());
        
        // Create the body and chain shape
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
        
        b2ChainDef chainDef = b2DefaultChainDef();
        chainDef.points = outlineVertices.data();
        chainDef.count = outlineVertices.size();
        chainDef.isLoop = true;
        chainDef.friction = 5.0F;
        
        b2ChainId chainId = b2CreateChain(bodyId, &chainDef);
        b2ShapeId shapeId = b2_nullShapeId;
        
        // Update the tiles to reference the chain shape
        for (const auto& [cy, cx] : chainTiles) {
            if (visited[cy][cx]) {
                for (auto& tile : tiles) {
                    if (tile->getX() == cx * tileWidth && tile->getY() == (mapHeight - cy - 1) * tileHeight) {
                        tile = std::make_shared<Tile>(renderer, tile->getType(), bodyId, chainId, shapeId, tileWidth, tileHeight, assetDir, cx * tileWidth, (mapHeight - cy - 1) * tileHeight);
                    }
                }
            }
        }
        
        spdlog::debug("Chain created with {} points", outlineVertices.size());
    }
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

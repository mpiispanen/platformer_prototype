#include "GameSettingsObserver.h"
#include <box2d/box2d.h>

GameSettingsObserver::GameSettingsObserver(b2WorldId worldId, Character& character)
    : worldId(worldId), character(character) {}

void GameSettingsObserver::onSettingChanged(const std::string& settingName, float newValue) {
    if (settingName == "gravity") {
        b2World_SetGravity(worldId, b2Vec2{0.0f, newValue});
    } else if (settingName == "characterSpeed") {
        character.setMaxWalkingSpeed(newValue);
    } else if (settingName == "jumpStrength") {
        character.setJumpStrength(newValue);
    } else if (settingName == "jumpCooldownDuration") {
        character.setJumpCooldownDuration(newValue);
    } else if (settingName == "groundAcceleration") {
        character.setGroundAcceleration(newValue);
    } else if (settingName == "airAcceleration") {
        character.setAirAcceleration(newValue);
    } else if (settingName == "maxWalkingSpeed") {
        character.setMaxWalkingSpeed(newValue);
    } else if (settingName == "showDebugVisualizations") {
        character.setShowDebugRectangles(static_cast<bool>(newValue));
    } else if (settingName == "showContactPoints") {
        character.setShowContactPoints(static_cast<bool>(newValue));
    } else if (settingName == "showForceVisualizations") {
        character.setShowForceVectors(static_cast<bool>(newValue));
    } else if (settingName == "maxContactPoints") {
        character.setMaxContactPoints(static_cast<int>(newValue));
    }
}

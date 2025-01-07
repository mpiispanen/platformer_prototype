#pragma once
#include "Observer.h"
#include <box2d/box2d.h>
#include "Character.h"

class GameSettingsObserver : public Observer {
public:
    GameSettingsObserver(b2WorldId worldId, Character& character);
    virtual void onSettingChanged(const std::string& settingName, float newValue) override;

private:
    b2WorldId worldId;
    Character& character;

    float jumpStrength;
    float minJumpHeight;
    float maxJumpHeight;
    float jumpCooldownDuration;

    void handleDebugVisualizationSetting(const std::string& settingName, float newValue);
};

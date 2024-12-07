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
};
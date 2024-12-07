#pragma once

#include <string>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onSettingChanged(const std::string& settingName, float newValue) = 0;
};
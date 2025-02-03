#pragma once
#include "GameObjects.h"

class LevelBackground
{
public:

    LevelBackground(std::string filepath) : background_path(filepath)
    {};

    std::string background_path = "";

    struct
    {
        float x = 1.0f;
        float y = 1.0f;
    }position;

    virtual void OnUpdate() {};
};
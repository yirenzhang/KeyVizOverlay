#pragma once

namespace keyviz
{
struct KeyState
{
    bool isDown = false;
    bool pressedThisFrame = false;
    bool releasedThisFrame = false;
    float downTimeSeconds = 0.0f;
    float animationProgress = 0.0f;
};
}

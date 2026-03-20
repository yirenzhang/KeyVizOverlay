#pragma once

namespace keyviz
{
class SweepLightEffect
{
public:
    void Reset();
    void SetEnabled(bool enabled);
    void SetSpeed(float speed);
    void Update(float deltaSeconds);

    bool IsEnabled() const;
    float GetProgress() const;

private:
    bool m_enabled = false;
    float m_progress = 0.0f;
    float m_speed = 1.0f;
};
}

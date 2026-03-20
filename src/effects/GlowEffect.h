#pragma once

namespace keyviz
{
class GlowEffect
{
public:
    void Reset();
    void SetTargetIntensity(float intensity);
    void Update(float deltaSeconds);

    float GetCurrentIntensity() const;

private:
    float m_currentIntensity = 0.0f;
    float m_targetIntensity = 0.0f;
    float m_fadeSpeed = 8.0f;
};
}

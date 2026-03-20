#include "GlowEffect.h"

namespace keyviz
{
void GlowEffect::Reset()
{
    m_currentIntensity = 0.0f;
    m_targetIntensity = 0.0f;
}

void GlowEffect::SetTargetIntensity(float intensity)
{
    m_targetIntensity = intensity;
}

void GlowEffect::Update(float deltaSeconds)
{
    if (deltaSeconds < 0.0f)
    {
        deltaSeconds = 0.0f;
    }

    const float delta = m_targetIntensity - m_currentIntensity;
    const float step = m_fadeSpeed * deltaSeconds;

    if (delta > step)
    {
        m_currentIntensity += step;
    }
    else if (delta < -step)
    {
        m_currentIntensity -= step;
    }
    else
    {
        m_currentIntensity = m_targetIntensity;
    }
}

float GlowEffect::GetCurrentIntensity() const
{
    return m_currentIntensity;
}
}

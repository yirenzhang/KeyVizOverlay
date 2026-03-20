#include "SweepLightEffect.h"

namespace keyviz
{
void SweepLightEffect::Reset()
{
    m_enabled = false;
    m_progress = 0.0f;
}

void SweepLightEffect::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

void SweepLightEffect::SetSpeed(float speed)
{
    m_speed = speed;
}

void SweepLightEffect::Update(float deltaSeconds)
{
    if (!m_enabled)
    {
        return;
    }

    if (deltaSeconds < 0.0f)
    {
        deltaSeconds = 0.0f;
    }

    m_progress += m_speed * deltaSeconds;
    while (m_progress >= 1.0f)
    {
        m_progress -= 1.0f;
    }
}

bool SweepLightEffect::IsEnabled() const
{
    return m_enabled;
}

float SweepLightEffect::GetProgress() const
{
    return m_progress;
}
}

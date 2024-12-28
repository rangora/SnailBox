#include "Actor.h"

namespace sb
{
    void Actor::Tick(float delta)
    {
    }

    void Actor::SetPosition(const Vector3d position)
    {
        _transform.SetTranslation(position);
    }

    void Actor::SetRotation(const Vector3d rotation)
    {
        _transform.SetRotation(rotation);
    }

    void Actor::SetScale(const Vector3d scale)
    {
        _transform.SetScale(scale);
    }

} // namespace sb
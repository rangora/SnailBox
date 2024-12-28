#include "Actor.h"
#include "World/Worldinstance.h"
#include "World/World.h"

namespace sb
{
    Actor::Actor()
    {
        WorldInstance& wInstance = WorldInstance::Get();
        _world = wInstance.GetWorld();
        _world->RegisterActor(this);
    }

    Actor::~Actor()
    {
    }

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
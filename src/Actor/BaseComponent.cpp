#include "BaseComponent.h"

namespace sb
{
    BaseComponent::BaseComponent(Actor* in_parentActor)
    : ParentActor(in_parentActor)
    {
    }

    BaseComponent::~BaseComponent()
    {
    }
    
    void BaseComponent::Draw()
    {
    }
} // namespace sb

#pragma once

namespace sb
{
    class Layout
    {
        public:
            virtual void OnAttach() {}
            virtual void OnDetach() {}
            virtual void OnUpdate(float in_delta) {}
    };
};
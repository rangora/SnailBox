#include "RenderResourceStorage.h"

namespace sb
{
    RenderResourceStorage::RenderResourceStorage()
    {
        FillBaseRenderResource(_customData);

        s_instnace = this;
    }

    void FillBaseRenderResource(std::vector<CustomRenderResource>& resources)
    {
        {
            CustomRenderResource resource1("sample1");
            resource1._RawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                               Vertex({0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}),
                                               Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f})});
            resource1._RawData._index.assign({0, 1, 2, 0, 3, 1});
            resource1._vsPath = projectPath + "/resources/shader/hlsl/sample1.vs.hlsl";
            resource1._psPath = projectPath + "/resources/shader/hlsl/sample1.fs.hlsl";

            resources.emplace_back(std::move(resource1));
        }
        {
            CustomRenderResource resource2("sample2");
            resource2._RawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),

                                               Vertex({-0.75f, 0.75f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                               Vertex({0.0f, 0.f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                               Vertex({-0.75f, 0.f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                               Vertex({0.f, 0.75f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f})});
            resource2._RawData._index.assign({0, 1, 2, 0, 3, 1});
            resource2._vsPath = projectPath + "/resources/shader/hlsl/sample1.vs.hlsl";
            resource2._psPath = projectPath + "/resources/shader/hlsl/sample1.fs.hlsl";

            resources.emplace_back(std::move(resource2));
        }
        {
            CustomRenderResource resource3("sample3");
            resource3._RawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                               Vertex({0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                               Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f, 1.0f})});
            resource3._RawData._index.assign({0, 1, 2, 0, 3, 1});
            resource3._vsPath = projectPath + "/resources/shader/hlsl/sample.vs.hlsl";
            resource3._psPath = projectPath + "/resources/shader/hlsl/sample.fs.hlsl";

            resources.emplace_back(std::move(resource3));
        }
        {
            CustomRenderResource resource4("sample4");
            resource4._RawData._vertex.assign({
                Vertex({-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // front

                Vertex({0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // right

                Vertex({-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // left

                Vertex({ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // back

                Vertex({-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({ 0.5f, 0.5f,  0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({ 0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // top

                Vertex({ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                Vertex({-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                Vertex({-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // bottom
                });


            resource4._RawData._index.assign({
                0,  1,  2,  0,  3,  1,
                4,  5,  6,  4,  7,  5,
                8,  9,  10, 8,  11, 9,
                12, 13, 14, 12, 15, 13,
                16, 17, 18, 16, 19, 17,
                20, 21, 22, 20, 24, 21});
            resource4._vsPath = projectPath + "/resources/shader/hlsl/sample4.vs.hlsl";
            resource4._psPath = projectPath + "/resources/shader/hlsl/sample4.fs.hlsl";

            resources.emplace_back(std::move(resource4));
        }
    }
} // namespace sb
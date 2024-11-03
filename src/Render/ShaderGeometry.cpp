#include "ShaderGeometry.h"

namespace sb
{
    UPtr<ShaderGeometry> DxShaderGeometryFactory::CreateGeometry(const ShaderGeometryRawData& data)
    {
        UPtr<DxShaderGeometry> geo = CreateUPtr<DxShaderGeometry>();

        for (const auto& vertex : data._vertex)
        {
            geo->_vertex.emplace_back(
                DxVertex({XMFLOAT3(vertex._pos.X, vertex._pos.Y, vertex._pos.Z),
                          XMFLOAT4(vertex._color.X, vertex._color.Y, vertex._color.Z, vertex._color.W)}));
        }

        geo->_index.assign(data._index.begin(), data._index.end());

        return std::move(geo);
    }
} // namespace sb

#pragma once

#include "corepch.h"
#include "core/math/Vector.h"
#include "core/Common.h"
#include "Vertex.h"
#include <vector>

namespace sb
{
    struct ShaderGeometryRawData
    {
        std::vector<Vertex> _vertex;
        std::vector<int32> _index; 
    };

    struct ShaderGeometry
    {
        ShaderGeometry() = default;
        virtual ~ShaderGeometry() = default;

        virtual void* GetVertexPointer() { return nullptr; }
        virtual void* GetIndexPointer() { return nullptr; }
        virtual uint32 GetVertexByteSize() const = 0;
        virtual uint32 GetIndexByteSize() const = 0;
        virtual uint32 GetStrideSize() const = 0;
    };

    struct DxShaderGeometry : public ShaderGeometry
    {
        std::vector<DxVertex> _vertex;
        std::vector<DWORD> _index;

        DxShaderGeometry() = default;
        ~DxShaderGeometry() = default;

        void* GetVertexPointer() final { return _vertex.data(); }
        void* GetIndexPointer() final { return _index.data(); }
        uint32 GetVertexByteSize() const { return sizeof(DxVertex) * _vertex.size(); }
        uint32 GetIndexByteSize() const { return sizeof(DWORD) * _index.size(); }
        uint32 GetStrideSize() const final { return sizeof(DxVertex); }
    };

    class ShaderGeometryFactory
    {
    public:
        ShaderGeometryFactory() = default;
        virtual ~ShaderGeometryFactory(){};

        virtual UPtr<ShaderGeometry> CreateGeometry(const ShaderGeometryRawData& data) = 0;
    };

    class DxShaderGeometryFactory : public ShaderGeometryFactory
    {
    public:
        DxShaderGeometryFactory() = default;
        ~DxShaderGeometryFactory() {}

        UPtr<ShaderGeometry> CreateGeometry(const ShaderGeometryRawData& data) final;
    };
}
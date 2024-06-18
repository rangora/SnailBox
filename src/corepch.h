#pragma once

#include "Core/Common.h"
#include "Core/Application.h"
#include "spdlog/spdlog.h"
#include "dxgi1_4.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>

#define sg_d3dDevice sb::Application::GetD3Device()->GetDevice()
#define sg_d3dDriver sb::Application::GetDirectXCanvas()
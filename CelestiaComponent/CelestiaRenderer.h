//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celestia/celestiacore.h>

#include <epoxy/wgl.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <thread>
#include <mutex>
#include "CelestiaRenderer.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer>
    {
        CelestiaRenderer(bool enableMultisample, CelestiaComponent::CelestiaRendererEngineStartedHandler const& engineStarted);

        bool Initialize();
        bool Prepare();
        void Destroy();
        inline bool ResizeIfNeeded();
        inline void TickAndDraw();
        void Start();
        void Stop();
        inline void Lock();
        inline void Unlock();
        void Pause();
        void Resume();
        inline void Wait();
        void SetSurface(Microsoft::UI::Xaml::Controls::SwapChainPanel const& surface, float scale);
        void SetCorePointer(int64_t core);
        void EnqueueTask(CelestiaComponent::CelestiaRendererTask const& task);
        void SetPreRenderTask(CelestiaComponent::CelestiaRendererTask const& task);
        std::pair<std::vector<CelestiaComponent::CelestiaRendererTask>, CelestiaComponent::CelestiaRendererTask> RetrieveAndResetTasks();

        int32_t StartReadingBackBuffer();
        void EndReadingBackBuffer(int32_t);

        CelestiaCore* core = nullptr;

        enum RenderThreadMessage {
            MSG_NONE = 0,
            MSG_WINDOW_SET,
        };

        enum RenderThreadMessage msg = MSG_NONE;

        bool enableMultisample = false;
        bool engineStartedCalled = false;

        winrt::com_ptr<ID3D11Device> device{ nullptr };
        winrt::com_ptr<ID3D11DeviceContext> deviceContext{ nullptr };
        winrt::com_ptr<IDXGIFactory2> dxgiFactory{ nullptr };
        winrt::com_ptr<IDXGISwapChain1> swapChain{ nullptr };

        HWND glWindow{ nullptr };
        HDC glDC{ nullptr };
        HANDLE glDeviceHandle{ nullptr };
        HGLRC glContext{ nullptr };
        GLuint depthRenderBuffer{ 0 };
        GLuint frameBuffer{ 0 };
        GLuint sampleFrameBuffer{ 0 };
        GLuint sampleColorBuffer{ 0 };
        GLuint sampleDepthBuffer{ 0 };
        GLuint glTexture{ 0 };

        HANDLE interopColorHandle{ nullptr };

    private:
        bool suspendedFlag = false;
        CRITICAL_SECTION msgCritSection;
        CONDITION_VARIABLE resumeCond;

        int newWindowWidth = 0;
        int newWindowHeight = 0;
        int currentWindowWidth = 0;
        int currentWindowHeight = 0;
        int currentWindowWidthScaled = 0;
        int currentWindowHeightScaled = 0;
        float windowScale = 1.0f;

        Microsoft::UI::Xaml::Controls::SwapChainPanel window{ nullptr };

        Windows::Foundation::IAsyncAction mRenderLoopWorker{ nullptr };

        std::vector<CelestiaComponent::CelestiaRendererTask> tasks;
        CelestiaComponent::CelestiaRendererTask preRenderTask;

        CelestiaComponent::CelestiaRendererEngineStartedHandler engineStarted;
        bool enableMSAA;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer, implementation::CelestiaRenderer>
    {
    };
}

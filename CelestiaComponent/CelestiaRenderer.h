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

#ifndef EGL_EGL_PROTOTYPES
#define EGL_EGL_PROTOTYPES 1
#endif

#include <EGL/egl.h>
#include <thread>
#include <mutex>
#include "CelestiaRenderer.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer>
    {
        CelestiaRenderer(bool enableMultisample, CelestiaComponent::CelestiaRendererEngineStartedHandler const& engineStarted);

        bool Initialize();
        void Destroy();
        inline void ResizeIfNeeded();
        inline void TickAndDraw() const;
        void Start();
        void Stop();
        inline void Lock();
        inline void Unlock();
        void Pause();
        void Resume();
        inline void Wait();
        void SetSurface(Windows::UI::Xaml::Controls::SwapChainPanel const& surface, float scale);
        void SetCorePointer(int64_t core);
        void EnqueueTask(CelestiaComponent::CelestiaRendererTask const& task);
        void SetPreRenderTask(CelestiaComponent::CelestiaRendererTask const& task);
        std::pair<std::vector<CelestiaComponent::CelestiaRendererTask>, CelestiaComponent::CelestiaRendererTask> RetrieveAndResetTasks();

        void MakeContextCurrent();

        CelestiaCore* core = nullptr;

        enum RenderThreadMessage {
            MSG_NONE = 0,
            MSG_WINDOW_SET,
        };

        enum RenderThreadMessage msg = MSG_NONE;

        bool enableMultisample = false;
        bool engineStartedCalled = false;

        EGLDisplay display = EGL_NO_DISPLAY;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;
        EGLConfig config{};
        EGLint format{};

    private:
        bool suspendedFlag = false;
        CRITICAL_SECTION msgCritSection;
        CONDITION_VARIABLE resumeCond;

        int currentWindowWidth = 0;
        int currentWindowHeight = 0;
        float windowScale = 1.0f;

        Windows::UI::Xaml::Controls::SwapChainPanel window{ nullptr };

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

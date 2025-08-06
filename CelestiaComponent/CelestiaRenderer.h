// CelestiaRenderer.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celestia/celestiacore.h>

#ifndef EGL_EGL_PROTOTYPES
#define EGL_EGL_PROTOTYPES 1
#endif

#include <epoxy/egl.h>
#include <thread>
#include <mutex>

#include "CelestiaRenderer.g.h"
#include "completion_source.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer>
    {
        CelestiaRenderer(bool enableMultisample, CelestiaComponent::CelestiaRendererEngineStartedHandler const& engineStarted);

        bool Initialize();
        void Destroy();
        inline void ResizeIfNeeded();
        inline void TickAndDraw() const;
        Windows::Foundation::IAsyncAction Start();
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
        static DWORD WINAPI Main(LPVOID context);

        void MakeContextCurrent();

        CelestiaCore* core = nullptr;

        enum RenderThreadMessage {
            MSG_NONE = 0,
            MSG_WINDOW_SET,
            MSG_RENDER_LOOP_EXIT,
        };

        enum RenderThreadMessage msg = MSG_NONE;

        bool enableMultisample = false;
        bool engineStartedCalled = false;

        EGLDisplay display = EGL_NO_DISPLAY;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;
        EGLConfig config { nullptr };
        EGLint format{};

    private:
        bool suspendedFlag = false;
        CRITICAL_SECTION msgCritSection;
        CONDITION_VARIABLE resumeCond;

        int currentWindowWidth { 0 };
        int currentWindowHeight { 0 };
        float windowScale { 1.0f };
        int sampleCount { 0 };

        Microsoft::UI::Xaml::Controls::SwapChainPanel window{ nullptr };

        DWORD threadID{ 0 };
        HANDLE threadHandle{ 0 };

        std::vector<CelestiaComponent::CelestiaRendererTask> tasks;
        CelestiaComponent::CelestiaRendererTask preRenderTask;

        CelestiaComponent::CelestiaRendererEngineStartedHandler engineStarted;
        bool enableMSAA;

        completion_source<bool> *startCompletionSource{ nullptr };
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer, implementation::CelestiaRenderer>
    {
    };
}

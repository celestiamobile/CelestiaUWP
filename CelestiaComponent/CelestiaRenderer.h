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
        CelestiaRenderer(CelestiaComponent::CelestiaRendererEngineStartedHandler const& engineStarted);

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
        void SetSize(int32_t width, int32_t height);
        void SetCorePointer(int64_t core);
        void EnqueueTask(CelestiaComponent::CelestiaRendererTask const& task);

        void FlushTasks();

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

        int windowWidth = 0;
        int windowHeight = 0;
        int currentWindowWidth = 0;
        int currentWindowHeight = 0;
        float windowScale = 1.0f;

        Windows::UI::Xaml::Controls::SwapChainPanel window{ nullptr };

        Windows::Foundation::IAsyncAction mRenderLoopWorker{ nullptr };

        std::vector<CelestiaComponent::CelestiaRendererTask> tasks;

        CelestiaComponent::CelestiaRendererEngineStartedHandler engineStarted;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaRenderer : CelestiaRendererT<CelestiaRenderer, implementation::CelestiaRenderer>
    {
    };
}

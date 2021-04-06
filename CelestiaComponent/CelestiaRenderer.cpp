//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaRenderer.h"
#if __has_include("CelestiaRenderer.g.cpp")
#include "CelestiaRenderer.g.cpp"
#endif

#ifndef EGL_EGLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif

#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <angle_windowsstore.h>

#define EGL_PLATFORM_ANGLE_ANGLE 0x3202
#define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
#define EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE 0x3204
#define EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE 0x3205
#define EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE 0x3206
#define EGL_PLATFORM_ANGLE_DEBUG_LAYERS_ENABLED_ANGLE 0x3451
#define EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE 0x3207
#define EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE 0x3208
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE 0x3209
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE 0x320A
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE 0x320B
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_REFERENCE_ANGLE 0x320C
#define EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE 0x320F

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaRenderer::CelestiaRenderer(bool enableMultisample, CelestiaComponent::CelestiaRendererEngineStartedHandler const& engineStarted) :
        CelestiaRendererT<CelestiaRenderer>(),
        engineStarted(engineStarted),
        enableMultisample(enableMultisample)
    {
        InitializeCriticalSection(&msgCritSection);
        InitializeConditionVariable(&resumeCond);
    }

    bool CelestiaRenderer::Initialize()
    {
        if (context == EGL_NO_CONTEXT)
        {
            printf("Initializing context");

            const EGLint multisampleAttribs[] =
            {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_SAMPLES, 4,
                EGL_SAMPLE_BUFFERS, 1,
                EGL_NONE
            };
            const EGLint attribs[] =
            {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_NONE
            };

            const EGLint defaultDisplayAttributes[] =
            {
                // These are the default display attributes, used to request ANGLE's D3D11 renderer.
                // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
                EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

                // EGL.PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
                // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
                // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
                EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
                EGL_NONE,
            };

            const EGLint fl9_3DisplayAttributes[] =
            {
                // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
                // These attributes are used if the call to eglInitialize fails with the default display attributes.
                EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
                EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
                EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
                EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
                EGL_NONE,
            };

            const EGLint warpDisplayAttributes[] =
            {
                // These attributes can be used to request D3D11 WARP.
                // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
                EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
                EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
                EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
                EGL_NONE,
            };

            EGLint numConfigs;

            display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
            if (display == EGL_NO_DISPLAY)
            {
                printf("eglGetDisplay() returned error %d", eglGetError());
                return false;
            }

            if (eglInitialize(display, nullptr, nullptr) == EGL_FALSE)
            {
                // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
                Destroy();
                display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY,
                    fl9_3DisplayAttributes);
                if (display == EGL_NO_DISPLAY)
                {
                    printf("eglGetDisplay() returned error %d", eglGetError());
                    return false;
                }

                if (eglInitialize(display, nullptr, nullptr) == EGL_FALSE)
                {
                    Destroy();

                    // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
                    display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY,
                        warpDisplayAttributes);
                    if (display == EGL_NO_DISPLAY)
                    {
                        printf("eglGetDisplay() returned error %d", eglGetError());
                        return false;
                    }

                    if (eglInitialize(display, nullptr, nullptr) == EGL_FALSE)
                    {
                        // If all of the calls to eglInitialize returned EGL.FALSE then an error has occurred.
                        Destroy();
                        printf("eglInitialize() returned error %d", eglGetError());
                        return false;
                    }
                }
            }

            if (enableMultisample) {
                // Try to enable multisample but fallback if not available
                if (!eglChooseConfig(display, multisampleAttribs, &config, 1, &numConfigs) && !eglChooseConfig(display, attribs, &config, 1, &numConfigs))
                {
                    printf("eglChooseConfig() returned error %d", eglGetError());
                    Destroy();
                    return false;
                }
            }
            else {
                if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
                {
                    printf("eglChooseConfig() returned error %d", eglGetError());
                    Destroy();
                    return false;
                }
            }

            if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
            {
                printf("eglGetConfigAttrib() returned error %d", eglGetError());
                Destroy();
                return false;
            }

            const EGLint contextAttributes[] =
            {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };

            if (!(context = eglCreateContext(display, config, nullptr, contextAttributes)))
            {
                printf("eglCreateContext() returned error %d", eglGetError());
                Destroy();
                return false;
            }
        }

        if (surface != EGL_NO_SURFACE)
        {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }

        if (window)
        {
            const EGLint surfaceAttributes[] =
            {
                EGL_NONE
            };

            Windows::Foundation::Collections::PropertySet propertySet;
            propertySet.Insert(EGLNativeWindowTypeProperty, window);
            propertySet.Insert(EGLRenderResolutionScaleProperty, Windows::Foundation::PropertyValue::CreateSingle(windowScale));
            EGLNativeWindowType win = reinterpret_cast<EGLNativeWindowType>(get_abi(propertySet));

            if (!(surface = eglCreateWindowSurface(display, config, win, surfaceAttributes)))
            {
                printf("eglCreateWindowSurface() returned error %d", eglGetError());
                Destroy();
                return false;
            }

            if (!eglMakeCurrent(display, surface, surface, context))
            {
                printf("eglMakeCurrent() returned error %d", eglGetError());
                Destroy();
                return false;
            }
        }
        return true;
    }

    void CelestiaRenderer::Destroy()
    {
        if (context != EGL_NO_CONTEXT)
        {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(display, context);
            if (surface != EGL_NO_SURFACE)
                eglDestroySurface(display, surface);
            eglTerminate(display);
        }
        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;
        currentWindowWidth = 0;
        currentWindowHeight = 0;
    }

    inline void CelestiaRenderer::ResizeIfNeeded()
    {
        if (currentWindowHeight != windowHeight || currentWindowWidth != windowWidth)
        {
            core->resize(windowWidth * windowScale, windowHeight * windowScale);
            currentWindowWidth = windowWidth;
            currentWindowHeight = windowHeight;
        }
    }

    inline void CelestiaRenderer::TickAndDraw() const
    {
        core->tick();
        core->draw();
    }

	void CelestiaRenderer::Start()
	{
        // If the render loop is already running then do not start another thread.
        if (mRenderLoopWorker != nullptr && mRenderLoopWorker.Status() == Windows::Foundation::AsyncStatus::Started)
        {
            return;
        }

        // Create a task for rendering that will be run on a background thread.
        auto workItemHandler = Windows::System::Threading::WorkItemHandler([this](Windows::Foundation::IAsyncAction action)
            {
                auto renderer = this;
                while (action.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
                {
                    if (renderer->surface != EGL_NO_SURFACE && !renderer->engineStartedCalled)
                    {
                        bool started = renderer->engineStarted();
                        if (!started)
                            break;
                        renderer->engineStartedCalled = true;
                    }

                    renderer->Lock();
                    renderer->Wait();

                    if (renderer->engineStartedCalled)
                        renderer->FlushTasks();

                    switch (renderer->msg)
                    {
                    case CelestiaRenderer::MSG_WINDOW_SET:
                        renderer->Initialize();
                        break;
                    default:
                        break;
                    }
                    renderer->msg = CelestiaRenderer::MSG_NONE;

                    bool needsDrawn = false;
                    if (renderer->engineStartedCalled && renderer->surface != EGL_NO_SURFACE && renderer->core)
                    {
                        renderer->ResizeIfNeeded();
                        needsDrawn = true;
                    }
                    renderer->Unlock();

                    if (needsDrawn)
                    {
                        renderer->TickAndDraw();
                        if (!eglSwapBuffers(renderer->display, renderer->surface))
                            printf("eglSwapBuffers() returned error %d", eglGetError());
                    }
                }
                renderer->Destroy();
            });

        // Run task on a dedicated high priority background thread.
        mRenderLoopWorker = Windows::System::Threading::ThreadPool::RunAsync(workItemHandler, Windows::System::Threading::WorkItemPriority::High, Windows::System::Threading::WorkItemOptions::TimeSliced);
	}

    void CelestiaRenderer::Stop()
    {
        if (mRenderLoopWorker)
        {
            mRenderLoopWorker.Cancel();
            mRenderLoopWorker = nullptr;
        }
    }

    inline void CelestiaRenderer::Lock()
    {
        EnterCriticalSection(&msgCritSection);
    }

    inline void CelestiaRenderer::Unlock()
    {
        LeaveCriticalSection(&msgCritSection);
    }

    void CelestiaRenderer::Pause()
    {
        Lock();
        suspendedFlag = true;
        Unlock();
    }

    void CelestiaRenderer::Resume()
    {
        Lock();
        suspendedFlag = false;
        WakeAllConditionVariable(&resumeCond);
        Unlock();
    }

    inline void CelestiaRenderer::Wait()
    {
        while (suspendedFlag)
        {
            SleepConditionVariableCS(&resumeCond, &msgCritSection, INFINITE);
        }
    }

    void CelestiaRenderer::SetSurface(Windows::UI::Xaml::Controls::SwapChainPanel const& surface, float scale)
    {
        Lock();
        msg = CelestiaRenderer::MSG_WINDOW_SET;
        window = surface;
        windowScale = scale;
        Unlock();
    }

    void CelestiaRenderer::SetSize(int32_t width, int32_t height)
    {
        Lock();
        windowWidth = width;
        windowHeight = height;
        Unlock();
    }

    void CelestiaRenderer::SetCorePointer(int64_t core)
    {
        Lock();
        this->core = reinterpret_cast<CelestiaCore*>(core);
        Unlock();
    }

    void CelestiaRenderer::EnqueueTask(CelestiaComponent::CelestiaRendererTask const& task)
    {
        Lock();
        tasks.push_back(task);
        Unlock();
    }

    void CelestiaRenderer::FlushTasks()
    {
        for (auto task : tasks)
            task();
        tasks.clear();
    }
}

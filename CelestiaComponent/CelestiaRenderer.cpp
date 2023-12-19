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

#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <microsoft.ui.xaml.media.dxinterop.h>
#include <winrt/Microsoft.UI.Dispatching.h>

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
        if (dxgiFactory == nullptr)
            if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.put()))))
                return false;

        if (device == nullptr || deviceContext == nullptr)
        {
            D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
            D3D_FEATURE_LEVEL supportedFeatureLevel;
            UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
            creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, device.put(), &supportedFeatureLevel, deviceContext.put())))
                return false;
        }

        if (swapChain == nullptr)
        {
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
            ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
            swapChainDesc.Width = std::max(currentWindowWidthScaled, 1);
            swapChainDesc.Height = std::max(currentWindowHeightScaled, 1);
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;
            swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapChainDesc.Flags = {};

            if (FAILED(dxgiFactory->CreateSwapChainForComposition(device.get(), &swapChainDesc, nullptr, swapChain.put())))
                return false;

            // Set swapchain on main thread
            HANDLE waitEvent = CreateEventExW(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
            if (waitEvent == nullptr)
                return false;

            auto panelNative = window.as<ISwapChainPanelNative>();
            auto currentSwapChain = swapChain;
            HRESULT setSwapChainResult = E_FAIL;
            window.DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [panelNative, currentSwapChain, waitEvent, &setSwapChainResult]
                {
                    setSwapChainResult = panelNative->SetSwapChain(currentSwapChain.get());
                    SetEvent(waitEvent);
                });

            auto waitResult = WaitForSingleObjectEx(waitEvent, 10 * 1000, true);
            CloseHandle(waitEvent);

            if (waitResult != WAIT_OBJECT_0 || FAILED(setSwapChainResult))
                return false;
        }

        if (glContext == nullptr)
        {
            glWindow = CreateWindowExW(0, L"STATIC", L"Temp", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);
            if (glWindow == nullptr)
                return false;

            glDC = GetDC(glWindow);
            if (glDC == nullptr)
                return false;

            PIXELFORMATDESCRIPTOR pfd;
            ZeroMemory(&pfd, sizeof(pfd));
            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_SUPPORT_OPENGL;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.iLayerType = PFD_MAIN_PLANE;
            auto format = ChoosePixelFormat(glDC, &pfd);
            DescribePixelFormat(glDC, format, sizeof(pfd), &pfd);
            SetPixelFormat(glDC, format, &pfd);
            HGLRC tempGLContext = wglCreateContext(glDC);
            wglMakeCurrent(glDC, tempGLContext);

            int attrib[] =
            {
                WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                0
            };

            auto newGLContext = wglCreateContextAttribsARB(glDC, nullptr, attrib);
            if (newGLContext != nullptr)
                wglMakeCurrent(glDC, newGLContext);
            wglDeleteContext(tempGLContext);
            glContext = newGLContext;

            if (glContext == nullptr)
                return false;
        }

        if (glDeviceHandle == nullptr)
        {
            auto handle = wglDXOpenDeviceNV(device.get());
            if (handle == nullptr)
                return false;

            glDeviceHandle = handle;
        }

        if (frameBuffer == 0)
            glGenFramebuffers(1, &frameBuffer);

        return true;
    }

    void CelestiaRenderer::Destroy()
    {
        if (interopColorHandle != nullptr)
        {
            wglDXUnregisterObjectNV(glDeviceHandle, interopColorHandle);
            interopColorHandle = nullptr;
        }

        if (glDeviceHandle != nullptr)
        {
            wglDXCloseDeviceNV(glDeviceHandle);
            glDeviceHandle = nullptr;
        }

        if (sampleColorBuffer != 0)
        {
            glDeleteRenderbuffers(1, &sampleColorBuffer);
            sampleColorBuffer = 0;
        }

        if (sampleDepthBuffer != 0)
        {
            glDeleteRenderbuffers(1, &sampleDepthBuffer);
            sampleDepthBuffer = 0;
        }

        if (sampleFrameBuffer != 0)
        {
            glDeleteFramebuffers(1, &sampleFrameBuffer);
            sampleFrameBuffer = 0;
        }

        if (glTexture != 0)
        {
            glDeleteTextures(1, &glTexture);
            glTexture = 0;
        }

        if (frameBuffer != 0)
        {
            glDeleteFramebuffers(1, &frameBuffer);
            frameBuffer = 0;
        }

        if (glContext != nullptr)
        {
            wglDeleteContext(glContext);
            glContext = nullptr;
        }

        if (glDC != nullptr)
        {
            ReleaseDC(glWindow, glDC);
            glDC = nullptr;
        }

        if (glWindow != nullptr)
        {
            DestroyWindow(glWindow);
            glWindow = nullptr;
        }

        swapChain = nullptr;

        device = nullptr;
        deviceContext = nullptr;
        dxgiFactory = nullptr;

        currentWindowWidth = 0;
        currentWindowHeight = 0;
        currentWindowWidthScaled = 0;
        currentWindowHeightScaled = 0;
        newWindowWidth = 0;
        newWindowHeight = 0;
    }

    inline bool CelestiaRenderer::ResizeIfNeeded()
    {
        if (frameBuffer == 0 || (currentWindowHeight != newWindowHeight || currentWindowWidth != newWindowWidth))
        {
            currentWindowWidth = newWindowWidth;
            currentWindowHeight = newWindowHeight;

            currentWindowWidthScaled = static_cast<int>(newWindowWidth * windowScale);
            currentWindowHeightScaled = static_cast<int>(newWindowHeight * windowScale);

            deviceContext->Flush();

            if (interopColorHandle != nullptr)
                wglDXUnregisterObjectNV(glDeviceHandle, interopColorHandle);

            if (glTexture != 0)
                glDeleteTextures(1, &glTexture);

            swapChain->ResizeBuffers(2, static_cast<UINT>(currentWindowWidthScaled), static_cast<UINT>(currentWindowHeightScaled), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

            winrt::com_ptr<ID3D11Texture2D> backBuffer = nullptr;
            if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
                return false;

            D3D11_VIEWPORT viewport = {};
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<float>(currentWindowWidthScaled);
            viewport.Height = static_cast<float>(currentWindowHeightScaled);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            deviceContext->RSSetViewports(1, &viewport);

            glGenTextures(1, &glTexture);
            interopColorHandle = wglDXRegisterObjectNV(glDeviceHandle, backBuffer.get(), glTexture, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTexture, 0);

            if (sampleFrameBuffer == 0)
                glGenFramebuffers(1, &sampleFrameBuffer);

            glBindFramebuffer(GL_FRAMEBUFFER, sampleFrameBuffer);
            if (sampleColorBuffer == 0)
                glGenRenderbuffers(1, &sampleColorBuffer);
            if (sampleDepthBuffer == 0)
                glGenRenderbuffers(1, &sampleDepthBuffer);

            if (enableMSAA)
            {
                GLint samples;
                glGetIntegerv(GL_MAX_SAMPLES, &samples);

                glBindRenderbuffer(GL_RENDERBUFFER, sampleColorBuffer);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, currentWindowWidthScaled, currentWindowHeightScaled);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sampleColorBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, sampleDepthBuffer);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, currentWindowWidthScaled, currentWindowHeightScaled);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sampleDepthBuffer);
            }
            else
            {
                glBindRenderbuffer(GL_RENDERBUFFER, sampleColorBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, currentWindowWidthScaled, currentWindowHeightScaled);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sampleColorBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, sampleDepthBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, currentWindowWidthScaled, currentWindowHeightScaled);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sampleDepthBuffer);
            }

            core->resize(static_cast<GLsizei>(currentWindowWidthScaled), static_cast<GLsizei>(currentWindowHeightScaled));

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        return true;
    }

    bool CelestiaRenderer::Prepare()
    {
        return true;
    }

    inline void CelestiaRenderer::TickAndDraw()
    {
        wglDXLockObjectsNV(glDeviceHandle, 1, &interopColorHandle);

        glBindFramebuffer(GL_FRAMEBUFFER, sampleFrameBuffer);

        core->tick();
        core->draw();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, sampleFrameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glBlitFramebuffer(0, 0, currentWindowWidthScaled, currentWindowHeightScaled, 0, currentWindowHeightScaled, currentWindowWidthScaled, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        wglDXUnlockObjectsNV(glDeviceHandle, 1, &interopColorHandle);
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
                while (action.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
                {
                    if (swapChain != nullptr && !engineStartedCalled)
                    {
                        if (!Prepare())
                            break;
                        bool started = engineStarted();
                        if (!started)
                            break;
                        engineStartedCalled = true;
                    }

                    Lock();
                    Wait();

                    bool shouldQuit = false;
                    switch (msg)
                    {
                    case CelestiaRenderer::MSG_WINDOW_SET:
                        shouldQuit = !Initialize();
                        break;
                    default:
                        break;
                    }
                    msg = CelestiaRenderer::MSG_NONE;

                    if (shouldQuit)
                        break;

                    bool needsDrawn = false;
                    if (engineStartedCalled && swapChain != nullptr && core != nullptr)
                        needsDrawn = true;
                    auto [tasks, prerenderTask] = RetrieveAndResetTasks();

                    Unlock();

                    for (const auto& task : tasks)
                        task();
                    if (preRenderTask != nullptr)
                        prerenderTask();

                    if (needsDrawn)
                    {
                        if (!ResizeIfNeeded())
                            break;
                        TickAndDraw();

                        swapChain->Present(1, 0);
                    }
                }
                Destroy();
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
        WakeConditionVariable(&resumeCond);
        Unlock();
    }

    inline void CelestiaRenderer::Wait()
    {
        while (suspendedFlag)
        {
            SleepConditionVariableCS(&resumeCond, &msgCritSection, INFINITE);
        }
    }

    void CelestiaRenderer::SetSurface(Microsoft::UI::Xaml::Controls::SwapChainPanel const& surface, float scale)
    {
        Lock();
        msg = CelestiaRenderer::MSG_WINDOW_SET;
        window = surface;
        windowScale = scale;
        newWindowWidth = surface.ActualWidth();
        newWindowHeight = surface.ActualHeight();
        Unlock();

        window.SizeChanged([weak_this{ get_weak() }](IInspectable const& sender, Microsoft::UI::Xaml::SizeChangedEventArgs const&)
            {
                if (auto strong_this = weak_this.get())
                {
                    strong_this->Lock();
                    auto panel = sender.as<Microsoft::UI::Xaml::Controls::SwapChainPanel>();
                    strong_this->newWindowWidth = panel.ActualWidth();
                    strong_this->newWindowHeight = panel.ActualHeight();
                    strong_this->Unlock();
                }
            });
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

    void CelestiaRenderer::SetPreRenderTask(CelestiaComponent::CelestiaRendererTask const& task)
    {
        Lock();
        preRenderTask = task;
        Unlock();
    }

    std::pair<std::vector<CelestiaComponent::CelestiaRendererTask>, CelestiaComponent::CelestiaRendererTask> CelestiaRenderer::RetrieveAndResetTasks()
    {
        if (!engineStartedCalled)
            return { {}, nullptr };
        auto tasksCopy = tasks;
        tasks.clear();
        return {tasksCopy, preRenderTask};
    }

    int32_t CelestiaRenderer::StartReadingBackBuffer()
    {
        GLint oldFboId = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &oldFboId);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        return static_cast<int32_t>(oldFboId);
    }

    void CelestiaRenderer::EndReadingBackBuffer(int32_t oldBuffer)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(oldBuffer));
    }
}

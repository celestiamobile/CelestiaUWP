using System;
using System.Diagnostics;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using GLUWP.ES20;

namespace GLUWP
{
	public delegate bool PrepareHandler(object sender);
	public delegate void DrawHandler(object sender);
	public delegate void ResizeHandler(object sender, int width, int height);

	public class GLView : SwapChainPanel
	{
		public event PrepareHandler Prepare;
		public event DrawHandler Draw;
		public event ResizeHandler Resize;

		private OpenGLES mOpenGLES;

		private EGLSurface mRenderSurface;
		private object mRenderSurfaceCriticalSection = new object();

		private IAsyncAction mRenderLoopWorker;

		private bool mPrepareFailed;
		private bool mIntialized;

		public GLView()
		{
			mPrepareFailed = false;
			mIntialized = false;
			mOpenGLES = new OpenGLES();
			mRenderSurface = EGL.NO_SURFACE;
			Window.Current.VisibilityChanged += (win, args) => OnVisibilityChanged(win, args);
			Loaded += (sender, args) => OnLoaded(sender, args);
		}

		~GLView()
		{
			StopRenderLoop();
			DestroyRenderSurface();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			CreateRenderSurface();
			StartRenderLoop();
		}

		private void OnVisibilityChanged(object sender, Windows.UI.Core.VisibilityChangedEventArgs args)
		{
			if (args.Visible && mRenderSurface != EGL.NO_SURFACE)
			{
				StartRenderLoop();
			}
			else
			{
				StopRenderLoop();
			}
		}

		private void CreateRenderSurface()
		{
			if (mOpenGLES != null && mRenderSurface == EGL.NO_SURFACE)
			{
				// The app can configure the the SwapChainPanel which may boost performance.
				// By default, this template uses the default configuration.
				mRenderSurface = mOpenGLES.CreateSurface(this, null, null);

				// You can configure the SwapChainPanel to render at a lower resolution and be scaled up to
				// the swapchain panel size. This scaling is often free on mobile hardware.
				//
				// One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
				// Size customRenderSurfaceSize = new Size(800, 600);
				// mRenderSurface = mOpenGLES.CreateSurface(this, customRenderSurfaceSize, null);
				//
				// Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
				// e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
				// float customResolutionScale = 0.5f;
				// mRenderSurface = mOpenGLES->CreateSurface(swapChainPanel, nullptr, &customResolutionScale);
				// 
			}
		}

		private void DestroyRenderSurface()
		{
			if (mOpenGLES != null)
			{
				mOpenGLES.DestroySurface(mRenderSurface);
			}

			mRenderSurface = EGL.NO_SURFACE;
		}

		private void RecoverFromLostDevice()
		{
			// Stop the render loop, reset OpenGLES, recreate the render surface
			// and start the render loop again to recover from a lost device.

			StopRenderLoop();

			{
				lock (mRenderSurfaceCriticalSection)
				{
					DestroyRenderSurface();
					mOpenGLES.Reset();
					CreateRenderSurface();
				}
			}

			StartRenderLoop();
		}

		void StartRenderLoop()
		{
			// If the render loop is already running then do not start another thread.
			if (mRenderLoopWorker != null && mRenderLoopWorker.Status == Windows.Foundation.AsyncStatus.Started)
			{
				return;
			}

			// Create a task for rendering that will be run on a background thread.
			var workItemHandler = new Windows.System.Threading.WorkItemHandler(action =>
			{
				lock (mRenderSurfaceCriticalSection)
				{
					mOpenGLES.MakeCurrent(mRenderSurface);

					// ANGLE seems to take a few frames to get itself up and running. This manifests as
					// calls to glGenTextures returning 0 as a texture name. I don't know why this is,
					// but we wait until glGenTextures gives us a nonzero texture name before declaring
					// ANGLE initialized, and only then do we continue onto the main draw loop, meaning
					// that the user can remain blissfully unaware of this wrinkle.
					while (action.Status == AsyncStatus.Started)
					{
						var tex = GL.GenTexture();
						if (tex != 0)
						{
							GL.DeleteTexture(tex);
							break;
						}
					}

					if (mPrepareFailed)
						return;

					if (!mIntialized)
                    {
						if (!Prepare(this))
                        {
							mPrepareFailed = true;
							return;
                        }
						mIntialized = true;
                    }

					int panelWidth = 0;
					int panelHeight = 0;
					while (action.Status == AsyncStatus.Started)
					{
						int newWidth = 0;
						int newHeight = 0;
						mOpenGLES.GetSurfaceDimensions(mRenderSurface, ref newWidth, ref newHeight);
						if (newWidth != panelWidth || newHeight != panelHeight)
						{
							panelWidth = newWidth;
							panelHeight = newHeight;
							Resize(this, panelWidth, panelHeight);
						}

						Draw(this);

						// The call to eglSwapBuffers might not be successful (i.e. due to Device Lost)
						// If the call fails, then we must reinitialize EGL and the GL resources.
						if (mOpenGLES.SwapBuffers(mRenderSurface) != EGL.TRUE)
						{
							// XAML objects like the SwapChainPanel must only be manipulated on the UI thread.
							_ = Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.High,
								new Windows.UI.Core.DispatchedHandler(() =>
								{
									RecoverFromLostDevice();
								}));

							return;
						}
					}
				}
			});

			// Run task on a dedicated high priority background thread.
			mRenderLoopWorker = Windows.System.Threading.ThreadPool.RunAsync(workItemHandler,
				Windows.System.Threading.WorkItemPriority.High,
				Windows.System.Threading.WorkItemOptions.TimeSliced);
		}

		void StopRenderLoop()
		{
			if (mRenderLoopWorker != null)
			{
				mRenderLoopWorker.Cancel();
				mRenderLoopWorker = null;
			}
		}
	}
}
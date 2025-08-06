using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.IO;
using System.Threading;

namespace CelestiaWinUITests
{
    [TestClass]
    public class UnitTest1
    {
        private WindowsDriver<WindowsElement> CreateDefaultSession()
        {
            AppiumOptions options = new AppiumOptions();
            options.AddAdditionalCapability("app", "40507LinfengLi.Celestia_r7f2qt5a05s84!App");
            options.AddAdditionalCapability("automationName", "Windows");
            var session = new WindowsDriver<WindowsElement>(new Uri("http://127.0.0.1:4723"), options);
            session.Manage().Window.Maximize();
            return session;
        }

        private WindowsDriver<WindowsElement> CreateRootSession()
        {
            AppiumOptions options = new AppiumOptions();
            options.AddAdditionalCapability("app", "Root");
            options.AddAdditionalCapability("automationName", "Windows");
            var session = new WindowsDriver<WindowsElement>(new Uri("http://127.0.0.1:4723"), options);
            return session;
        }

        private void SaveScreenshot(string filename, WindowsDriver<WindowsElement> session)
        {
            var path = "C:\\Users\\lilin\\source\\repos\\CelestiaUWP";
            var screenshotDir = string.Format("{0}\\{1}", path, "Screenshots");
            if (!Directory.Exists(screenshotDir))
                Directory.CreateDirectory(screenshotDir);
            session.GetScreenshot().SaveAsFile(string.Format("{0}\\{1}.png", screenshotDir, filename));
        }

        [TestMethod]
        public void TestMethod1()
        {
            var root = CreateRootSession();
            var testCount = 6;
            for (var i = 0; i < testCount; i++)
            {
                TestItem(i, string.Format("URL{0}", i), root);
            }
        }

        private void TestItem(int testIndex, string filename, WindowsDriver<WindowsElement> root)
        {
            var session = CreateDefaultSession();

            Thread.Sleep(TimeSpan.FromSeconds(15));

            session.FindElementByName("File").Click();
            Thread.Sleep(TimeSpan.FromSeconds(1));
            session.FindElementByName(string.Format("Test {0}", testIndex)).Click();
            Thread.Sleep(TimeSpan.FromSeconds(10));

            SaveScreenshot(filename, root);
            session.Quit();

            Thread.Sleep(TimeSpan.FromSeconds(20));
        }
    }
}

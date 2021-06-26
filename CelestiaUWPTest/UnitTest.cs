
using System;
using System.IO;
using System.Threading;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;

namespace CelestiaUWPTest
{
    [TestClass]
    public class UnitTest1
    {
        private TestContext testContextInstance;

        public TestContext TestContext
        {
            get { return testContextInstance; }
            set { testContextInstance = value; }
        }

        private WindowsDriver CreateDefaultSession()
        {
            AppiumOptions options = new AppiumOptions();
            options.App = "40507LinfengLi.Celestia_r7f2qt5a05s84!App";
            options.AutomationName = "Windows";
            WindowsDriver session = new WindowsDriver(new Uri("http://127.0.0.1:4723"), options);
            session.Manage().Window.Maximize();
            return session;
        }

        [TestMethod]
        public void TestGetScreenshotForTestInfos()
        {
            var testCount = 6;
            for (var i = 0; i < testCount; i++)
            {
                TestItem(i, string.Format("URL{0}", i));
            }
        }

        public void TestItem(int testIndex, string filename)
        {
            var session = CreateDefaultSession();
            Thread.Sleep(TimeSpan.FromSeconds(20));

            session.FindElement(by: "accessibility id", value: "File").Click();
            Thread.Sleep(TimeSpan.FromSeconds(1));
            session.FindElement(by: "accessibility id", value: string.Format("Test {0}", testIndex)).Click();
            Thread.Sleep(TimeSpan.FromSeconds(20));

            SaveScreenshot(filename, session);
            session.Quit();
        }

        public void SaveScreenshot(string filename, WindowsDriver session)
        {
            var path = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            var screenshotDir = string.Format("{0}\\{1}", path, "Screenshots");
            if (!Directory.Exists(screenshotDir))
                Directory.CreateDirectory(screenshotDir);
            session.GetScreenshot().SaveAsFile(string.Format("{0}\\{1}.png", screenshotDir, filename));
            TestContext.WriteLine(string.Format("Saved to Screenshot Directory: {0}", screenshotDir));
        }
    }
}

//
// GamepadManager.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using System.Collections.Generic;
using Windows.Gaming.Input;

namespace CelestiaUWP.Helper
{
    public class GamepadManager
    {
        private AppSettings AppSettings;
        private List<Gamepad> _controllers = new List<Gamepad>();
        private Gamepad _controller;
        private readonly object myLock = new object();
        private bool _isAPressed = false;
        private bool _isBPressed = false;
        private bool _isXPressed = false;
        private bool _isYPressed = false;
        private bool _isLeftTriggerPressed = false;
        private bool _isRightTriggerPressed = false;
        private bool _isLeftBumperPressed = false;
        private bool _isRightBumperPressed = false;
        private bool _isDpadLeftPressed = false;
        private bool _isDpadRightPressed = false;
        private bool _isDpadUpPressed = false;
        private bool _isDpadDownPressed = false;

        public GamepadManager(AppSettings appSettings)
        {
            AppSettings = appSettings;
            foreach (var gamepad in Gamepad.Gamepads)
                _controllers.Add(gamepad);
            Gamepad.GamepadAdded += Gamepad_GamepadAdded;
            Gamepad.GamepadRemoved += Gamepad_GamepadRemoved;
        }

        public void PollGamepad(CelestiaAppCore core)
        {
            UpdateGamepad();
            var controller = _controller;
            if (controller != null)
            {
                var reading = controller.GetCurrentReading();
                bool isAPressed = (reading.Buttons & GamepadButtons.A) != 0;
                bool isBPressed = (reading.Buttons & GamepadButtons.B) != 0;
                bool isXPressed = (reading.Buttons & GamepadButtons.X) != 0;
                bool isYPressed = (reading.Buttons & GamepadButtons.Y) != 0;
                bool isLeftTriggerPressed = reading.LeftTrigger >= 0.5;
                bool isRightTriggerPressed = reading.RightTrigger >= 0.5;
                bool isLeftBumperPressed = (reading.Buttons & GamepadButtons.LeftShoulder) != 0;
                bool isRightBumperPressed = (reading.Buttons & GamepadButtons.RightShoulder) != 0;
                bool isDpadLeftPressed = (reading.Buttons & GamepadButtons.DPadLeft) != 0;
                bool isDpadRightPressed = (reading.Buttons & GamepadButtons.DPadRight) != 0;
                bool isDpadUpPressed = (reading.Buttons & GamepadButtons.DPadUp) != 0;
                bool isDpadDownPressed = (reading.Buttons & GamepadButtons.DPadDown) != 0;
                double thumbstickX = reading.LeftThumbstickX + reading.RightThumbstickX;
                double thumbstickY = reading.LeftThumbstickY + reading.RightThumbstickY;
                if (isAPressed != _isAPressed)
                {
                    _isAPressed = isAPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapA, isAPressed);
                }
                if (isBPressed != _isBPressed)
                {
                    _isBPressed = isBPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapB, isBPressed);
                }
                if (isXPressed != _isXPressed)
                {
                    _isXPressed = isXPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapX, isXPressed);
                }
                if (isYPressed != _isYPressed)
                {
                    _isYPressed = isYPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapY, isYPressed);
                }
                if (isLeftTriggerPressed != _isLeftTriggerPressed)
                {
                    _isLeftTriggerPressed = isLeftTriggerPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapLT, isLeftTriggerPressed);
                }
                if (isRightTriggerPressed != _isRightTriggerPressed)
                {
                    _isRightTriggerPressed = isRightTriggerPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapRT, isRightTriggerPressed);
                }
                if (isLeftBumperPressed != _isLeftBumperPressed)
                {
                    _isLeftBumperPressed = isLeftBumperPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapLB, isLeftBumperPressed);
                }
                if (isRightBumperPressed != _isRightBumperPressed)
                {
                    _isRightBumperPressed = isRightBumperPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapRB, isRightBumperPressed);
                }
                if (isDpadLeftPressed != _isDpadLeftPressed)
                {
                    _isDpadLeftPressed = isDpadLeftPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapDpadLeft, isDpadLeftPressed);
                }
                if (isDpadRightPressed != _isDpadRightPressed)
                {
                    _isDpadRightPressed = isDpadRightPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapDpadRight, isDpadRightPressed);
                }
                if (isDpadUpPressed != _isDpadUpPressed)
                {
                    _isDpadUpPressed = isDpadUpPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapDpadUp, isDpadUpPressed);
                }
                if (isDpadDownPressed != _isDpadDownPressed)
                {
                    _isDpadDownPressed = isDpadDownPressed;
                    GamepadButtonAction(core, AppSettings.GamepadRemapDpadDown, isDpadDownPressed);
                }
                if (AppSettings.GamepadInvertX)
                    thumbstickX = -thumbstickX;
                if (AppSettings.GamepadInvertY)
                    thumbstickY = -thumbstickY;
                if (thumbstickX != 0.0)
                {
                    GamepadJoystickAction(core, CelestiaJoystickAxis.X, thumbstickX);
                }
                if (thumbstickY != 0.0)
                {
                    GamepadJoystickAction(core, CelestiaJoystickAxis.Y, thumbstickY);
                }
            }
        }

        public void GamepadButtonAction(CelestiaAppCore core, CelestiaGamepadAction action, bool pressed)
        {
            CelestiaExtension.InvokeCelestiaGamepadAction(core, action, !pressed);
        }

        public void GamepadJoystickAction(CelestiaAppCore core, CelestiaJoystickAxis axis, double amount)
        {
            core.JoystickAxis(axis, (float)amount);
        }

        private void UpdateGamepad()
        {
            // Update the current gamepad if necessary.
            if (_controller == null)
            {
                _controller = GetFirstGamepad();
            }
        }

        private Gamepad GetFirstGamepad()
        {
            lock(myLock)
            {
                Gamepad gamepad = null;
                if (_controllers.Count > 0)
                {
                    gamepad = _controllers[0];
                }
                return gamepad;
            }
        }

        private void Gamepad_GamepadRemoved(object sender, Gamepad e)
        {
            lock (myLock)
            {
                int indexRemoved = _controllers.IndexOf(e);
                if (indexRemoved > -1)
                {
                    if (_controller == e)
                        _controller = null;

                    _controllers.RemoveAt(indexRemoved);
                }
            }
        }
        private void Gamepad_GamepadAdded(object sender, Gamepad e)
        {
            lock(myLock)
            {
                if (!_controllers.Contains(e))
                    _controllers.Add(e);
            }
        }
    }
}

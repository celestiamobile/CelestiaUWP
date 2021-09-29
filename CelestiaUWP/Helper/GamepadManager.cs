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
        private List<Gamepad> _controllers = new List<Gamepad>();
        private Gamepad _controller;
        private readonly object myLock = new object();
        private bool _isAPressed = false;
        private bool _isXPressed = false;
        private bool _isLeftTriggerPressed = false;
        private bool _isRightTriggerPressed = false;

        public GamepadManager()
        {
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
                bool isXPressed = (reading.Buttons & GamepadButtons.X) != 0;
                bool isLeftTriggerPressed = reading.LeftTrigger >= 0.5;
                bool isRightTriggerPressed = reading.RightTrigger >= 0.5;
                double thumbstickX = reading.LeftThumbstickX + reading.RightThumbstickX;
                double thumbstickY = reading.LeftThumbstickY + reading.RightThumbstickY;
                if (isAPressed != _isAPressed)
                {
                    _isAPressed = isAPressed;
                    GamepadButtonAction(core, CelestiaJoystickButton.Button1, isAPressed);
                }
                if (isXPressed != _isXPressed)
                {
                    _isXPressed = isXPressed;
                    GamepadButtonAction(core, CelestiaJoystickButton.Button2, isBPressed);
                }
                if (isLeftTriggerPressed != _isLeftTriggerPressed)
                {
                    _isLeftTriggerPressed = isLeftTriggerPressed;
                    GamepadButtonAction(core, CelestiaJoystickButton.Button7, isLeftTriggerPressed);
                }
                if (isRightTriggerPressed != _isRightTriggerPressed)
                {
                    _isRightTriggerPressed = isRightTriggerPressed;
                    GamepadButtonAction(core, CelestiaJoystickButton.Button8, isRightTriggerPressed);
                }
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

        public void GamepadButtonAction(CelestiaAppCore core, CelestiaJoystickButton button, bool pressed)
        {
            if (pressed)
                core.JoystickButtonDown(button);
            else
                core.JoystickButtonUp(button);
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

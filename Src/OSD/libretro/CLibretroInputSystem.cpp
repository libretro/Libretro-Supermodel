#include <cctype>
#include <cstring>
#include <libretro.h>
#include "CLibretroInputSystem.h"
#include "CoreOptionsTypes.h"
#include <Inputs/Input.h>
#include <cmath>
#include <algorithm>

extern retro_input_poll_t input_poll_cb;
extern retro_input_state_t input_state_cb;
extern retro_log_printf_t log_cb;  // defined in libretro.cpp

CLibretroInputSystem::CLibretroInputSystem()
    : CInputSystem("Libretro")
{
    memset(m_joyButtons, 0, sizeof(m_joyButtons));
    memset(m_joyAxes,    0, sizeof(m_joyAxes));
    memset(m_joyPOV,     0, sizeof(m_joyPOV));
    memset(m_keyState,   0, sizeof(m_keyState));
    memset(m_mouseAxes,  0, sizeof(m_mouseAxes));
    memset(m_mouseButtons, 0, sizeof(m_mouseButtons));
    memset(m_mouseWheelDir, 0, sizeof(m_mouseWheelDir));
    memset(m_mouseIsAbsolute, 0, sizeof(m_mouseIsAbsolute));
    memset(m_mouseX, 0, sizeof(m_mouseX));
    memset(m_mouseY, 0, sizeof(m_mouseY));
    memset(m_mousePositionInitialized, 0, sizeof(m_mousePositionInitialized));
    memset(m_gunCursorX, 0, sizeof(m_gunCursorX));
    memset(m_gunCursorY, 0, sizeof(m_gunCursorY));
    memset(m_previousLightgunX, 0, sizeof(m_previousLightgunX));
    memset(m_previousLightgunY, 0, sizeof(m_previousLightgunY));
    memset(m_gunCursorInitialized, 0, sizeof(m_gunCursorInitialized));
    memset(m_lightgunPositionInitialized, 0,
           sizeof(m_lightgunPositionInitialized));
    m_analogJoystickCursorX = 0;
    m_analogJoystickCursorY = 0;
    m_analogJoystickCursorInitialized = false;
    m_analogJoystickSource = 0;
    m_port2MouseDistinct = false;
    m_port2LightgunDistinct = false;
    memset(&m_rumbleInterface, 0, sizeof(m_rumbleInterface));
}

CLibretroInputSystem::~CLibretroInputSystem() {}

bool CLibretroInputSystem::Poll()
{
    if (!input_poll_cb || !input_state_cb)
        return false;

    input_poll_cb();

    // ----- Keyboard -----
    for (int k = 0; k < 512; k++)
        m_keyState[k] = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, k);

    // Ensure we have some display dimensions
    if (m_dispW == 0) { m_dispW = 496; m_dispH = 384; }

    int mouseDeltaX[2] = {};
    int mouseDeltaY[2] = {};
    int16_t lightgunX[2] = {};
    int16_t lightgunY[2] = {};
    bool lightgunOffscreen[2] = {};
    bool lightgunPositionMoved[2] = {};

    // ----- RetroMouse ports (Supermodel MOUSE and MOUSE2) -----
    for (int port = 0; port < 2; ++port)
    {
        if (!m_mousePositionInitialized[port])
        {
            m_mouseX[port] = static_cast<int>(m_dispX + m_dispW / 2);
            m_mouseY[port] = static_cast<int>(m_dispY + m_dispH / 2);
            m_mousePositionInitialized[port] = true;
        }

        mouseDeltaX[port] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
        mouseDeltaY[port] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
        m_mouseX[port] += mouseDeltaX[port];
        m_mouseY[port] += mouseDeltaY[port];

        const int min_x = static_cast<int>(m_dispX);
        const int min_y = static_cast<int>(m_dispY);
        const int max_x = min_x + static_cast<int>(m_dispW) - 1;
        const int max_y = min_y + static_cast<int>(m_dispH) - 1;
        m_mouseX[port] = std::clamp(m_mouseX[port], min_x, max_x);
        m_mouseY[port] = std::clamp(m_mouseY[port], min_y, max_y);

        m_mouseAxes[port][AXIS_X] = m_mouseX[port];
        m_mouseAxes[port][AXIS_Y] = m_mouseY[port];
        m_mouseWheelDir[port] = 0;
        if (input_state_cb(port, RETRO_DEVICE_MOUSE, 0,
                           RETRO_DEVICE_ID_MOUSE_WHEELUP))
        {
            m_mouseAxes[port][AXIS_Z] += 5;
            m_mouseWheelDir[port] = 1;
        }
        else if (input_state_cb(port, RETRO_DEVICE_MOUSE, 0,
                                RETRO_DEVICE_ID_MOUSE_WHEELDOWN))
        {
            m_mouseAxes[port][AXIS_Z] -= 5;
            m_mouseWheelDir[port] = -1;
        }

        // CInputSystem mouse button order is Left, Middle, Right, X1, X2.
        m_mouseButtons[port][0] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
        m_mouseButtons[port][1] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
        m_mouseButtons[port][2] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
        m_mouseButtons[port][3] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_BUTTON_4);
        m_mouseButtons[port][4] = input_state_cb(
            port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_BUTTON_5);
        m_mouseIsAbsolute[port] = false;
    }

    // ----- RetroLightgun ports (Supermodel MOUSE3 and MOUSE4) -----
    for (int port = 0; port < 2; ++port)
    {
        const int dev = port + 2;
        // Use SCREEN_X/Y for absolute position (-32768 to 32767)
        lightgunX[port] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X);
        lightgunY[port] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y);
        lightgunOffscreen[port] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0,
            RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN);
        
        // Scale to display coordinates
        m_mouseAxes[dev][AXIS_X] = static_cast<int>(m_dispX) +
            static_cast<int>(((static_cast<float>(lightgunX[port]) + 32768.0f) / 65535.0f) *
                             static_cast<float>(m_dispW - 1));
        m_mouseAxes[dev][AXIS_Y] = static_cast<int>(m_dispY) +
            static_cast<int>(((static_cast<float>(lightgunY[port]) + 32768.0f) / 65535.0f) *
                             static_cast<float>(m_dispH - 1));
        m_mouseAxes[dev][AXIS_Z] = 0;
        m_mouseWheelDir[dev] = 0;

        m_mouseButtons[dev][0] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_TRIGGER);
        m_mouseButtons[dev][1] = 0;
        // Ocean Hunter has two independent shot triggers. AUX_A is the
        // natural second trigger; Reload is also accepted because RetroArch
        // binds it to the second mouse button by default. IS_OFFSCREEN is a
        // position state, not a button: treating it as one leaves Shot 2 stuck.
        m_mouseButtons[dev][2] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_AUX_A) ||
            input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_RELOAD);
        m_mouseButtons[dev][3] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_START);
        m_mouseButtons[dev][4] = input_state_cb(
            port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SELECT);
        m_mouseIsAbsolute[dev] = true;
    }

    const int16_t THRESHOLD = 8000;

    for (int joy = 0; joy < 2; joy++)
    {
        // ----- Initialize all buttons to 0 first -----
        for (int b = 0; b < NUM_JOY_BUTTONS; b++)
        {
            m_joyButtons[joy][b] = 0;
        }

        // ----- Axes -----
        int16_t x = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                   RETRO_DEVICE_INDEX_ANALOG_LEFT,
                                   RETRO_DEVICE_ID_ANALOG_X);

        int16_t y = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                   RETRO_DEVICE_INDEX_ANALOG_LEFT,
                                   RETRO_DEVICE_ID_ANALOG_Y);

        m_joyAxes[joy][AXIS_X] = x;
        m_joyAxes[joy][AXIS_Y] = y;

        // Right stick -> RX/RY, which the analog driving layout uses as a gate shifter
        // (JOY1_RYAXIS_NEG/POS = gears 1/2, JOY1_RXAXIS_NEG/POS = gears 3/4).
        m_joyAxes[joy][AXIS_RX] = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                                 RETRO_DEVICE_INDEX_ANALOG_RIGHT,
                                                 RETRO_DEVICE_ID_ANALOG_X);
        m_joyAxes[joy][AXIS_RY] = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                                 RETRO_DEVICE_INDEX_ANALOG_RIGHT,
                                                 RETRO_DEVICE_ID_ANALOG_Y);

        // Triggers as analog axes (0..32767). Frontends with only digital L2/R2 report 0 or
        // 32767 here, so this is safe either way. L2 -> ZAXIS, R2 -> RZAXIS, matching how
        // Supermodel names trigger axes elsewhere.
        m_joyAxes[joy][AXIS_Z]  = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                                 RETRO_DEVICE_INDEX_ANALOG_BUTTON,
                                                 RETRO_DEVICE_ID_JOYPAD_L2);
        m_joyAxes[joy][AXIS_RZ] = input_state_cb(joy, RETRO_DEVICE_ANALOG,
                                                 RETRO_DEVICE_INDEX_ANALOG_BUTTON,
                                                 RETRO_DEVICE_ID_JOYPAD_R2);

        // Touch overlays (and some pads) press L2/R2 only digitally and report 0 on the
        // analog channel, which would leave the pedals dead. Treat that as full travel.
        if (m_joyAxes[joy][AXIS_Z] == 0 &&
            input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2))
            m_joyAxes[joy][AXIS_Z] = 0x7FFF;
        if (m_joyAxes[joy][AXIS_RZ] == 0 &&
            input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2))
            m_joyAxes[joy][AXIS_RZ] = 0x7FFF;

        // ----- D-Pad -----
        bool d_up    = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
        bool d_down  = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
        bool d_left  = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
        bool d_right = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

        // CInputSystem POV directions are ordered Up, Down, Left, Right.
        // Keep this array in the same order: IsJoyPOVInDir() indexes it with
        // the POV_* constants declared by Supermodel.
        m_joyPOV[joy][0] = (y < -THRESHOLD) || d_up;    // Up
        m_joyPOV[joy][1] = (y >  THRESHOLD) || d_down;  // Down
        m_joyPOV[joy][2] = (x < -THRESHOLD) || d_left;  // Left
        m_joyPOV[joy][3] = (x >  THRESHOLD) || d_right; // Right

        // ----- Buttons -----
        // Standard game buttons (0-3: B, A, Y, X)
        static const int game_buttons[4] =
        {
            RETRO_DEVICE_ID_JOYPAD_B,      // Button 0 (BUTTON1)
            RETRO_DEVICE_ID_JOYPAD_A,      // Button 1 (BUTTON2)
            RETRO_DEVICE_ID_JOYPAD_Y,      // Button 2 (BUTTON3)
            RETRO_DEVICE_ID_JOYPAD_X,      // Button 3 (BUTTON4)
        };

        for (int b = 0; b < 4; b++)
        {
            m_joyButtons[joy][b] =
                input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, game_buttons[b]);
        }

        // Gear shift / shoulder buttons (4-7: L, R, L2, R2)
        m_joyButtons[joy][4] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
        m_joyButtons[joy][5] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);
        m_joyButtons[joy][6] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
        m_joyButtons[joy][7] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);

        // Coin/Start buttons (8-9)
        // Button 8: START button -> maps to InputStart1/InputStart2
        // Button 9: SELECT button -> maps to InputCoin1/InputCoin2
        m_joyButtons[joy][8] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
        m_joyButtons[joy][9] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
        
        // **FIX: Map D-Pad AND Analog Stick to buttons 10-13**
        // This makes both the analog stick and D-Pad work for movement!
        m_joyButtons[joy][10] = (y < -THRESHOLD) || d_up;      // Up - ANALOG + DIGITAL
        m_joyButtons[joy][11] = (y >  THRESHOLD) || d_down;    // Down - ANALOG + DIGITAL
        m_joyButtons[joy][12] = (x < -THRESHOLD) || d_left;    // Left - ANALOG + DIGITAL
        m_joyButtons[joy][13] = (x >  THRESHOLD) || d_right;   // Right - ANALOG + DIGITAL

        // Service/Test buttons (20-23) - BUTTON21-24 in Supermodel numbering
        // Service/Test buttons (20-23) - mapping depends on user preference
        if (m_serviceOnSticks)
        {
            m_joyButtons[joy][20] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3);
            m_joyButtons[joy][21] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3);
            m_joyButtons[joy][22] = 0; // no 3rd/4th stick button on standard pads
            m_joyButtons[joy][23] = 0;
        }
        else
        {
            m_joyButtons[joy][20] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
            m_joyButtons[joy][21] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);
            m_joyButtons[joy][22] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
            m_joyButtons[joy][23] = input_state_cb(joy, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);
        }
    }

    // ----- Virtual gun cursors (Supermodel MOUSE5 and MOUSE6) -----
    // MOUSE5/MOUSE6 combine the sources selected by Gun Input Mode. Dedicated
    // Mouse mode consumes only RetroMouse relative movement and buttons;
    // existing Lightgun and Hybrid behavior retains its raw-mouse fallback.
    const int minX = static_cast<int>(m_dispX);
    const int minY = static_cast<int>(m_dispY);
    const int maxX = minX + static_cast<int>(m_dispW) - 1;
    const int maxY = minY + static_cast<int>(m_dispH) - 1;
    const auto stickDelta = [](int16_t axis) -> int
    {
        constexpr int deadZone = 6000;
        constexpr int maxSpeed = 12;
        const int magnitude = std::abs(static_cast<int>(axis));
        if (magnitude <= deadZone)
            return 0;
        const float normalized = static_cast<float>(magnitude - deadZone) /
                                 static_cast<float>(32767 - deadZone);
        const int speed = 1 + static_cast<int>(std::lround(
            normalized * static_cast<float>(maxSpeed - 1)));
        return axis < 0 ? -speed : speed;
    };

    // Many frontend input drivers (RetroArch on Wayland and X11, for example) have a
    // single system pointer and report that same mouse and light gun on every port.
    // Player 2 would then mirror player 1's aim and shots. Port 2's mouse and light
    // gun are therefore ignored until they have reported something port 1 did not:
    // a different movement, position or button. Drivers with real per-port devices
    // (udev, raw input) unlock on the first such difference, for the rest of the session.
    if (!m_port2MouseDistinct)
    {
        const bool moved = mouseDeltaX[1] != 0 || mouseDeltaY[1] != 0;
        const bool movedDifferently = mouseDeltaX[1] != mouseDeltaX[0] ||
                                      mouseDeltaY[1] != mouseDeltaY[0];
        const bool buttonDiffers = (m_mouseButtons[1][0] && !m_mouseButtons[0][0]) ||
                                   (m_mouseButtons[1][2] && !m_mouseButtons[0][2]);
        if ((moved && movedDifferently) || buttonDiffers)
            m_port2MouseDistinct = true;
    }
    if (!m_port2LightgunDistinct)
    {
        const bool valid = !lightgunOffscreen[1] &&
            lightgunX[1] != INT16_MIN && lightgunY[1] != INT16_MIN;
        const bool positionDiffers = lightgunX[1] != lightgunX[0] ||
                                     lightgunY[1] != lightgunY[0];
        const bool triggerDiffers = m_mouseButtons[3][0] && !m_mouseButtons[2][0];
        if ((valid && positionDiffers) || triggerDiffers)
            m_port2LightgunDistinct = true;
    }

    for (int port = 0; port < 2; ++port)
    {
        const int lightgunDev = port + 2;
        const int cursorDev = port + 4;
        const bool lightgunUsable = port == 0 || m_port2LightgunDistinct;
        const bool mouseUsable = port == 0 || m_port2MouseDistinct;
        const bool allowLightgun = lightgunUsable &&
            (g_options.gun_input == GunInput::Hybrid ||
             g_options.gun_input == GunInput::Lightgun);
        const bool allowMouse = mouseUsable &&
            (g_options.gun_input == GunInput::Hybrid ||
             g_options.gun_input == GunInput::Lightgun ||
             g_options.gun_input == GunInput::Mouse);
        const bool allowAnalog =
            g_options.gun_input == GunInput::Hybrid ||
            g_options.gun_input == GunInput::AnalogSticks;
        if (!m_gunCursorInitialized[port])
        {
            m_gunCursorX[port] = minX + static_cast<int>(m_dispW) / 2;
            m_gunCursorY[port] = minY + static_cast<int>(m_dispH) / 2;
            m_gunCursorInitialized[port] = true;
        }

        const bool lightgunValid = allowLightgun &&
            !lightgunOffscreen[port] &&
            lightgunX[port] != INT16_MIN && lightgunY[port] != INT16_MIN;
        const bool lightgunMoved = lightgunValid &&
            (!m_lightgunPositionInitialized[port] ||
             lightgunX[port] != m_previousLightgunX[port] ||
             lightgunY[port] != m_previousLightgunY[port]);
        lightgunPositionMoved[port] = lightgunMoved;
        const bool lightgunAction = allowLightgun && lightgunValid &&
            (m_mouseButtons[lightgunDev][0] ||
             m_mouseButtons[lightgunDev][2] ||
             m_mouseButtons[lightgunDev][3] ||
             m_mouseButtons[lightgunDev][4]);
        const bool rawMouseMoved = allowMouse &&
            (mouseDeltaX[port] != 0 || mouseDeltaY[port] != 0);
        const int stickX = stickDelta(m_joyAxes[port][AXIS_X]);
        const int stickY = stickDelta(m_joyAxes[port][AXIS_Y]);

        if (g_options.gun_input == GunInput::Mouse)
        {
            if (rawMouseMoved)
            {
                m_gunCursorX[port] += mouseDeltaX[port];
                m_gunCursorY[port] += mouseDeltaY[port];
            }
        }
        else if (g_options.gun_input == GunInput::Lightgun)
        {
            if (lightgunValid)
            {
                m_gunCursorX[port] = m_mouseAxes[lightgunDev][AXIS_X];
                m_gunCursorY[port] = m_mouseAxes[lightgunDev][AXIS_Y];
            }
            else if (rawMouseMoved)
            {
                m_gunCursorX[port] += mouseDeltaX[port];
                m_gunCursorY[port] += mouseDeltaY[port];
            }
        }
        else if (lightgunMoved || lightgunAction)
        {
            m_gunCursorX[port] = m_mouseAxes[lightgunDev][AXIS_X];
            m_gunCursorY[port] = m_mouseAxes[lightgunDev][AXIS_Y];
        }
        else if (rawMouseMoved)
        {
            m_gunCursorX[port] += mouseDeltaX[port];
            m_gunCursorY[port] += mouseDeltaY[port];
        }
        else
        {
            if (allowAnalog)
            {
                m_gunCursorX[port] += stickX;
                m_gunCursorY[port] += stickY;
            }
        }

        m_gunCursorX[port] = std::clamp(m_gunCursorX[port], minX, maxX);
        m_gunCursorY[port] = std::clamp(m_gunCursorY[port], minY, maxY);
        m_mouseAxes[cursorDev][AXIS_X] = m_gunCursorX[port];
        m_mouseAxes[cursorDev][AXIS_Y] = m_gunCursorY[port];
        m_mouseAxes[cursorDev][AXIS_Z] = 0;
        m_mouseWheelDir[cursorDev] = 0;
        m_mouseIsAbsolute[cursorDev] = true;

        m_mouseButtons[cursorDev][0] =
            (allowLightgun && m_mouseButtons[lightgunDev][0]) ||
            (allowMouse && m_mouseButtons[port][0]) ||
            (allowAnalog && m_joyButtons[port][0]);
        m_mouseButtons[cursorDev][1] = false;
        m_mouseButtons[cursorDev][2] =
            (allowLightgun && m_mouseButtons[lightgunDev][2]) ||
            (allowMouse && m_mouseButtons[port][2]) ||
            (allowAnalog && m_joyButtons[port][1]);
        m_mouseButtons[cursorDev][3] =
            (allowLightgun && m_mouseButtons[lightgunDev][3]) ||
            (allowMouse && m_mouseButtons[port][3]);
        m_mouseButtons[cursorDev][4] =
            (allowLightgun && m_mouseButtons[lightgunDev][4]) ||
            (allowMouse && m_mouseButtons[port][4]);

        m_previousLightgunX[port] = lightgunX[port];
        m_previousLightgunY[port] = lightgunY[port];
        m_lightgunPositionInitialized[port] = true;
    }

    // ----- Star Wars Trilogy analog joystick (Supermodel MOUSE7) -----
    // Supermodel's traditional JOY1_X/Y,MOUSE_X/Y mapping is ambiguous in a
    // libretro core: the joystick is absolute, while RetroMouse supplies
    // relative deltas which we accumulate into a position. Once the mouse has
    // left centre, its retained position reappears whenever the stick returns
    // to neutral and makes the game's calibration centre unstable.
    //
    // Present one absolute virtual axis to Supermodel instead. Star Wars
    // Trilogy Input Mode selects the accepted sources and, in Hybrid mode, the last source
    // actually moved owns it. Mouse movement adjusts the current position;
    // moving the stick switches back to its absolute position, including its
    // stable centre when released. No values from the devices are mixed.
    constexpr int analogJoystickDev = 6;
    if (!m_analogJoystickCursorInitialized)
    {
        m_analogJoystickCursorX = minX + static_cast<int>(m_dispW) / 2;
        m_analogJoystickCursorY = minY + static_cast<int>(m_dispH) / 2;
        m_analogJoystickCursorInitialized = true;
    }

    const bool analogMouseMoved = mouseDeltaX[0] != 0 || mouseDeltaY[0] != 0;
    constexpr int analogSwitchDeadZone = 6000;
    const bool analogStickMoved =
        std::abs(static_cast<int>(m_joyAxes[0][AXIS_X])) > analogSwitchDeadZone ||
        std::abs(static_cast<int>(m_joyAxes[0][AXIS_Y])) > analogSwitchDeadZone;
    switch (g_options.star_wars_input)
    {
    case StarWarsInput::AnalogSticks:
        m_analogJoystickSource = 0;
        break;
    case StarWarsInput::Mouse:
        m_analogJoystickSource = 1;
        break;
    case StarWarsInput::Hybrid:
        if (analogMouseMoved)
            m_analogJoystickSource = 1;
        else if (analogStickMoved)
            m_analogJoystickSource = 0;
        break;
    }

    if (m_analogJoystickSource == 0)
    {
        const auto axisToDisplay = [](int16_t axis, int minimum,
                                      unsigned extent) -> int
        {
            if (extent <= 1)
                return minimum;
            const float normalized =
                (static_cast<float>(axis) + 32768.0f) / 65535.0f;
            return minimum + static_cast<int>(std::lround(
                normalized * static_cast<float>(extent - 1)));
        };
        m_analogJoystickCursorX = axisToDisplay(
            m_joyAxes[0][AXIS_X], minX, m_dispW);
        m_analogJoystickCursorY = axisToDisplay(
            m_joyAxes[0][AXIS_Y], minY, m_dispH);
    }
    else if (m_analogJoystickSource == 1 && analogMouseMoved)
    {
        m_analogJoystickCursorX += mouseDeltaX[0];
        m_analogJoystickCursorY += mouseDeltaY[0];
    }
    m_analogJoystickCursorX = std::clamp(
        m_analogJoystickCursorX, minX, maxX);
    m_analogJoystickCursorY = std::clamp(
        m_analogJoystickCursorY, minY, maxY);
    m_mouseAxes[analogJoystickDev][AXIS_X] = m_analogJoystickCursorX;
    m_mouseAxes[analogJoystickDev][AXIS_Y] = m_analogJoystickCursorY;
    m_mouseAxes[analogJoystickDev][AXIS_Z] = 0;
    m_mouseWheelDir[analogJoystickDev] = 0;
    m_mouseIsAbsolute[analogJoystickDev] = true;
    const bool analogAllowMouse =
        g_options.star_wars_input == StarWarsInput::Hybrid ||
        g_options.star_wars_input == StarWarsInput::Mouse;
    const bool analogAllowStick =
        g_options.star_wars_input == StarWarsInput::Hybrid ||
        g_options.star_wars_input == StarWarsInput::AnalogSticks;
    m_mouseButtons[analogJoystickDev][0] =
        (analogAllowMouse && m_mouseButtons[0][0]) ||
        (analogAllowStick && m_joyButtons[0][0]);
    m_mouseButtons[analogJoystickDev][1] = false;
    m_mouseButtons[analogJoystickDev][2] =
        (analogAllowMouse && m_mouseButtons[0][2]) ||
        (analogAllowStick && m_joyButtons[0][1]);
    m_mouseButtons[analogJoystickDev][3] =
        (analogAllowMouse && m_mouseButtons[0][3]) ||
        (analogAllowStick && m_joyButtons[0][2]);
    m_mouseButtons[analogJoystickDev][4] =
        (analogAllowMouse && m_mouseButtons[0][4]) ||
        (analogAllowStick && m_joyButtons[0][3]);

    return true;
}

bool CLibretroInputSystem::IsJoyButPressed(int joyNum, int butNum) const
{
    if (joyNum < 0 || joyNum >= 2) return false;
    if (butNum < 0 || butNum >= NUM_JOY_BUTTONS) return false;

    return m_joyButtons[joyNum][butNum] != 0;
}


int CLibretroInputSystem::GetJoyAxisValue(int joyNum, int axisNum) const
{
    if (joyNum < 0 || joyNum >= 2) return 0;
    if (axisNum < 0 || axisNum >= NUM_JOY_AXES) return 0;
    return m_joyAxes[joyNum][axisNum];
}

bool CLibretroInputSystem::IsJoyPOVInDir(int joyNum, int povNum, int povDir) const
{
    if (joyNum < 0 || joyNum >= 2) return false;
    if (povDir < 0 || povDir > 3) return false;

    return m_joyPOV[joyNum][povDir] != 0;
}


// Supermodel key names (CInputSystem::s_validKeyNames, without the KEY_ prefix,
// which CInputSystem strips before calling GetKeyIndex) to libretro key codes.
// The generic SHIFT, CTRL and ALT names have no single libretro key and are
// not mapped, as in the SDL build.
struct KeyMapEntry
{
    const char *name;
    int code;
};

static const KeyMapEntry s_keyMap[] = {
    { "BACKSPACE",      RETROK_BACKSPACE },
    { "TAB",            RETROK_TAB },
    { "CLEAR",          RETROK_CLEAR },
    { "RETURN",         RETROK_RETURN },
    { "PAUSE",          RETROK_PAUSE },
    { "ESCAPE",         RETROK_ESCAPE },
    { "SPACE",          RETROK_SPACE },
    { "EXCLAIM",        RETROK_EXCLAIM },
    { "DBLQUOTE",       RETROK_QUOTEDBL },
    { "HASH",           RETROK_HASH },
    { "DOLLAR",         RETROK_DOLLAR },
    { "AMPERSAND",      RETROK_AMPERSAND },
    { "QUOTE",          RETROK_QUOTE },
    { "LEFTPAREN",      RETROK_LEFTPAREN },
    { "RIGHTPAREN",     RETROK_RIGHTPAREN },
    { "ASTERISK",       RETROK_ASTERISK },
    { "PLUS",           RETROK_PLUS },
    { "COMMA",          RETROK_COMMA },
    { "MINUS",          RETROK_MINUS },
    { "PERIOD",         RETROK_PERIOD },
    { "SLASH",          RETROK_SLASH },
    { "0",              RETROK_0 },
    { "1",              RETROK_1 },
    { "2",              RETROK_2 },
    { "3",              RETROK_3 },
    { "4",              RETROK_4 },
    { "5",              RETROK_5 },
    { "6",              RETROK_6 },
    { "7",              RETROK_7 },
    { "8",              RETROK_8 },
    { "9",              RETROK_9 },
    { "COLON",          RETROK_COLON },
    { "SEMICOLON",      RETROK_SEMICOLON },
    { "LESS",           RETROK_LESS },
    { "EQUALS",         RETROK_EQUALS },
    { "GREATER",        RETROK_GREATER },
    { "QUESTION",       RETROK_QUESTION },
    { "AT",             RETROK_AT },
    { "LEFTBRACKET",    RETROK_LEFTBRACKET },
    { "BACKSLASH",      RETROK_BACKSLASH },
    { "RIGHTBRACKET",   RETROK_RIGHTBRACKET },
    { "CARET",          RETROK_CARET },
    { "UNDERSCORE",     RETROK_UNDERSCORE },
    { "BACKQUOTE",      RETROK_BACKQUOTE },
    { "A",              RETROK_a },
    { "B",              RETROK_b },
    { "C",              RETROK_c },
    { "D",              RETROK_d },
    { "E",              RETROK_e },
    { "F",              RETROK_f },
    { "G",              RETROK_g },
    { "H",              RETROK_h },
    { "I",              RETROK_i },
    { "J",              RETROK_j },
    { "K",              RETROK_k },
    { "L",              RETROK_l },
    { "M",              RETROK_m },
    { "N",              RETROK_n },
    { "O",              RETROK_o },
    { "P",              RETROK_p },
    { "Q",              RETROK_q },
    { "R",              RETROK_r },
    { "S",              RETROK_s },
    { "T",              RETROK_t },
    { "U",              RETROK_u },
    { "V",              RETROK_v },
    { "W",              RETROK_w },
    { "X",              RETROK_x },
    { "Y",              RETROK_y },
    { "Z",              RETROK_z },
    { "DEL",            RETROK_DELETE },
    { "KEYPAD0",        RETROK_KP0 },
    { "KEYPAD1",        RETROK_KP1 },
    { "KEYPAD2",        RETROK_KP2 },
    { "KEYPAD3",        RETROK_KP3 },
    { "KEYPAD4",        RETROK_KP4 },
    { "KEYPAD5",        RETROK_KP5 },
    { "KEYPAD6",        RETROK_KP6 },
    { "KEYPAD7",        RETROK_KP7 },
    { "KEYPAD8",        RETROK_KP8 },
    { "KEYPAD9",        RETROK_KP9 },
    { "KEYPADPERIOD",   RETROK_KP_PERIOD },
    { "KEYPADDIVIDE",   RETROK_KP_DIVIDE },
    { "KEYPADMULTIPLY", RETROK_KP_MULTIPLY },
    { "KEYPADMINUS",    RETROK_KP_MINUS },
    { "KEYPADPLUS",     RETROK_KP_PLUS },
    { "KEYPADENTER",    RETROK_KP_ENTER },
    { "KEYPADEQUALS",   RETROK_KP_EQUALS },
    { "UP",             RETROK_UP },
    { "DOWN",           RETROK_DOWN },
    { "RIGHT",          RETROK_RIGHT },
    { "LEFT",           RETROK_LEFT },
    { "INSERT",         RETROK_INSERT },
    { "HOME",           RETROK_HOME },
    { "END",            RETROK_END },
    { "PGUP",           RETROK_PAGEUP },
    { "PGDN",           RETROK_PAGEDOWN },
    { "F1",             RETROK_F1 },
    { "F2",             RETROK_F2 },
    { "F3",             RETROK_F3 },
    { "F4",             RETROK_F4 },
    { "F5",             RETROK_F5 },
    { "F6",             RETROK_F6 },
    { "F7",             RETROK_F7 },
    { "F8",             RETROK_F8 },
    { "F9",             RETROK_F9 },
    { "F10",            RETROK_F10 },
    { "F11",            RETROK_F11 },
    { "F12",            RETROK_F12 },
    { "F13",            RETROK_F13 },
    { "F14",            RETROK_F14 },
    { "F15",            RETROK_F15 },
    { "NUMLOCK",        RETROK_NUMLOCK },
    { "CAPSLOCK",       RETROK_CAPSLOCK },
    { "SCROLLLOCK",     RETROK_SCROLLOCK },
    { "RIGHTSHIFT",     RETROK_RSHIFT },
    { "LEFTSHIFT",      RETROK_LSHIFT },
    { "RIGHTCTRL",      RETROK_RCTRL },
    { "LEFTCTRL",       RETROK_LCTRL },
    { "RIGHTALT",       RETROK_RALT },
    { "LEFTALT",        RETROK_LALT },
    { "RIGHTMETA",      RETROK_RMETA },
    { "LEFTMETA",       RETROK_LMETA },
    { "RIGHTWINDOWS",   RETROK_RSUPER },
    { "LEFTWINDOWS",    RETROK_LSUPER },
    { "ALTGR",          RETROK_MODE },
    { "COMPOSE",        RETROK_COMPOSE },
    { "HELP",           RETROK_HELP },
    { "PRINT",          RETROK_PRINT },
    { "SYSREQ",         RETROK_SYSREQ },
    { "BREAK",          RETROK_BREAK },
    { "MENU",           RETROK_MENU },
    { "POWER",          RETROK_POWER },
};

static const size_t kNumKeyMapEntries = sizeof(s_keyMap) / sizeof(s_keyMap[0]);

static bool KeyNameEquals(const char *a, const char *b)
{
    for (; *a && *b; ++a, ++b) {
        if (std::toupper((unsigned char)*a) != std::toupper((unsigned char)*b))
            return false;
    }
    return *a == *b;
}

int CLibretroInputSystem::GetKeyIndex(const char *keyName)
{
    if (!keyName) return -1;

    // Supermodel indexes its key source arrays with this value, so it must be
    // a position in s_keyMap (fewer entries than s_validKeyNames), not a
    // RETROK code.
    for (size_t i = 0; i < kNumKeyMapEntries; i++) {
        if (KeyNameEquals(keyName, s_keyMap[i].name))
            return (int)i;
    }
    return -1;
}

bool CLibretroInputSystem::IsKeyPressed(int kbdNum, int keyIndex) const
{
    if (kbdNum != 0) return false;

    if (keyIndex < 0 || (size_t)keyIndex >= kNumKeyMapEntries)
        return false;

    int code = s_keyMap[keyIndex].code;
    return code >= 0 && code < 512 && m_keyState[code];
}


bool CLibretroInputSystem::IsMouseButPressed(int mseNum, int butNum) const
{
    if (butNum < 0 || butNum >= NUM_MOUSE_BUTTONS) return false;
    if (mseNum == ANY_MOUSE)
    {
        for (int dev = 0; dev < kMouseDeviceCount; ++dev)
            if (m_mouseButtons[dev][butNum])
                return true;
        return false;
    }
    if (mseNum < 0 || mseNum >= kMouseDeviceCount) return false;
    return m_mouseButtons[mseNum][butNum];
}

int CLibretroInputSystem::GetMouseAxisValue(int mseNum, int axisNum) const
{
    if (mseNum == ANY_MOUSE)
    {
        // For absolute axes, return the one that is currently being used.
        // For simplicity, prioritize lightguns if they moved significantly, otherwise mouse 0.
        return m_mouseAxes[0][axisNum];
    }
    if (mseNum < 0 || mseNum >= kMouseDeviceCount) return 0;
    if (axisNum < 0 || axisNum >= NUM_MOUSE_AXES) return 0;
    return m_mouseAxes[mseNum][axisNum];
}

int CLibretroInputSystem::GetMouseWheelDir(int mseNum) const
{
    if (mseNum == ANY_MOUSE)
    {
        for (int dev = 0; dev < kMouseDeviceCount; ++dev)
            if (m_mouseWheelDir[dev] != 0)
                return m_mouseWheelDir[dev];
        return 0;
    }
    if (mseNum < 0 || mseNum >= kMouseDeviceCount) return 0;
    return m_mouseWheelDir[mseNum];
}

void CLibretroInputSystem::SetMouseVisibility(bool visible)
{
}

const MouseDetails *CLibretroInputSystem::GetMouseDetails(int mseNum)
{
    static MouseDetails d[kMouseDeviceCount];
    static bool initialized = false;

    if (!initialized) {
        for (int i = 0; i < 2; ++i)
        {
            std::memset(&d[i], 0, sizeof(d[i]));
            snprintf(d[i].name, MAX_NAME_LENGTH, "Libretro Mouse %d", i + 1);
            d[i].isAbsolute = false;
        }
        for (int i = 0; i < 2; ++i)
        {
            const int dev = i + 2;
            std::memset(&d[dev], 0, sizeof(d[dev]));
            snprintf(d[dev].name, MAX_NAME_LENGTH, "Libretro Lightgun %d", i + 1);
            d[dev].isAbsolute = true;
        }
        for (int i = 0; i < 2; ++i)
        {
            const int dev = i + 4;
            std::memset(&d[dev], 0, sizeof(d[dev]));
            snprintf(d[dev].name, MAX_NAME_LENGTH,
                     "Libretro Gun Cursor %d", i + 1);
            d[dev].isAbsolute = true;
        }
        std::memset(&d[6], 0, sizeof(d[6]));
        snprintf(d[6].name, MAX_NAME_LENGTH,
                 "Libretro Analog Joystick Cursor");
        d[6].isAbsolute = true;
        initialized = true;
    }
    
    if (mseNum < 0 || mseNum >= kMouseDeviceCount) return nullptr;
    return &d[mseNum];
}

bool CLibretroInputSystem::ProcessForceFeedbackCmd(int joyNum, int axisNum, ForceFeedbackCmd ffCmd)
{
    if (!m_rumbleInterface.set_rumble_state)
    {
        return false;
    }

    // If the user disabled FFB in the menu, or we have no interface, abort early
    if (!m_ffbEnabled || !m_rumbleInterface.set_rumble_state)
        return false;
        
    // 2. Map the float force (-1.0 to 1.0) to Libretro's uint16_t (0 to 65535)
    // Most gamepads only support vibration strength, not direction, so we use absolute value.
    uint16_t strength = (uint16_t)(std::min(std::abs(ffCmd.force), 1.0f) * 65535);

    switch (ffCmd.id)
    {
        case FFConstantForce:
        case FFSelfCenter:
            // High torque effects go to the Strong (Low-Frequency) motor
            m_rumbleInterface.set_rumble_state(joyNum, RETRO_RUMBLE_STRONG, strength);
            break;

        case FFVibrate:
        case FFFriction:
            // High frequency effects go to the Weak (High-Frequency) motor
            m_rumbleInterface.set_rumble_state(joyNum, RETRO_RUMBLE_WEAK, strength);
            break;

        case FFStop:
            m_rumbleInterface.set_rumble_state(joyNum, RETRO_RUMBLE_STRONG, 0);
            m_rumbleInterface.set_rumble_state(joyNum, RETRO_RUMBLE_WEAK, 0);
            break;

        default:
            return false;
    }

    return true;
}

void CLibretroInputSystem::StopAllRumble()
{
    if (!m_rumbleInterface.set_rumble_state)
        return;

    for (unsigned port = 0; port < 2; ++port)
    {
        m_rumbleInterface.set_rumble_state(port, RETRO_RUMBLE_STRONG, 0);
        m_rumbleInterface.set_rumble_state(port, RETRO_RUMBLE_WEAK, 0);
    }
}

const KeyDetails *CLibretroInputSystem::GetKeyDetails(int kbdNum)
{
    static KeyDetails d{};
    std::strncpy(d.name, "Libretro Keyboard", MAX_NAME_LENGTH);
    return &d;
}

const JoyDetails *CLibretroInputSystem::GetJoyDetails(int joyNum)
{
    static JoyDetails d[2];
    static bool initialized = false;

    if (!initialized) {
        for (int i = 0; i < 2; i++) {
            std::memset(&d[i], 0, sizeof(d[i]));
            std::strncpy(d[i].name, "Libretro Joypad", MAX_NAME_LENGTH);
            d[i].numButtons = 32;
            d[i].numAxes = 6;   // left stick, right stick, and the two triggers
            d[i].numPOVs = 1;
            d[i].hasAxis[AXIS_X]  = true;   // left stick
            d[i].hasAxis[AXIS_Y]  = true;
            d[i].hasAxis[AXIS_Z]  = true;   // L2
            d[i].hasAxis[AXIS_RX] = true;   // right stick
            d[i].hasAxis[AXIS_RY] = true;
            d[i].hasAxis[AXIS_RZ] = true;   // R2
            d[i].hasFFeedback = true;
            for (int a = 0; a < NUM_JOY_AXES; a++)
                d[i].axisHasFF[a] = true;
        }
        initialized = true;
    }
    return &d[joyNum];
}

bool CLibretroInputSystem::InitializeSystem()
{
    return true;
}
bool CLibretroInputSystem::Initialize()
{
    bool result = CInputSystem::Initialize(); // let base do its thing
    return result;
}
const char *CLibretroInputSystem::GetKeyName(int keyIndex)
{
    if (keyIndex >= 0 && (size_t)keyIndex < kNumKeyMapEntries)
        return s_keyMap[keyIndex].name;
    return "NONE";
}

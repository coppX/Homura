//
// Created by FDC on 2022/1/5.
//

#ifndef VULKANDEMO_INPUT_H
#define VULKANDEMO_INPUT_H

namespace Hela
{
    enum class EventSource
    {
        KeyCode,
        Mouse
    };

    class InputEvent
    {
    public:
        InputEvent(EventSource source)
                : mSource(source)
        {

        }

        EventSource getSource() const
        {
            return mSource;
        }

    private:
        EventSource mSource;
    };

    enum class KeyCode
    {
        Unknown,
        Space,
        Apostrophe, /* ' */
        Comma,      /* , */
        Minus,      /* - */
        Period,     /* . */
        Slash,      /* / */
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        Semicolon, /* ; */
        Equal,     /* = */
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LeftBracket,  /* [ */
        Backslash,    /* \ */
        RightBracket, /* ] */
        GraveAccent,  /* ` */
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        DelKey,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,
        Back,
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,
        KP_Decimal,
        KP_Divide,
        KP_Multiply,
        KP_Subtract,
        KP_Add,
        KP_Enter,
        KP_Equal,
        LeftShift,
        LeftControl,
        LeftAlt,
        RightShift,
        RightControl,
        RightAlt
    };

    enum class KeyAction
    {
        Down,
        Up,
        Repeat,
        Unknown
    };

    class KeyInputEvent : public InputEvent
    {
    public:
        KeyInputEvent(KeyCode code, KeyAction action)
                : InputEvent{EventSource::KeyCode}, mCode{code}, mAction{action}
        {

        }

        KeyCode getCode() const
        {
            return mCode;
        }

        KeyAction getAction() const
        {
            return mAction;
        }

    private:
        KeyCode mCode;
        KeyAction mAction;
    };

    enum class MouseButton
    {
        Left,
        Right,
        Middle,
        Back,
        Forward,
        Unknown
    };

    enum class MouseAction
    {
        Down,
        Up,
        Move,
        Unknown
    };

    class MouseEventInput : public InputEvent
    {
    public:
        MouseEventInput(MouseButton button, MouseAction action, float x, float y)
                : InputEvent{EventSource::Mouse}, mButton{button}, mAction{action}, mPosX{x}, mPosY{y}
        {

        }

        MouseButton getButton() const
        {
            return mButton;
        }

        MouseAction getAction() const
        {
            return mAction;
        }

        float getPosX() const
        {
            return mPosX;
        }

        float getPosY() const
        {
            return mPosY;
        }

    private:
        MouseButton mButton;
        MouseAction mAction;
        float mPosX;
        float mPosY;
    };
}

#endif //VULKANDEMO_INPUT_H

#ifndef WINDOW_WINDOW_HPP
#define WINDOW_WINDOW_HPP

#include <string>
#include <vector>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace proj
{
    class Window
    {
      private:
        inline static SDL_Window* win_ = nullptr;

      public:
        Window(uint32_t width, uint32_t height, const std::string& title = "");
        ~Window();

        vk::SurfaceKHR create_surface(vk::Instance instance);

        static std::vector<const char*> get_instance_exts();
        static inline SDL_Window* handle() { return Window::win_; }
    };

    // keyboard input wrapper
    class KEY
    {
      private:
        inline static int KEY_MAP_CURR_STATE[SDL_NUM_SCANCODES]{};
        inline static int KEY_MAP_PREV_STATE[SDL_NUM_SCANCODES]{};

      public:
        // normal state
        const inline static int STATE_NONE = 0;
        const inline static int STATE_UP = 1;
        const inline static int STATE_DOWN = 2;
        const inline static int STATE_HOLD = 3;

        // advance state
        const inline static int STATE_QUICK_UP = 4;
        const inline static int STATE_SLOW_UP = 5;
        const inline static int STATE_JUST_HOLD = 6;
        const inline static int STATE_JUST_DOWN = 7;
        const inline static int STATE_NEVER_PRESS = 8;

        static void set_state(int key, int state);
        static int get_state(int key);
        static int advance_state(int key);

        const inline static int UNKNOWN = SDL_SCANCODE_UNKNOWN;
        const inline static int A = SDL_SCANCODE_A;
        const inline static int B = SDL_SCANCODE_B;
        const inline static int C = SDL_SCANCODE_C;
        const inline static int D = SDL_SCANCODE_D;
        const inline static int E = SDL_SCANCODE_E;
        const inline static int F = SDL_SCANCODE_F;
        const inline static int G = SDL_SCANCODE_G;
        const inline static int H = SDL_SCANCODE_H;
        const inline static int I = SDL_SCANCODE_I;
        const inline static int J = SDL_SCANCODE_J;
        const inline static int K = SDL_SCANCODE_K;
        const inline static int L = SDL_SCANCODE_L;
        const inline static int M = SDL_SCANCODE_M;
        const inline static int N = SDL_SCANCODE_N;
        const inline static int O = SDL_SCANCODE_O;
        const inline static int P = SDL_SCANCODE_P;
        const inline static int Q = SDL_SCANCODE_Q;
        const inline static int R = SDL_SCANCODE_R;
        const inline static int S = SDL_SCANCODE_S;
        const inline static int T = SDL_SCANCODE_T;
        const inline static int U = SDL_SCANCODE_U;
        const inline static int V = SDL_SCANCODE_V;
        const inline static int W = SDL_SCANCODE_W;
        const inline static int X = SDL_SCANCODE_X;
        const inline static int Y = SDL_SCANCODE_Y;
        const inline static int Z = SDL_SCANCODE_Z;
        const inline static int N1 = SDL_SCANCODE_1;
        const inline static int N2 = SDL_SCANCODE_2;
        const inline static int N3 = SDL_SCANCODE_3;
        const inline static int N4 = SDL_SCANCODE_4;
        const inline static int N5 = SDL_SCANCODE_5;
        const inline static int N6 = SDL_SCANCODE_6;
        const inline static int N7 = SDL_SCANCODE_7;
        const inline static int N8 = SDL_SCANCODE_8;
        const inline static int N9 = SDL_SCANCODE_9;
        const inline static int N0 = SDL_SCANCODE_0;
        const inline static int RETURN = SDL_SCANCODE_RETURN;
        const inline static int ESCAPE = SDL_SCANCODE_ESCAPE;
        const inline static int BACKSPACE = SDL_SCANCODE_BACKSPACE;
        const inline static int TAB = SDL_SCANCODE_TAB;
        const inline static int SPACE = SDL_SCANCODE_SPACE;
        const inline static int MINUS = SDL_SCANCODE_MINUS;
        const inline static int EQUALS = SDL_SCANCODE_EQUALS;
        const inline static int LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET;
        const inline static int RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET;
        const inline static int BACKSLASH = SDL_SCANCODE_BACKSLASH;
        const inline static int NONUSHASH = SDL_SCANCODE_NONUSHASH;
        const inline static int SEMICOLON = SDL_SCANCODE_SEMICOLON;
        const inline static int APOSTROPHE = SDL_SCANCODE_APOSTROPHE;
        const inline static int GRAVE = SDL_SCANCODE_GRAVE;
        const inline static int COMMA = SDL_SCANCODE_COMMA;
        const inline static int PERIOD = SDL_SCANCODE_PERIOD;
        const inline static int SLASH = SDL_SCANCODE_SLASH;
        const inline static int CAPSLOCK = SDL_SCANCODE_CAPSLOCK;
        const inline static int F1 = SDL_SCANCODE_F1;
        const inline static int F2 = SDL_SCANCODE_F2;
        const inline static int F3 = SDL_SCANCODE_F3;
        const inline static int F4 = SDL_SCANCODE_F4;
        const inline static int F5 = SDL_SCANCODE_F5;
        const inline static int F6 = SDL_SCANCODE_F6;
        const inline static int F7 = SDL_SCANCODE_F7;
        const inline static int F8 = SDL_SCANCODE_F8;
        const inline static int F9 = SDL_SCANCODE_F9;
        const inline static int F10 = SDL_SCANCODE_F10;
        const inline static int F11 = SDL_SCANCODE_F11;
        const inline static int F12 = SDL_SCANCODE_F12;
        const inline static int PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN;
        const inline static int SCROLLLOCK = SDL_SCANCODE_SCROLLLOCK;
        const inline static int PAUSE = SDL_SCANCODE_PAUSE;
        const inline static int INSERT = SDL_SCANCODE_INSERT;
        const inline static int HOME = SDL_SCANCODE_HOME;
        const inline static int PAGEUP = SDL_SCANCODE_PAGEUP;
        const inline static int DELETE = SDL_SCANCODE_DELETE;
        const inline static int END = SDL_SCANCODE_END;
        const inline static int PAGEDOWN = SDL_SCANCODE_PAGEDOWN;
        const inline static int RIGHT = SDL_SCANCODE_RIGHT;
        const inline static int LEFT = SDL_SCANCODE_LEFT;
        const inline static int DOWN = SDL_SCANCODE_DOWN;
        const inline static int UP = SDL_SCANCODE_UP;
        const inline static int NUMLOCKCLEAR = SDL_SCANCODE_NUMLOCKCLEAR;
        const inline static int KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE;
        const inline static int KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY;
        const inline static int KP_MINUS = SDL_SCANCODE_KP_MINUS;
        const inline static int KP_PLUS = SDL_SCANCODE_KP_PLUS;
        const inline static int KP_ENTER = SDL_SCANCODE_KP_ENTER;
        const inline static int KP_1 = SDL_SCANCODE_KP_1;
        const inline static int KP_2 = SDL_SCANCODE_KP_2;
        const inline static int KP_3 = SDL_SCANCODE_KP_3;
        const inline static int KP_4 = SDL_SCANCODE_KP_4;
        const inline static int KP_5 = SDL_SCANCODE_KP_5;
        const inline static int KP_6 = SDL_SCANCODE_KP_6;
        const inline static int KP_7 = SDL_SCANCODE_KP_7;
        const inline static int KP_8 = SDL_SCANCODE_KP_8;
        const inline static int KP_9 = SDL_SCANCODE_KP_9;
        const inline static int KP_0 = SDL_SCANCODE_KP_0;
        const inline static int KP_PERIOD = SDL_SCANCODE_KP_PERIOD;
        const inline static int NONUSBACKSLASH = SDL_SCANCODE_NONUSBACKSLASH;
        const inline static int APPLICATION = SDL_SCANCODE_APPLICATION;
        const inline static int POWER = SDL_SCANCODE_POWER;
        const inline static int KP_EQUALS = SDL_SCANCODE_KP_EQUALS;
        const inline static int F13 = SDL_SCANCODE_F13;
        const inline static int F14 = SDL_SCANCODE_F14;
        const inline static int F15 = SDL_SCANCODE_F15;
        const inline static int F16 = SDL_SCANCODE_F16;
        const inline static int F17 = SDL_SCANCODE_F17;
        const inline static int F18 = SDL_SCANCODE_F18;
        const inline static int F19 = SDL_SCANCODE_F19;
        const inline static int F20 = SDL_SCANCODE_F20;
        const inline static int F21 = SDL_SCANCODE_F21;
        const inline static int F22 = SDL_SCANCODE_F22;
        const inline static int F23 = SDL_SCANCODE_F23;
        const inline static int F24 = SDL_SCANCODE_F24;
        const inline static int EXECUTE = SDL_SCANCODE_EXECUTE;
        const inline static int HELP = SDL_SCANCODE_HELP;
        const inline static int MENU = SDL_SCANCODE_MENU;
        const inline static int SELECT = SDL_SCANCODE_SELECT;
        const inline static int STOP = SDL_SCANCODE_STOP;
        const inline static int AGAIN = SDL_SCANCODE_AGAIN;
        const inline static int UNDO = SDL_SCANCODE_UNDO;
        const inline static int CUT = SDL_SCANCODE_CUT;
        const inline static int COPY = SDL_SCANCODE_COPY;
        const inline static int PASTE = SDL_SCANCODE_PASTE;
        const inline static int FIND = SDL_SCANCODE_FIND;
        const inline static int MUTE = SDL_SCANCODE_MUTE;
        const inline static int VOLUMEUP = SDL_SCANCODE_VOLUMEUP;
        const inline static int VOLUMEDOWN = SDL_SCANCODE_VOLUMEDOWN;
        const inline static int KP_COMMA = SDL_SCANCODE_KP_COMMA;
        const inline static int KP_EQUALSAS400 = SDL_SCANCODE_KP_EQUALSAS400;
        const inline static int INTERNATIONAL1 = SDL_SCANCODE_INTERNATIONAL1;
        const inline static int INTERNATIONAL2 = SDL_SCANCODE_INTERNATIONAL2;
        const inline static int INTERNATIONAL3 = SDL_SCANCODE_INTERNATIONAL3;
        const inline static int INTERNATIONAL4 = SDL_SCANCODE_INTERNATIONAL4;
        const inline static int INTERNATIONAL5 = SDL_SCANCODE_INTERNATIONAL5;
        const inline static int INTERNATIONAL6 = SDL_SCANCODE_INTERNATIONAL6;
        const inline static int INTERNATIONAL7 = SDL_SCANCODE_INTERNATIONAL7;
        const inline static int INTERNATIONAL8 = SDL_SCANCODE_INTERNATIONAL8;
        const inline static int INTERNATIONAL9 = SDL_SCANCODE_INTERNATIONAL9;
        const inline static int LANG1 = SDL_SCANCODE_LANG1;
        const inline static int LANG2 = SDL_SCANCODE_LANG2;
        const inline static int LANG3 = SDL_SCANCODE_LANG3;
        const inline static int LANG4 = SDL_SCANCODE_LANG4;
        const inline static int LANG5 = SDL_SCANCODE_LANG5;
        const inline static int LANG6 = SDL_SCANCODE_LANG6;
        const inline static int LANG7 = SDL_SCANCODE_LANG7;
        const inline static int LANG8 = SDL_SCANCODE_LANG8;
        const inline static int LANG9 = SDL_SCANCODE_LANG9;
        const inline static int ALTERASE = SDL_SCANCODE_ALTERASE;
        const inline static int SYSREQ = SDL_SCANCODE_SYSREQ;
        const inline static int CANCEL = SDL_SCANCODE_CANCEL;
        const inline static int CLEAR = SDL_SCANCODE_CLEAR;
        const inline static int PRIOR = SDL_SCANCODE_PRIOR;
        const inline static int RETURN2 = SDL_SCANCODE_RETURN2;
        const inline static int SEPARATOR = SDL_SCANCODE_SEPARATOR;
        const inline static int OUT = SDL_SCANCODE_OUT;
        const inline static int OPER = SDL_SCANCODE_OPER;
        const inline static int CLEARAGAIN = SDL_SCANCODE_CLEARAGAIN;
        const inline static int CRSEL = SDL_SCANCODE_CRSEL;
        const inline static int EXSEL = SDL_SCANCODE_EXSEL;
        const inline static int KP_00 = SDL_SCANCODE_KP_00;
        const inline static int KP_000 = SDL_SCANCODE_KP_000;
        const inline static int THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR;
        const inline static int DECIMALSEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR;
        const inline static int CURRENCYUNIT = SDL_SCANCODE_CURRENCYUNIT;
        const inline static int CURRENCYSUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT;
        const inline static int KP_LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN;
        const inline static int KP_RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN;
        const inline static int KP_LEFTBRACE = SDL_SCANCODE_KP_LEFTBRACE;
        const inline static int KP_RIGHTBRACE = SDL_SCANCODE_KP_RIGHTBRACE;
        const inline static int KP_TAB = SDL_SCANCODE_KP_TAB;
        const inline static int KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE;
        const inline static int KP_A = SDL_SCANCODE_KP_A;
        const inline static int KP_B = SDL_SCANCODE_KP_B;
        const inline static int KP_C = SDL_SCANCODE_KP_C;
        const inline static int KP_D = SDL_SCANCODE_KP_D;
        const inline static int KP_E = SDL_SCANCODE_KP_E;
        const inline static int KP_F = SDL_SCANCODE_KP_F;
        const inline static int KP_XOR = SDL_SCANCODE_KP_XOR;
        const inline static int KP_POWER = SDL_SCANCODE_KP_POWER;
        const inline static int KP_PERCENT = SDL_SCANCODE_KP_PERCENT;
        const inline static int KP_LESS = SDL_SCANCODE_KP_LESS;
        const inline static int KP_GREATER = SDL_SCANCODE_KP_GREATER;
        const inline static int KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND;
        const inline static int KP_DBLAMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND;
        const inline static int KP_VERTICALBAR = SDL_SCANCODE_KP_VERTICALBAR;
        const inline static int KP_DBLVERTICALBAR = SDL_SCANCODE_KP_DBLVERTICALBAR;
        const inline static int KP_COLON = SDL_SCANCODE_KP_COLON;
        const inline static int KP_HASH = SDL_SCANCODE_KP_HASH;
        const inline static int KP_SPACE = SDL_SCANCODE_KP_SPACE;
        const inline static int KP_AT = SDL_SCANCODE_KP_AT;
        const inline static int KP_EXCLAM = SDL_SCANCODE_KP_EXCLAM;
        const inline static int KP_MEMSTORE = SDL_SCANCODE_KP_MEMSTORE;
        const inline static int KP_MEMRECALL = SDL_SCANCODE_KP_MEMRECALL;
        const inline static int KP_MEMCLEAR = SDL_SCANCODE_KP_MEMCLEAR;
        const inline static int KP_MEMADD = SDL_SCANCODE_KP_MEMADD;
        const inline static int KP_MEMSUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT;
        const inline static int KP_MEMMULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY;
        const inline static int KP_MEMDIVIDE = SDL_SCANCODE_KP_MEMDIVIDE;
        const inline static int KP_PLUSMINUS = SDL_SCANCODE_KP_PLUSMINUS;
        const inline static int KP_CLEAR = SDL_SCANCODE_KP_CLEAR;
        const inline static int KP_CLEARENTRY = SDL_SCANCODE_KP_CLEARENTRY;
        const inline static int KP_BINARY = SDL_SCANCODE_KP_BINARY;
        const inline static int KP_OCTAL = SDL_SCANCODE_KP_OCTAL;
        const inline static int KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL;
        const inline static int KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL;
        const inline static int LCTRL = SDL_SCANCODE_LCTRL;
        const inline static int LSHIFT = SDL_SCANCODE_LSHIFT;
        const inline static int LALT = SDL_SCANCODE_LALT;
        const inline static int LGUI = SDL_SCANCODE_LGUI;
        const inline static int RCTRL = SDL_SCANCODE_RCTRL;
        const inline static int RSHIFT = SDL_SCANCODE_RSHIFT;
        const inline static int RALT = SDL_SCANCODE_RALT;
        const inline static int RGUI = SDL_SCANCODE_RGUI;
        const inline static int MODE = SDL_SCANCODE_MODE;
        const inline static int AUDIONEXT = SDL_SCANCODE_AUDIONEXT;
        const inline static int AUDIOPREV = SDL_SCANCODE_AUDIOPREV;
        const inline static int AUDIOSTOP = SDL_SCANCODE_AUDIOSTOP;
        const inline static int AUDIOPLAY = SDL_SCANCODE_AUDIOPLAY;
        const inline static int AUDIOMUTE = SDL_SCANCODE_AUDIOMUTE;
        const inline static int MEDIASELECT = SDL_SCANCODE_MEDIASELECT;
        const inline static int WWW = SDL_SCANCODE_WWW;
        const inline static int MAIL = SDL_SCANCODE_MAIL;
        const inline static int CALCULATOR = SDL_SCANCODE_CALCULATOR;
        const inline static int COMPUTER = SDL_SCANCODE_COMPUTER;
        const inline static int AC_SEARCH = SDL_SCANCODE_AC_SEARCH;
        const inline static int AC_HOME = SDL_SCANCODE_AC_HOME;
        const inline static int AC_BACK = SDL_SCANCODE_AC_BACK;
        const inline static int AC_FORWARD = SDL_SCANCODE_AC_FORWARD;
        const inline static int AC_STOP = SDL_SCANCODE_AC_STOP;
        const inline static int AC_REFRESH = SDL_SCANCODE_AC_REFRESH;
        const inline static int AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS;
        const inline static int BRIGHTNESSDOWN = SDL_SCANCODE_BRIGHTNESSDOWN;
        const inline static int BRIGHTNESSUP = SDL_SCANCODE_BRIGHTNESSUP;
        const inline static int DISPLAYSWITCH = SDL_SCANCODE_DISPLAYSWITCH;
        const inline static int KBDILLUMTOGGLE = SDL_SCANCODE_KBDILLUMTOGGLE;
        const inline static int KBDILLUMDOWN = SDL_SCANCODE_KBDILLUMDOWN;
        const inline static int KBDILLUMUP = SDL_SCANCODE_KBDILLUMUP;
        const inline static int EJECT = SDL_SCANCODE_EJECT;
        const inline static int SLEEP = SDL_SCANCODE_SLEEP;
        const inline static int APP1 = SDL_SCANCODE_APP1;
        const inline static int APP2 = SDL_SCANCODE_APP2;
        const inline static int AUDIOREWIND = SDL_SCANCODE_AUDIOREWIND;
        const inline static int AUDIOFASTFORWARD = SDL_SCANCODE_AUDIOFASTFORWARD;
        const inline static int SOFTLEFT = SDL_SCANCODE_SOFTLEFT;
        const inline static int SOFTRIGHT = SDL_SCANCODE_SOFTRIGHT;
        const inline static int CALL = SDL_SCANCODE_CALL;
        const inline static int ENDCALL = SDL_SCANCODE_ENDCALL;
        const inline static int NUM_SCANCODES = SDL_NUM_SCANCODES;
    };
}; // namespace proj

#endif // WINDOW_WINDOW_HPP

/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsgQt/KeyboardMap.h>

using namespace vsgQt;

KeyboardMap::KeyboardMap() :
    _keycodeMap{
        {0x0, vsg::KEY_Undefined},
        {Qt::Key_Space, vsg::KEY_Space},

        /* Cursor control & motion */

        {Qt::Key_Home, vsg::KEY_Home},
        {Qt::Key_Left, vsg::KEY_Left},   /* Move left, left arrow */
        {Qt::Key_Up, vsg::KEY_Up},       /* Move up, up arrow */
        {Qt::Key_Right, vsg::KEY_Right}, /* Move right, right arrow */
        {Qt::Key_Down, vsg::KEY_Down},   /* Move down, down arrow */
        //{Qt::Key_Home, vsg::KEY_Prior}, /* Prior, previous */
        {Qt::Key_PageUp, vsg::KEY_Page_Up},
        {Qt::Key_Home, vsg::KEY_Next}, /* Next */
        {Qt::Key_PageDown, vsg::KEY_Page_Down},
        {Qt::Key_End, vsg::KEY_End}, /* EOL */
        //{ KEY_Begin = 0xFF58, /* BOL */

        {'!', vsg::KEY_Exclaim},
        {'"', vsg::KEY_Quotedbl},
        {'#', vsg::KEY_Hash},
        {'$', vsg::KEY_Dollar},
        {'&', vsg::KEY_Ampersand},
        {Qt::Key_QuoteLeft, vsg::KEY_Quote},
        {'(', vsg::KEY_Leftparen},
        {')', vsg::KEY_Rightparen},
        {'*', vsg::KEY_Asterisk},
        {'+', vsg::KEY_Plus},
        {Qt::Key_Comma, vsg::KEY_Comma},
        {Qt::Key_Minus, vsg::KEY_Minus},
        {Qt::Key_Period, vsg::KEY_Period},
        {Qt::Key_Slash, vsg::KEY_Slash},
        {':', vsg::KEY_Colon},
        {Qt::Key_Semicolon, vsg::KEY_Semicolon},
        {'<', vsg::KEY_Less},
        {Qt::Key_Equal, vsg::KEY_Equals}, // + isn't an unmodded key, why does windows map is as a virtual??
        {'>', vsg::KEY_Greater},
        {'?', vsg::KEY_Question},
        {'@', vsg::KEY_At},
        {Qt::Key_BracketLeft, vsg::KEY_Leftbracket},
        {Qt::Key_Backslash, vsg::KEY_Backslash},
        {Qt::Key_BracketRight, vsg::KEY_Rightbracket},
        {'|', vsg::KEY_Caret},
        {'_', vsg::KEY_Underscore},
        {0xc0, vsg::KEY_Backquote},

        {Qt::Key_Backspace, vsg::KEY_BackSpace}, /* back space, back char */
        {Qt::Key_Tab, vsg::KEY_Tab},
        {Qt::Key_Backtab, vsg::KEY_Tab},
        //    KEY_Linefeed = 0xFF0A, /* Linefeed, LF */
        {Qt::Key_Clear, vsg::KEY_Clear},
        {Qt::Key_Return, vsg::KEY_Return}, /* Return, enter */
        {Qt::Key_Pause, vsg::KEY_Pause},   /* Pause, hold */
        {Qt::Key_ScrollLock, vsg::KEY_Scroll_Lock},
        //    KEY_Sys_Req = 0xFF15,
        {Qt::Key_Escape, vsg::KEY_Escape},
        {Qt::Key_Delete, vsg::KEY_Delete}, /* Delete, rubout */

        /* Misc Functions */

        {Qt::Key_Select, vsg::KEY_Select}, /* Select, mark */
        {Qt::Key_Print, vsg::KEY_Print},
        {Qt::Key_Execute, vsg::KEY_Execute}, /* Execute, run, do */
        {Qt::Key_Insert, vsg::KEY_Insert},   /* Insert, insert here */
        //{ KEY_Undo = 0xFF65,    /* Undo, oops */
        //KEY_Redo = 0xFF66,    /* redo, again */
        {Qt::Key_Menu, vsg::KEY_Menu}, /* On Windows, this is VK_APPS, the context-menu key */
        // KEY_Find = 0xFF68,    /* Find, search */
        {Qt::Key_Cancel, vsg::KEY_Cancel}, /* Cancel, stop, abort, exit */
        {Qt::Key_Help, vsg::KEY_Help},     /* Help */
        //{ KEY_Break = 0xFF6B,
        //KEY_Mode_switch = 0xFF7E,   /* Character set switch */
        //KEY_Script_switch = 0xFF7E, /* Alias for mode_switch */
        {Qt::Key_NumLock, vsg::KEY_Num_Lock},

#if 0
        {Qt::Key_0, vsg::KEY_KP_0},
        {Qt::Key_1, vsg::KEY_KP_1},
        {Qt::Key_2, vsg::KEY_KP_2},
        {Qt::Key_3, vsg::KEY_KP_3},
        {Qt::Key_4, vsg::KEY_KP_4},
        {Qt::Key_5, vsg::KEY_KP_5},
        {Qt::Key_6, vsg::KEY_KP_6},
        {Qt::Key_7, vsg::KEY_KP_7},
        {Qt::Key_8, vsg::KEY_KP_8},
        {Qt::Key_9, vsg::KEY_KP_9},

#endif
        /*
                * Auxiliary Functions; note the duplicate definitions for left and right
                * function keys;  Sun keyboards and a few other manufacturers have such
                * function key groups on the left and/or right sides of the keyboard.
                * We've not found a keyboard with more than 35 function keys total.
                */

        {Qt::Key_F1, vsg::KEY_F1},
        {Qt::Key_F2, vsg::KEY_F2},
        {Qt::Key_F3, vsg::KEY_F3},
        {Qt::Key_F4, vsg::KEY_F4},
        {Qt::Key_F5, vsg::KEY_F5},
        {Qt::Key_F6, vsg::KEY_F6},
        {Qt::Key_F7, vsg::KEY_F7},
        {Qt::Key_F8, vsg::KEY_F8},
        {Qt::Key_F9, vsg::KEY_F9},
        {Qt::Key_F10, vsg::KEY_F10},
        {Qt::Key_F11, vsg::KEY_F11},
        {Qt::Key_F12, vsg::KEY_F12},
        {Qt::Key_F13, vsg::KEY_F13},
        {Qt::Key_F14, vsg::KEY_F14},
        {Qt::Key_F15, vsg::KEY_F15},
        {Qt::Key_F16, vsg::KEY_F16},
        {Qt::Key_F17, vsg::KEY_F17},
        {Qt::Key_F18, vsg::KEY_F18},
        {Qt::Key_F19, vsg::KEY_F19},
        {Qt::Key_F20, vsg::KEY_F20},
        {Qt::Key_F21, vsg::KEY_F21},
        {Qt::Key_F22, vsg::KEY_F22},
        {Qt::Key_F23, vsg::KEY_F23},
        {Qt::Key_F24, vsg::KEY_F24},

        //KEY_F25 = 0xFFD6,
        //KEY_F26 = 0xFFD7,
        //KEY_F27 = 0xFFD8,
        //KEY_F28 = 0xFFD9,
        //KEY_F29 = 0xFFDA,
        //KEY_F30 = 0xFFDB,
        //KEY_F31 = 0xFFDC,
        //KEY_F32 = 0xFFDD,
        //KEY_F33 = 0xFFDE,
        //KEY_F34 = 0xFFDF,
        //KEY_F35 = 0xFFE0,

        /* Modifiers */

        {Qt::Key_Shift, vsg::KEY_Shift_L}, /* Left shift */
        //            {VK_RSHIFT, KEY_Shift_R},     /* Right shift */
        //            {VK_LCONTROL, KEY_Control_L}, /* Left control */
        {Qt::Key_Control, vsg::KEY_Control_R}, /* Right control */
        {Qt::Key_CapsLock, vsg::KEY_Caps_Lock} /* Caps lock */
        //KEY_Shift_Lock = 0xFFE6, /* Shift lock */

        //KEY_Meta_L = 0xFFE7,  /* Left meta */
        //KEY_Meta_R = 0xFFE8,  /* Right meta */
        //            {VK_LMENU, KEY_Alt_L},  /* Left alt */
        //            {VK_RMENU, KEY_Alt_R},  /* Right alt */
        //            {VK_LWIN, KEY_Super_L}, /* Left super */
        //            {VK_RWIN, KEY_Super_R}  /* Right super */
        //KEY_Hyper_L = 0xFFED, /* Left hyper */
        //KEY_Hyper_R = 0xFFEE  /* Right hyper */
    }
{
}

bool KeyboardMap::getKeySymbol(const QKeyEvent* e, vsg::KeySymbol& keySymbol, vsg::KeySymbol& modifiedKeySymbol, vsg::KeyModifier& keyModifier)
{
    uint16_t modifierMask = 0;
    if (e->modifiers() & Qt::ShiftModifier)
    {
        modifierMask |= vsg::KeyModifier::MODKEY_Shift;
    }
    if (e->modifiers() & Qt::ControlModifier)
    {
        modifierMask |= vsg::KeyModifier::MODKEY_Control;
    }
    if (e->modifiers() & Qt::AltModifier)
    {
        modifierMask |= vsg::KeyModifier::MODKEY_Alt;
    }
    if (e->modifiers() & Qt::MetaModifier)
    {
        modifierMask |= vsg::KeyModifier::MODKEY_Meta;
    }

    // On key releases, mod keys are not present in QKeyEvent::modifiers(), so need to manually add them to the modifierMask
    const int key = e->key();
    if (key == Qt::Key_Control)
        modifierMask |= vsg::KeyModifier::MODKEY_Control;
    else if (key == Qt::Key_Shift)
        modifierMask |= vsg::KeyModifier::MODKEY_Shift;
    else if (key == Qt::Key_Alt)
        modifierMask |= vsg::KeyModifier::MODKEY_Alt;

    keyModifier = (vsg::KeyModifier)modifierMask;

    auto itr = _keycodeMap.find((uint32_t)key);
    if (itr != _keycodeMap.end())
    {
        keySymbol = itr->second;
        modifiedKeySymbol = keySymbol;

        // std::cout<<"KeyboardMap::getKeySymbol() found in map, keySymbol = "<<keySymbol<<" "<<char(keySymbol)<< ", modifiedKeySymbol = "<<modifiedKeySymbol<<" "<<char(modifiedKeySymbol)<<std::endl;
    }
    else
    {
        keySymbol = vsg::KeySymbol(key);
        if (keySymbol >= 'A' && keySymbol <= 'Z') keySymbol = vsg::KeySymbol(int(keySymbol) + int('a' - 'A'));

        modifiedKeySymbol = vsg::KeySymbol(*(e->text().toLatin1().data()));

        // std::cout<<"KeyboardMap::getKeySymbol() NOT found in map, keySymbol = "<<keySymbol<<" "<<char(keySymbol)<< ", modifiedKeySymbol = "<<modifiedKeySymbol<<" "<<char(modifiedKeySymbol)<<std::endl;
    }

    return true;
}

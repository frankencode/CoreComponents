/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/Format>
#include <cc/ui/keyboard>

namespace cc {
namespace ui {

String str(ScanCode scanCode)
{
    const char* names[] = {
        nullptr,                    // 0
        nullptr,                    // 1
        nullptr,                    // 2
        nullptr,                    // 3
        "Key_A",                    // 4
        "Key_B",                    // 5
        "Key_C",                    // 6
        "Key_D",                    // 7
        "Key_E",                    // 8
        "Key_F",                    // 9
        "Key_G",                    // 10
        "Key_H",                    // 11
        "Key_I",                    // 12
        "Key_J",                    // 13
        "Key_K",                    // 14
        "Key_L",                    // 15
        "Key_M",                    // 16
        "Key_N",                    // 17
        "Key_O",                    // 18
        "Key_P",                    // 19
        "Key_Q",                    // 20
        "Key_R",                    // 21
        "Key_S",                    // 22
        "Key_T",                    // 23
        "Key_U",                    // 24
        "Key_V",                    // 25
        "Key_W",                    // 26
        "Key_X",                    // 27
        "Key_Y",                    // 28
        "Key_Z",                    // 29
        "Key_1",                    // 30
        "Key_2",                    // 31
        "Key_3",                    // 32
        "Key_4",                    // 33
        "Key_5",                    // 34
        "Key_6",                    // 35
        "Key_7",                    // 36
        "Key_8",                    // 37
        "Key_9",                    // 38
        "Key_0",                    // 39
        "Key_Return",               // 40
        "Key_Escape",               // 41
        "Key_Backspace",            // 42
        "Key_Tab",                  // 43
        "Key_Space",                // 44
        "Key_Minus",                // 45
        "Key_Equals",               // 46
        "Key_LeftBracket",          // 47
        "Key_RightBracket",         // 48
        "Key_Backslash",            // 49
        "Key_NonUsHash",            // 50
        "Key_Semicolon",            // 51
        "Key_Apostrophe",           // 52
        "Key_Grave",                // 53
        "Key_Comma",                // 54
        "Key_Period",               // 55
        "Key_Slash",                // 56
        "Key_CapsLock",             // 57
        "Key_F1",                   // 58
        "Key_F2",                   // 59
        "Key_F3",                   // 60
        "Key_F4",                   // 61
        "Key_F5",                   // 62
        "Key_F6",                   // 63
        "Key_F7",                   // 64
        "Key_F8",                   // 65
        "Key_F9",                   // 66
        "Key_F10",                  // 67
        "Key_F11",                  // 68
        "Key_F12",                  // 69
        "Key_PrintScreen",          // 70
        "Key_ScrollLock",           // 71
        "Key_Pause",                // 72
        "Key_Insert",               // 73
        "Key_Home",                 // 74
        "Key_PageUp",               // 75
        "Key_Delete",               // 76
        "Key_End",                  // 77
        "Key_PageDown",             // 78
        "Key_Right",                // 79
        "Key_Left",                 // 80
        "Key_Down",                 // 81
        "Key_Up",                   // 82
        "Key_NumLockClear",         // 83
        "KeyPad_Divide",            // 84
        "KeyPad_Multiply",          // 85
        "KeyPad_Minus",             // 86
        "KeyPad_Plus",              // 87
        "KeyPad_Enter",             // 88
        "KeyPad_1",                 // 89
        "KeyPad_2",                 // 90
        "KeyPad_3",                 // 91
        "KeyPad_4",                 // 92
        "KeyPad_5",                 // 93
        "KeyPad_6",                 // 94
        "KeyPad_7",                 // 95
        "KeyPad_8",                 // 96
        "KeyPad_9",                 // 97
        "KeyPad_0",                 // 98
        "KeyPad_Period",            // 99
        "Key_NonUsBackslash",       // 100
        "Key_Application",          // 101
        "Key_Power",                // 102
        "KeyPad_Equals",            // 103
        "Key_F13",                  // 104
        "Key_F14",                  // 105
        "Key_F15",                  // 106
        "Key_F16",                  // 107
        "Key_F17",                  // 108
        "Key_F18",                  // 109
        "Key_F19",                  // 110
        "Key_F20",                  // 111
        "Key_F21",                  // 112
        "Key_F22",                  // 113
        "Key_F23",                  // 114
        "Key_F24",                  // 115
        "Key_Execute",              // 116
        "Key_Help",                 // 117
        "Key_Menu",                 // 118
        "Key_Select",               // 119
        "Key_Stop",                 // 120
        "Key_Again",                // 121
        "Key_Undo",                 // 122
        "Key_Cut",                  // 123
        "Key_Copy",                 // 124
        "Key_Paste",                // 125
        "Key_Find",                 // 126
        "Key_Mute",                 // 127
        "Key_VolumeUp",             // 128
        "Key_VolumeDown",           // 129
        "Key_LockingCapsLock",      // 130
        "Key_LockingNumLock",       // 131
        "Key_LockingScrollLock",    // 132
        "KeyPad_Comma",             // 133
        "KeyPad_EqualsAs400",       // 134
        "Key_International1",       // 135
        "Key_International2",       // 136
        "Key_International3",       // 137
        "Key_International4",       // 138
        "Key_International5",       // 139
        "Key_International6",       // 140
        "Key_International7",       // 141
        "Key_International8",       // 142
        "Key_International9",       // 143
        "Key_Lang1",                // 144
        "Key_Lang2",                // 145
        "Key_Lang3",                // 146
        "Key_Lang4",                // 147
        "Key_Lang5",                // 148
        "Key_Lang6",                // 149
        "Key_Lang7",                // 150
        "Key_Lang8",                // 151
        "Key_Lang9",                // 152
        "Key_AltErase",             // 153
        "Key_SysReq",               // 154
        "Key_Cancel",               // 155
        "Key_Clear",                // 156
        "Key_Prior",                // 157
        "Key_Return2",              // 158
        "Key_Separator",            // 159
        "Key_Out",                  // 160
        "Key_Oper",                 // 161
        "Key_ClearAgain",           // 162
        "Key_CrSel",                // 163
        "Key_ExSel",                // 164
        nullptr,                    // 165
        nullptr,                    // 166
        nullptr,                    // 167
        nullptr,                    // 168
        nullptr,                    // 169
        nullptr,                    // 170
        nullptr,                    // 171
        nullptr,                    // 172
        nullptr,                    // 173
        nullptr,                    // 174
        nullptr,                    // 175
        "KeyPad_00",                // 176
        "KeyPad_000",               // 177
        "Key_ThousandsSeparator",   // 178
        "Key_DecimalSeparator",     // 179
        "Key_CurrencyUnit",         // 180
        "Key_CurrencySubUnit",      // 181
        "KeyPad_LeftParenthesis",   // 182
        "KeyPad_RightParenthesis",  // 183
        "KeyPad_LeftBrace",         // 184
        "KeyPad_RightBrace",        // 185
        "KeyPad_Tab",               // 186
        "KeyPad_Backspace",         // 187
        "KeyPad_A",                 // 188
        "KeyPad_B",                 // 189
        "KeyPad_C",                 // 190
        "KeyPad_D",                 // 191
        "KeyPad_E",                 // 192
        "KeyPad_F",                 // 193
        "KeyPad_Xor",               // 194
        "KeyPad_Power",             // 195
        "KeyPad_Percent",           // 196
        "KeyPad_Less",              // 197
        "KeyPad_Greater",           // 198
        "KeyPad_Ampersand",         // 199
        "KeyPad_DoubleAmpersand",   // 200
        "KeyPad_VerticalBar",       // 201
        "KeyPad_DoubleVerticalBar", // 202
        "KeyPad_Colon",             // 203
        "KeyPad_Hash",              // 204
        "KeyPad_Space",             // 205
        "KeyPad_At",                // 206
        "KeyPad_ExclamationMark",   // 207
        "KeyPad_MemoryStore",       // 208
        "KeyPad_MemoryRecall",      // 209
        "KeyPad_MemoryClear",       // 210
        "KeyPad_MemoryAdd",         // 211
        "KeyPad_MemorySubstract",   // 212
        "KeyPad_MemoryMultiply",    // 213
        "KeyPad_MemoryDivide",      // 214
        "KeyPad_PlusMinus",         // 215
        "KeyPad_Clear",             // 216
        "KeyPad_ClearEntry",        // 217
        "KeyPad_Binary",            // 218
        "KeyPad_Octal",             // 219
        "KeyPad_Decimal",           // 220
        "KeyPad_Hexadecimal",       // 221
        nullptr,                    // 222
        nullptr,                    // 223
        "Key_LeftControl",          // 224
        "Key_LeftShift",            // 225
        "Key_LeftAlt",              // 226
        "Key_LeftMeta",             // 227
        "Key_RightControl",         // 228
        "Key_RightShift",           // 229
        "Key_RightAlt",             // 230
        "Key_RightMeta",            // 231
        nullptr,                    // 232
        nullptr,                    // 233
        nullptr,                    // 234
        nullptr,                    // 235
        nullptr,                    // 236
        nullptr,                    // 237
        nullptr,                    // 238
        nullptr,                    // 239
        nullptr,                    // 240
        nullptr,                    // 241
        nullptr,                    // 242
        nullptr,                    // 243
        nullptr,                    // 244
        nullptr,                    // 245
        nullptr,                    // 246
        nullptr,                    // 247
        nullptr,                    // 248
        nullptr,                    // 249
        nullptr,                    // 250
        nullptr,                    // 251
        nullptr,                    // 252
        nullptr,                    // 253
        nullptr,                    // 254
        nullptr,                    // 255
        nullptr,                    // 256
        "Key_Mode",                 // 257
        "Key_AudioNext",            // 258
        "Key_AudioPrevious",        // 259
        "Key_AudioStop",            // 260
        "Key_AudioPlay",            // 261
        "Key_AudioMute",            // 262
        "Key_MediaSelect",          // 263
        "Key_Internet",             // 264
        "Key_Mail",                 // 265
        "Key_Calculator",           // 266
        "Key_Computer",             // 267
        "Key_AppSearch",            // 268
        "Key_AppHome",              // 269
        "Key_AppBack",              // 270
        "Key_AppForward",           // 271
        "Key_AppStop",              // 272
        "Key_AppRefresh",           // 273
        "Key_AppBookmarks",         // 274
        "Key_BrightnessDown",       // 275
        "Key_BrightnessUp",         // 276
        "Key_DisplaySwitch",        // 277
        "Key_KeyboardLightToggle",  // 278
        "Key_KeyboardLightDown",    // 279
        "Key_KeyboardLightUp",      // 280
        "Key_Eject",                // 281
        "Key_Sleep",                // 282
        "Key_App1",                 // 283
        "Key_App2",                 // 284
        "Key_AudioRewind",          // 285
        "Key_AudioFastForward"      // 286
    };

    const int namesCount = sizeof(names) / sizeof(names[0]);
    static_assert(namesCount == 287, "");

    if (4 <= +scanCode && +scanCode < namesCount && names[+scanCode]) return names[+scanCode];
    return Format{"Unknown (%%)"} << +scanCode;
}

String str(KeyCode keyCode)
{
    if (0x20 <= +keyCode && +keyCode <= 0x7E) return Format{"'%%'"} << char(+keyCode);
    return "0x" + hex(+keyCode);
}

String str(KeyModifier modifiers)
{
    if (modifiers == KeyModifier::None) return "None";

    Ref<StringList> parts = StringList::create();
    if (+(modifiers & KeyModifier::LeftShift   )) parts << "LeftShift";
    if (+(modifiers & KeyModifier::RightShift  )) parts << "RightShift";
    if (+(modifiers & KeyModifier::LeftControl )) parts << "LeftControl";
    if (+(modifiers & KeyModifier::RightControl)) parts << "RightControl";
    if (+(modifiers & KeyModifier::LeftAlt     )) parts << "LeftAlt";
    if (+(modifiers & KeyModifier::RightAlt    )) parts << "RightAlt";
    if (+(modifiers & KeyModifier::LeftMeta    )) parts << "LeftMeta";
    if (+(modifiers & KeyModifier::RightMeta   )) parts << "RightMeta";
    if (+(modifiers & KeyModifier::NumLock     )) parts << "NumLock";
    if (+(modifiers & KeyModifier::CapsLock    )) parts << "CapsLock";
    if (+(modifiers & KeyModifier::ModeLock    )) parts << "ModeLock";
    return parts->join("|");
}

}} // namespace cc::ui

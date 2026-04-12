#pragma once
#include "starlib/general/stdint.hpp"

namespace starwin
{
    using namespace starlib::stdint;

    ///Named types for standard inputs.
    enum class input_control_type : u32
    {
        unknown = u32_max,

        key_a = 65,
        key_b = 66,
        key_c = 67,
        key_d = 68,
        key_e = 69,
        key_f = 70,
        key_g = 71,
        key_h = 72,
        key_i = 73,
        key_j = 74,
        key_k = 75,
        key_l = 76,
        key_m = 77,
        key_n = 78,
        key_o = 79,
        key_p = 80,
        key_q = 81,
        key_r = 82,
        key_s = 83,
        key_t = 84,
        key_u = 85,
        key_v = 86,
        key_w = 87,
        key_x = 88,
        key_y = 89,
        key_z = 90,

        key_0 = 48,
        key_1 = 49,
        key_2 = 50,
        key_3 = 51,
        key_4 = 52,
        key_5 = 53,
        key_6 = 54,
        key_7 = 55,
        key_8 = 56,
        key_9 = 57,

        key_space = 32,
        key_apostrophe = 39,
        key_comma = 44,
        key_period = 46,
        key_slash = 47,
        key_backslash = 92,
        key_semicolon = 59,
        key_minus = 45,
        key_equal = 61,
        key_left_bracket = 91,
        key_right_bracket = 93,
        key_grave_accent = 96,
        key_world_1 = 161,
        key_world_2 = 162,
        key_escape = 256,
        key_enter = 257,
        key_tab = 258,
        key_backspace = 259,
        key_insert = 260,
        key_del = 261,
        key_right = 262,
        key_left = 263,
        key_down = 264,
        key_up = 265,
        key_page_up = 266,
        key_page_down = 267,
        key_home = 268,
        key_end = 269,
        key_caps_lock = 280,
        key_scroll_lock = 281,
        key_num_lock = 282,
        key_print_screen = 283,
        key_pause = 284,

        key_f1 = 290,
        key_f2 = 291,
        key_f3 = 292,
        key_f4 = 293,
        key_f5 = 294,
        key_f6 = 295,
        key_f7 = 296,
        key_f8 = 297,
        key_f9 = 298,
        key_f10 = 299,
        key_f11 = 300,
        key_f12 = 301,
        key_f13 = 302,
        key_f14 = 303,
        key_f15 = 304,
        key_f16 = 305,
        key_f17 = 306,
        key_f18 = 307,
        key_f19 = 308,
        key_f20 = 309,
        key_f21 = 310,
        key_f22 = 311,
        key_f23 = 312,
        key_f24 = 313,
        key_f25 = 314,

        key_numpad_0 = 320,
        key_numpad_1 = 321,
        key_numpad_2 = 322,
        key_numpad_3 = 323,
        key_numpad_4 = 324,
        key_numpad_5 = 325,
        key_numpad_6 = 326,
        key_numpad_7 = 327,
        key_numpad_8 = 328,
        key_numpad_9 = 329,

        key_numpad_decimal = 330,
        key_numpad_divide = 331,
        key_numpad_multiply = 332,
        key_numpad_subtract = 333,
        key_numpad_add = 334,
        key_numpad_enter = 335,
        key_numpad_equal = 336,

        key_left_shift = 340,
        key_left_control = 341,
        key_left_alt = 342,
        key_left_super = 343,
        key_right_shift = 344,
        key_right_control = 345,
        key_right_alt = 346,
        key_right_super = 347,
        key_menu = 348,

        mouse_left = 500,
        mouse_right = 501,
        mouse_middle = 502,

        mouse_side_1 = 503,
        mouse_side_2 = 504,
        mouse_side_3 = 505,
        mouse_side_4 = 506,
        mouse_side_5 = 507,

        mouse_horizontal_pos = 508,
        mouse_vertical_pos = 509,

        mouse_horizontal_scroll = 510,
        mouse_vertical_scroll = 511,

        gamepad_a = 600,
        gamepad_b = 601,
        gamepad_x = 602,
        gamepad_y = 603,

        gamepad_left_bumper = 604,
        gamepad_right_bumper = 605,

        gamepad_select = 606,
        gamepad_start = 607,
        gamepad_home = 608,

        gamepad_left_joystick_click = 609,
        gamepad_right_joystick_click = 610,

        gamepad_dpad_up = 611,
        gamepad_dpad_right = 612,
        gamepad_dpad_down = 613,
        gamepad_dpad_left = 614,

        gamepad_left_joystick_x = 700,
        gamepad_left_joystick_y = 701,
        gamepad_right_joystick_x = 702,
        gamepad_right_joystick_y = 703,

        gamepad_left_trigger = 704,
        gamepad_right_trigger = 705,
    };

    enum class input_device_type
    {
        keyboard, mouse, controller
    };

    enum class input_event_type
    {
        value_change, device_connect, device_disconnect
    };

    struct input_event
    {
        ///Input device type that generated the event
        input_device_type device_type;

        ///Type of input event
        input_event_type event_type;

        ///Arbitrary ID that identifies the specific input device.
        i32 device_id;

        ///Name of the device. Only provided in device connection events.
        const char* device_name;

        ///Arbitrary ID that identifies the physical control. Consistent on a per-device basis.
        u64 input_control_id;

        ///Identifier for the control, if known.
        input_control_type input_control_name;

        ///New value of the control. \n
        ///Buttons: -1 to +1, where <0 indicates the button is released. \n
        ///Controller axes: -1 to +1, where <0 is normally left/up. \n
        ///Controller triggers: -1 to +1, where -1 is fully released. \n
        ///Mouse position: Absolute x or y. \n
        ///Mouse scroll: Delta h or v. \n
        f64 value;
    };
}

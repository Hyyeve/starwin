#include "glfw_window.hpp"

#include <format>
#include <unordered_set>
#include <tracy/Tracy.hpp>

namespace starwin
{
    constexpr static u64 scancode_control_bit = (1ull << 63ull);
    constexpr static u64 mouse_control_bit = (1ull << 62ull);
    constexpr static u64 controller_button_bit = (1ull << 61ull);
    constexpr static u64 controller_axis_bit = (1ull << 60ull);

    static std::unordered_map<u64, std::string> nonprintable_keyname_map;
    static bool has_init_nonprintable_map;

    const static std::array<std::string, 12> mouse_button_names = {
        "mouse_left",
        "mouse_right",
        "mouse_middle",
        "mouse_side_1",
        "mouse_side_2",
        "mouse_side_3",
        "mouse_side_4",
        "mouse_side_5",
        "mouse_horizontal_pos",
        "mouse_vertical_pos",
        "mouse_horizontal_scroll",
        "mouse_vertical_scroll",
    };

    const static std::array<std::string, 15> controller_button_names = {
        "gamepad_a",
        "gamepad_b",
        "gamepad_x",
        "gamepad_y",
        "gamepad_left_bumper",
        "gamepad_right_bumper",
        "gamepad_select",
        "gamepad_start",
        "gamepad_home",
        "gamepad_left_joystick_click",
        "gamepad_right_joystick_click",
        "gamepad_dpad_up",
        "gamepad_dpad_right",
        "gamepad_dpad_down",
        "gamepad_dpad_left",
    };

    const static std::array<std::string, 6> controller_axis_names = {
        "gamepad_left_joystick_x",
        "gamepad_left_joystick_y",
        "gamepad_right_joystick_x",
        "gamepad_right_joystick_y",
        "gamepad_left_trigger",
        "gamepad_right_trigger",
    };

    static bool has_loaded_glfw;

    void check_load_glfw()
    {
        if (!has_loaded_glfw)
        {
            has_loaded_glfw = (glfwInit() == GLFW_TRUE);
        }
    }

    void init_nonprintable_keyname_map(window* window)
    {
        const std::vector<std::tuple<input_control_type, std::string>> unprintables = {
            {input_control_type::key_space, "space"}, {input_control_type::key_left_alt, "left_alt"},
            {input_control_type::key_right_alt, "right_alt"}, {input_control_type::key_escape, "esc"},
            {input_control_type::key_enter, "enter"}, {input_control_type::key_tab, "tab"},
            {input_control_type::key_backspace, "backspace"}, {input_control_type::key_insert, "insert"},
            {input_control_type::key_del, "delete"}, {input_control_type::key_left, "left_arrow"},
            {input_control_type::key_right, "right_arrow"}, {input_control_type::key_up, "up_arrow"},
            {input_control_type::key_down, "down_arrow"}, {input_control_type::key_page_up, "page_up"},
            {input_control_type::key_page_down, "page_down"}, {input_control_type::key_home, "home"},
            {input_control_type::key_end, "end"}, {input_control_type::key_caps_lock, "capslock"},
            {input_control_type::key_scroll_lock, "scroll_lock"},
            {input_control_type::key_num_lock, "numlock"}, {input_control_type::key_print_screen, "printscreen"},
            {input_control_type::key_pause, "pause"}, {input_control_type::key_f1, "f1"},
            {input_control_type::key_f2, "f2"}, {input_control_type::key_f3, "f3"},
            {input_control_type::key_f4, "f4"}, {input_control_type::key_f5, "f5"},
            {input_control_type::key_f6, "f6"}, {input_control_type::key_f7, "f7"},
            {input_control_type::key_f8, "f8"}, {input_control_type::key_f9, "f9"},
            {input_control_type::key_f10, "f10"}, {input_control_type::key_f11, "f11"},
            {input_control_type::key_f12, "f12"}, {input_control_type::key_f13, "f13"},
            {input_control_type::key_f14, "f14"}, {input_control_type::key_f15, "f15"},
            {input_control_type::key_f16, "f16"}, {input_control_type::key_f17, "f17"},
            {input_control_type::key_f18, "f18"}, {input_control_type::key_f19, "f19"},
            {input_control_type::key_f20, "f20"}, {input_control_type::key_f21, "f21"},
            {input_control_type::key_f22, "f22"}, {input_control_type::key_f23, "f23"},
            {input_control_type::key_f24, "f24"}, {input_control_type::key_f25, "f25"},
            {input_control_type::key_numpad_0, "numpad_0"}, {input_control_type::key_numpad_1, "numpad_1"},
            {input_control_type::key_numpad_2, "numpad_2"}, {input_control_type::key_numpad_3, "numpad_3"},
            {input_control_type::key_numpad_4, "numpad_4"}, {input_control_type::key_numpad_5, "numpad_5"},
            {input_control_type::key_numpad_6, "numpad_6"}, {input_control_type::key_numpad_7, "numpad_7"},
            {input_control_type::key_numpad_8, "numpad_8"}, {input_control_type::key_numpad_9, "numpad_9"},
            {input_control_type::key_numpad_decimal, "numpad_decimal"}, {input_control_type::key_numpad_divide, "numpad_divide"},
            {input_control_type::key_numpad_multiply, "numpad_multiply"}, {input_control_type::key_numpad_subtract, "numpad_subtract"},
            {input_control_type::key_numpad_add, "numpad_add"}, {input_control_type::key_numpad_enter, "numpad_enter"},
            {input_control_type::key_numpad_equal, "numpad_equal"}, {input_control_type::key_left_shift, "left_shift"},
            {input_control_type::key_left_control, "left_control"}, {input_control_type::key_left_alt, "left_alt"},
            {input_control_type::key_left_super, "left_super"}, {input_control_type::key_right_shift, "right_shift"},
            {input_control_type::key_right_control, "right_control"}, {input_control_type::key_right_alt, "right_alt"},
            {input_control_type::key_right_super, "right_super"}, {input_control_type::key_menu, "menu"},
        };

        for (const auto& [keycode, name] : unprintables)
        {
            nonprintable_keyname_map[window->get_input_control_id(keycode)] = name;
        }
    }

    u64 scancode_to_input_control(const i32 scancode)
    {
        return static_cast<u64>(scancode) | scancode_control_bit;
    }

    u64 mouse_input_to_input_control(const i32 mouse_button)
    {
        return static_cast<u64>(mouse_button) | mouse_control_bit;
    }

    u64 controller_button_to_input_control(const i32 controller_button)
    {
        return static_cast<u64>(controller_button) | controller_button_bit;
    }

    u64 controller_axis_to_input_control(const i32 controller_axis)
    {
        return static_cast<u64>(controller_axis) | controller_axis_bit;
    }

    input_control_type keyname_to_input_control_type(const i32 keyname)
    {
        if (keyname == GLFW_KEY_UNKNOWN) return input_control_type::unknown;
        return static_cast<input_control_type>(keyname);
    }

    input_control_type mouse_button_to_input_control_type(const i32 mouse_button)
    {
        return static_cast<input_control_type>(mouse_button + 500);
    }

    input_control_type controller_button_to_input_control_type(const i32 controller_button)
    {
        return static_cast<input_control_type>(controller_button + 600);
    }

    input_control_type controller_axis_to_input_control_type(const i32 controller_axis)
    {
        return static_cast<input_control_type>(controller_axis + 700);
    }

    static std::unordered_set<glfw_window*> all_windows;

    std::string get_controller_name(const u32 controller_index)
    {
        ZoneScoped;
        if (glfwJoystickIsGamepad(controller_index)) return glfwGetGamepadName(controller_index);
        return glfwGetJoystickName(controller_index);
    }

    status glfw_window::set_title(const std::string& title)
    {
        ZoneScoped;
        glfwSetWindowTitle(handle, title.c_str());
        return status_from_last_glfw_error();
    }

    status glfw_window::set_icon(const u32 width, const u32 height, void* rgba8_pixels)
    {
        ZoneScoped;
        GLFWimage image;
        image.width = width;
        image.height = height;
        image.pixels = static_cast<unsigned char*>(rgba8_pixels);
        glfwSetWindowIcon(handle, 1, &image);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_cursor_mode(const cursor_mode mode)
    {
        ZoneScoped;
        i32 mode_id = GLFW_CURSOR_NORMAL;
        switch (mode)
        {
            case cursor_mode::NORMAL:
            {
                mode_id = GLFW_CURSOR_NORMAL;
                break;
            }
            case cursor_mode::HIDDEN:
            {
                mode_id = GLFW_CURSOR_HIDDEN;
                break;
            }
            case cursor_mode::CAPTURED:
            {
                mode_id = GLFW_CURSOR_DISABLED;
                break;
            }
            case cursor_mode::CONFINED:
            {
                mode_id = GLFW_CURSOR_CAPTURED;
                break;
            }
        }

        glfwSetInputMode(handle, GLFW_CURSOR, mode_id);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_visible(const bool visible)
    {
        ZoneScoped;
        if (visible) glfwShowWindow(handle);
        else glfwHideWindow(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_floating(const bool floating)
    {
        ZoneScoped;
        glfwSetWindowAttrib(handle, GLFW_FLOATING, floating);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_opacity(const f32 opacity)
    {
        ZoneScoped;
        glfwSetWindowOpacity(handle, opacity);
        return status_from_last_glfw_error();
    }

    status glfw_window::steal_focus()
    {
        ZoneScoped;
        glfwFocusWindow(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::request_focus()
    {
        ZoneScoped;
        glfwRequestWindowAttention(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_size(const u32 width, const u32 height)
    {
        ZoneScoped;
        glfwSetWindowSize(handle, width, height);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_position(const i32 x, const i32 y)
    {
        ZoneScoped;
        glfwSetWindowPos(handle, x, y);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_decorated(const bool decorations)
    {
        ZoneScoped;
        glfwSetWindowAttrib(handle, GLFW_DECORATED, decorations);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_resizable(const bool resizable)
    {
        ZoneScoped;
        glfwSetWindowAttrib(handle, GLFW_RESIZABLE, resizable);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_resizing_limit(const u32 min_width, const u32 min_height, const u32 max_width, const u32 max_height)
    {
        ZoneScoped;
        glfwSetWindowSizeLimits(handle, min_width, min_height, max_width, max_height);
        return status_from_last_glfw_error();
    }

    status glfw_window::clear_resizing_limit()
    {
        ZoneScoped;
        glfwSetWindowSizeLimits(handle, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
        return status_from_last_glfw_error();
    }

    status glfw_window::set_aspect_ratio_limit(const u32 width, const u32 height)
    {
        ZoneScoped;
        glfwSetWindowAspectRatio(handle, width, height);
        return status_from_last_glfw_error();
    }

    status glfw_window::clear_aspect_ratio_limit()
    {
        ZoneScoped;
        glfwSetWindowAspectRatio(handle, GLFW_DONT_CARE, GLFW_DONT_CARE);
        return status_from_last_glfw_error();
    }

    status glfw_window::to_exclusive_fullscreen(const u32 display_index, const fullscreen_window_config config)
    {
        ZoneScoped;
        const std::vector<GLFWmonitor*> monitors = get_monitors();
        if (display_index >= monitors.size()) return {status_type::RANGE_OVERFLOW, std::format("Display index {0} is out of range", display_index)};
        GLFWmonitor* desired_monitor = monitors[display_index];
        glfwSetWindowMonitor(handle, desired_monitor, 0, 0, config.width, config.height, config.refresh_rate);
        return status_from_last_glfw_error();
    }

    status glfw_window::to_windowed_fullscreen(const u32 display_index)
    {
        ZoneScoped;
        const std::vector<GLFWmonitor*> monitors = get_monitors();
        if (display_index >= monitors.size()) return {status_type::RANGE_OVERFLOW, std::format("Display index {0} is out of range", display_index)};
        GLFWmonitor* desired_monitor = monitors[display_index];

        i32 monitor_x, monitor_y, monitor_width, monitor_height;
        glfwGetMonitorWorkarea(desired_monitor, &monitor_x, &monitor_y, &monitor_width, &monitor_height);
        glfwSetWindowMonitor(handle, nullptr, monitor_x, monitor_y, monitor_width, monitor_height, GLFW_DONT_CARE);
        return status_from_last_glfw_error();
    }

    status glfw_window::to_windowed(const u32 width, const u32 height, const i32 x, const i32 y)
    {
        ZoneScoped;
        glfwSetWindowMonitor(handle, nullptr, x, y, width, height, GLFW_DONT_CARE);
        return status_from_last_glfw_error();
    }

    status glfw_window::maximise()
    {
        ZoneScoped;
        glfwMaximizeWindow(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::minimise()
    {
        ZoneScoped;
        glfwIconifyWindow(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::restore()
    {
        ZoneScoped;
        glfwRestoreWindow(handle);
        return status_from_last_glfw_error();
    }

    ///Under GLFW, input IDs match GLFW's defined input indices, plus marker bits (but use scancodes for keyboard keys)
    u64 glfw_window::get_input_control_id(const input_control_type control)
    {
        const u64 control_index = static_cast<u32>(control);
        if (control_index == u32_max) return u64_max;                                             //Unknown
        if (control_index < 500) return glfwGetKeyScancode(control_index) | scancode_control_bit; //Keyboard
        if (control_index < 600) return (control_index - 500) | mouse_control_bit;                //Mouse
        if (control_index < 700) return (control_index - 600) | controller_button_bit;            //Controller buttons
        if (control_index < 800) return (control_index - 700) | controller_axis_bit;              //Controller axes
        return u64_max;
    }

    std::string glfw_window::get_input_control_name(const u64 input_control_id)
    {
        if (input_control_id == u64_max) return "unknown";
        if (input_control_id & scancode_control_bit)
        {
            if (!has_init_nonprintable_map) init_nonprintable_keyname_map(this);
            if (nonprintable_keyname_map.contains(input_control_id)) return nonprintable_keyname_map[input_control_id];
            const u32 scancode = input_control_id ^ scancode_control_bit;
            const char* str = glfwGetKeyName(GLFW_KEY_UNKNOWN, scancode);
            return str != nullptr ? std::string(str) : "unknown";
        }

        if (input_control_id & mouse_control_bit)
        {
            const u32 mouse_button_id = input_control_id ^ mouse_control_bit;
            return mouse_button_names[mouse_button_id];
        }

        if (input_control_id & controller_button_bit)
        {
            const u32 controller_button = input_control_id ^ controller_button_bit;
            return controller_button_names[controller_button];
        }

        if (input_control_id & controller_axis_bit)
        {
            const u32 controller_axis = input_control_id ^ controller_axis_bit;
            return controller_axis_names[controller_axis];
        }

        return "unknown";
    }

    display_info glfw_window::get_primary_display() const
    {
        ZoneScoped;
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const std::vector<GLFWmonitor*> monitors = get_monitors();

        for (u32 idx = 0; idx < monitors.size(); idx++)
        {
            if (primary != monitors[idx]) continue;
            return get_display_info(primary, idx);
        }

        return get_display_info(primary, 0);
    }

    std::vector<fullscreen_window_config> glfw_window::get_supported_fullscreen_configs(const u32 display_index) const
    {
        ZoneScoped;
        std::vector<fullscreen_window_config> results;

        const std::vector<GLFWmonitor*> monitors = get_monitors();
        if (display_index >= monitors.size()) return results;
        GLFWmonitor* desired_monitor = monitors[display_index];

        i32 num_modes;
        const GLFWvidmode* modes = glfwGetVideoModes(desired_monitor, &num_modes);

        for (u32 idx = 0; idx < num_modes; idx++)
        {
            const GLFWvidmode mode = modes[idx];
            results.push_back(fullscreen_window_config {static_cast<u32>(mode.width), static_cast<u32>(mode.height), static_cast<u32>(mode.refreshRate)});
        }

        return results;
    }

    std::vector<display_info> glfw_window::get_available_displays() const
    {
        ZoneScoped;
        std::vector<display_info> display_infos;
        const std::vector<GLFWmonitor*> monitors = get_monitors();
        for (u32 idx = 0; idx < monitors.size(); idx++)
        {
            display_infos.push_back(get_display_info(monitors[idx], idx));
        }

        return display_infos;
    }

    bool glfw_window::is_close_requested() const
    {
        ZoneScoped;
        return glfwWindowShouldClose(handle);
    }

    bool glfw_window::is_focused() const
    {
        ZoneScoped;
        return glfwGetWindowAttrib(handle, GLFW_FOCUSED);
    }

    gl_loader_func glfw_window::gl_get_loader_func()
    {
        return reinterpret_cast<gl_loader_func>(glfwGetProcAddress);
    }

    status glfw_window::gl_apply_context()
    {
        ZoneScoped;
        glfwMakeContextCurrent(handle);
        return status_from_last_glfw_error();
    }

    status glfw_window::poll()
    {
        ZoneScoped;
        collected_events.clear();
        glfwPollEvents();
        poll_controllers();
        if (collected_events.size() > 0) callbacks.on_input_events(this, collected_events);
        return status_from_last_glfw_error();
    }

    glfw_window::glfw_window()
    {
        check_load_glfw();
        all_windows.emplace(this);
        init_input_state();
    }

    glfw_window::~glfw_window()
    {
        all_windows.erase(this);
    }

    status glfw_window::initialize_window(const window_config& config)
    {
        ZoneScoped;
        glfwWindowHint(GLFW_AUTO_ICONIFY, false);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, config.transparent_framebuffer ? GLFW_TRUE : GLFW_FALSE);
        handle = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

        if (handle == nullptr)
        {
            return status_from_last_glfw_error();
        }

        glfwSetWindowUserPointer(handle, this);

        glfwSetWindowCloseCallback(handle, close_requested_event);
        glfwSetWindowFocusCallback(handle, focused_event);
        glfwSetWindowSizeCallback(handle, resized_event);
        glfwSetWindowPosCallback(handle, repositioned_event);
        glfwSetWindowRefreshCallback(handle, redraw_event);
        glfwSetWindowIconifyCallback(handle, minimized_restored_event);
        glfwSetWindowMaximizeCallback(handle, maximized_restored_event);
        glfwSetFramebufferSizeCallback(handle, framebuffer_resize_event);

        glfwSetKeyCallback(handle, key_event);
        glfwSetCharCallback(handle, char_event);
        glfwSetMouseButtonCallback(handle, mouse_button_event);
        glfwSetScrollCallback(handle, mouse_scroll_event);
        glfwSetCursorPosCallback(handle, mouse_position_event);
        glfwSetJoystickCallback(joystick_connection_event);

        return status_type::SUCCESS;
    }

    status glfw_window::status_from_last_glfw_error()
    {
        ZoneScoped;
        const char* description;
        const i32 error_code = glfwGetError(&description);
        if (error_code == GLFW_NO_ERROR) return status_type::SUCCESS;
        return status {status_type::BACKEND_ERROR, std::string(description)};
    }

    std::vector<GLFWmonitor*> glfw_window::get_monitors()
    {
        ZoneScoped;
        std::vector<GLFWmonitor*> monitors;
        int count;
        GLFWmonitor** pointer = glfwGetMonitors(&count);
        monitors.reserve(count);
        for (int i = 0; i < count; i++)
        {
            monitors.push_back(pointer[i]);
        }
        return monitors;
    }

    display_info glfw_window::get_display_info(GLFWmonitor* monitor, const u32 monitor_idx)
    {
        ZoneScoped;
        i32 x, y, width, height;
        glfwGetMonitorWorkarea(monitor, &x, &y, &width, &height);
        const char* display_name = glfwGetMonitorName(monitor);
        return display_info {std::string(display_name), monitor_idx, static_cast<u32>(width), static_cast<u32>(height), x, y};
    }

    void glfw_window::poll_controllers()
    {
        ZoneScoped;

        static std::array<std::unordered_map<u32, bool>, GLFW_JOYSTICK_LAST + 1> previous_button_states;

        for (i32 joystick_idx = 0; joystick_idx < 16; joystick_idx++)
        {
            if (!glfwJoystickPresent(joystick_idx))
            {
                previous_button_states[joystick_idx].clear();
                continue;
            }

            const bool did_just_connect = previous_button_states[joystick_idx].empty();

            GLFWgamepadstate gamepad_state;
            if (glfwJoystickIsGamepad(joystick_idx) && glfwGetGamepadState(joystick_idx, &gamepad_state) == GLFW_TRUE)
            {
                for (i32 axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; axis++)
                {
                    collected_events.push_back(input_event
                        {
                            input_device_type::controller, input_event_type::value_change, joystick_idx, nullptr,
                            controller_axis_to_input_control(axis),
                            controller_axis_to_input_control_type(axis),
                            gamepad_state.axes[axis]
                        }
                    );
                }

                for (i32 button_idx = 0; button_idx <= GLFW_GAMEPAD_BUTTON_LAST; button_idx++)
                {
                    const bool pressed = gamepad_state.buttons[button_idx] == GLFW_PRESS;
                    const bool was_pressed = previous_button_states[joystick_idx][button_idx];
                    if (pressed == was_pressed && !did_just_connect) continue;

                    collected_events.push_back(input_event
                        {
                            input_device_type::controller, input_event_type::value_change, joystick_idx, nullptr,
                            controller_button_to_input_control(button_idx),
                            controller_button_to_input_control_type(button_idx),
                            pressed ? 1.0f : -1.0f,
                        }
                    );
                }
            }
            else
            {
                i32 buttons_count;
                const unsigned char* buttons = glfwGetJoystickButtons(joystick_idx, &buttons_count);
                i32 axes_count;
                const float* axes = glfwGetJoystickAxes(joystick_idx, &axes_count);

                for (u32 button_idx = 0; button_idx < buttons_count; button_idx++)
                {
                    const bool pressed = buttons[button_idx] == GLFW_PRESS;
                    const bool was_pressed = previous_button_states[joystick_idx][button_idx];
                    if (pressed == was_pressed && !did_just_connect) continue;

                    collected_events.push_back(input_event
                        {
                            input_device_type::controller, input_event_type::value_change, joystick_idx, nullptr,
                            button_idx,
                            input_control_type::unknown,
                            pressed ? 1.0f : -1.0f,
                        }
                    );
                }

                for (u32 axis = 0; axis < axes_count; axis++)
                {
                    collected_events.push_back(input_event
                        {
                            input_device_type::controller, input_event_type::value_change, joystick_idx, nullptr,
                            axis,
                            input_control_type::unknown,
                            axes[axis]
                        }
                    );
                }
            }
        }
    }

    void glfw_window::init_input_state()
    {
        ZoneScoped;
        collected_events.push_back(input_event {input_device_type::keyboard, input_event_type::device_connect, 0, "Default Keyboard"});
        collected_events.push_back(input_event {input_device_type::mouse, input_event_type::device_connect, 0, "Default Mouse"});

        for (i32 controller_idx = 0; controller_idx <= GLFW_JOYSTICK_LAST; controller_idx++)
        {
            if (!glfwJoystickPresent(controller_idx)) continue;
            std::string name = get_controller_name(controller_idx);
            collected_events.push_back(input_event {input_device_type::controller, input_event_type::device_connect, controller_idx, name.c_str()});
        }
    }

    void glfw_window::close_requested_event(GLFWwindow* window)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_close_requested != nullptr) _this->callbacks.on_close_requested(_this);
    }

    void glfw_window::resized_event(GLFWwindow* window, const i32 width, const i32 height)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_resized != nullptr) _this->callbacks.on_resized(_this, width, height);
    }

    void glfw_window::repositioned_event(GLFWwindow* window, const i32 x, const i32 y)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_repositioned != nullptr) _this->callbacks.on_repositioned(_this, x, y);
    }

    void glfw_window::minimized_restored_event(GLFWwindow* window, const i32 minimized)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_minimization_change != nullptr) _this->callbacks.on_minimization_change(_this, minimized);
    }

    void glfw_window::maximized_restored_event(GLFWwindow* window, const i32 maximized)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_maximization_change != nullptr) _this->callbacks.on_maximization_change(_this, maximized);
    }

    void glfw_window::focused_event(GLFWwindow* window, const i32 focused)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_focus_change != nullptr) _this->callbacks.on_focus_change(_this, focused);
    }

    void glfw_window::redraw_event(GLFWwindow* window)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle && _this->callbacks.on_redraw != nullptr) _this->callbacks.on_redraw(_this);
    }

    void glfw_window::framebuffer_resize_event(GLFWwindow* window, const i32 width, const i32 height)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle) _this->callbacks.on_framebuffer_recreate(_this, width, height);
    }

    void glfw_window::key_event(GLFWwindow* window, const i32 key, const i32 scancode, const i32 action, const i32 mods)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    _this->collected_events.push_back(input_event {input_device_type::keyboard, input_event_type::value_change, 0, nullptr, scancode_to_input_control(scancode), keyname_to_input_control_type(key), 1.0});
                    break;
                }
                case GLFW_RELEASE:
                {
                    _this->collected_events.push_back(input_event {input_device_type::keyboard, input_event_type::value_change, 0, nullptr, scancode_to_input_control(scancode), keyname_to_input_control_type(key), -1.0});
                    break;
                }
                default: break;
            }
        }
    }

    void glfw_window::char_event(GLFWwindow* window, const u32 codepoint)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle) _this->callbacks.on_input_character(_this, codepoint);
    }

    void glfw_window::mouse_button_event(GLFWwindow* window, const i32 button, const i32 action, const i32 mods)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle)
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(button), static_cast<input_control_type>(button), 1.0});
                    break;
                }
                case GLFW_RELEASE:
                {
                    _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(button), static_cast<input_control_type>(button), -1.0});
                    break;
                }
                default: break;
            }
        }
    }

    void glfw_window::mouse_scroll_event(GLFWwindow* window, const f64 x_offset, const f64 y_offset)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle)
        {
            if (x_offset != 0) _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(10), input_control_type::mouse_horizontal_scroll, x_offset});
            if (y_offset != 0) _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(11), input_control_type::mouse_vertical_scroll, y_offset});
        }
    }

    void glfw_window::mouse_position_event(GLFWwindow* window, const f64 x, const f64 y)
    {
        ZoneScoped;
        glfw_window* _this = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
        if (window == _this->handle)
        {
            _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(8), input_control_type::mouse_horizontal_pos, x});
            _this->collected_events.push_back(input_event {input_device_type::mouse, input_event_type::value_change, 0, nullptr, mouse_input_to_input_control(9), input_control_type::mouse_vertical_pos, y});
        }
    }

    void glfw_window::joystick_connection_event(const i32 id, const i32 event)
    {
        ZoneScoped;
        for (glfw_window* window : all_windows)
        {
            std::string name = get_controller_name(id);
            if (event == GLFW_CONNECTED) window->collected_events.push_back(input_event {input_device_type::controller, input_event_type::device_connect, id, name.c_str()});
            else if (event == GLFW_DISCONNECTED) window->collected_events.push_back(input_event {input_device_type::controller, input_event_type::device_disconnect, id});
        }
    }
}

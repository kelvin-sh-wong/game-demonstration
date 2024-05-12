#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <unordered_map>

class Window {
public:
    Window(int width, int height, char const * title);
    Window(Window const &) = delete;
    Window & operator=(Window const &) = delete;
    ~Window() noexcept;

    bool should_close() const;
    void swap_buffers();

    int width() const;
    int height() const;

    std::unordered_map<int, bool> const & keys() const;

private:
    static void set_window_key(GLFWwindow * window, int key, int scancode, int action, int mods);

    void set_key(int key, int scancode, int action, int mods);

    GLFWwindow * window_;

    std::unordered_map<int, bool> keys_;
};

#endif  // WINDOW_HPP

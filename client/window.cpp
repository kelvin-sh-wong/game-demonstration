#include "window.hpp"

#define GLFW_INCLUDE_NONE

#include "glad_error.hpp"
#include "glfw_error.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <unordered_map>
#include <utility>

Window::Window(int width, int height, char const * title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if ((window_ = glfwCreateWindow(width, height, title, nullptr, nullptr)) == nullptr) {
        throw GLFWError{"Failed to create a GLFW window.", glfwGetError(nullptr)};
    }

    glfwMakeContextCurrent(window_);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        throw GLADError{"Failed to load OpenGL functions."};
    }

    glfwSwapInterval(1);

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, set_window_key);
}

Window::~Window() noexcept {
    glfwDestroyWindow(window_);
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window_);
}

void Window::swap_buffers() {
    glfwSwapBuffers(window_);
}

int Window::width() const {
    int width;
    glfwGetFramebufferSize(window_, &width, nullptr);

    return width;
}

int Window::height() const {
    int height;
    glfwGetFramebufferSize(window_, nullptr, &height);

    return height;
}

std::unordered_map<int, bool> const & Window::keys() const {
    return keys_;
}

void Window::set_window_key(GLFWwindow * window, int key, int scancode, int action, int mods) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->set_key(key, scancode, action, mods);
}

void Window::set_key(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_UNKNOWN) {
        return;
    }

    if (action == GLFW_PRESS) {
        keys_.insert(std::make_pair(key, true));
    }
    else if (action == GLFW_RELEASE) {
        keys_.erase(key);
    }
}

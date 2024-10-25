#pragma once

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "order_book.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

namespace renderer {
extern GLFWwindow* window;
void init();
bool should_close();
void render_state(OrderBook* orderbook);
};  // namespace renderer
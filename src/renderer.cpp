#include <limits>

#include "imgui.h"
#include "imgui_internal.h"
#include "order.h"
#ifdef VISUAL
#include <cstdio>
#include <iostream>

#include "imgui_impl_glfw.h"
#include "renderer.h"

#define MAX_ORDERS 100

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace renderer {
GLFWwindow* window = nullptr;

void init() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) exit(1);

  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac

  // Create window with graphics context
  // full screen
  window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example",
                            nullptr, nullptr);
  if (window == nullptr) exit(1);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImFontConfig geist_config;
  geist_config.OversampleH = 5;
  geist_config.OversampleV = 5;
  ImFont* geist = io.Fonts->AddFontFromFileTTF("./fonts/GeistMono-Regular.ttf",
                                               18.0f, &geist_config);
  IM_ASSERT(geist != nullptr);
}

bool should_close() { return glfwWindowShouldClose(window); }

void render_state(OrderBook* orderbook) {
  using namespace ImGui;

  static ImVec4 clear_color = ImVec4(0.1, 0.1, 0.1f, 1.00f);
  static bool dark_mode = true;

  // Trade input
  static Side side = Side::BID;
  static int price = 100;
  static int quantity = 100;
  price = std::max(0, price);
  quantity = std::max(0, quantity);

  // Sizing information
  float width = GetIO().DisplaySize.x;
  float height = GetIO().DisplaySize.y;
  const auto window_flags = ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  float sidebar_width = std::min(0.3 * width, 500.0);

  glfwPollEvents();
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
    ImGui_ImplGlfw_Sleep(10);
    return;
  }

  // Create new frame
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
  }

  Begin("Config", nullptr, window_flags);
  {
    SetWindowSize(ImVec2(sidebar_width, height / 4));
    SetWindowPos(ImVec2(0, 3 * height / 4));

    if (dark_mode) {
      if (Button("Toggle Light Mode")) {
        clear_color = ImVec4(0.1, 0.1, 0.1f, 1.00f);
        ImGui::StyleColorsDark();
        dark_mode = false;
      }
    } else {
      if (Button("Toggle Dark Mode")) {
        clear_color = ImVec4(0.9, 0.9, 0.9f, 1.00f);
        ImGui::StyleColorsLight();
        dark_mode = true;
      }
    }

    Text("FPS: %zu", (size_t)GetIO().Framerate);
  }
  End();

  Begin("Price Graph", nullptr, window_flags);
  {
    SetWindowSize(ImVec2(width - sidebar_width, height / 2));
    SetWindowPos(ImVec2(sidebar_width, 0));
    Text("TODO: Graph");
  }
  End();

  Begin("Depth Chart", nullptr, window_flags);
  {
    SetWindowSize(ImVec2(width - sidebar_width, height / 2));
    SetWindowPos(ImVec2(sidebar_width, height / 2));
    Text("TODO: Graph");
  }
  End();

  Begin("Orderbook", nullptr, window_flags);
  {
    SetWindowSize(ImVec2(sidebar_width, 3 * height / 4));
    SetWindowPos(ImVec2(0, 0));

    Text("Select an action:");

    // Buy/Sell radio buttons
    if (RadioButton("Bid", side == Side::BID)) {
      side = Side::BID;
    }
    SameLine();
    if (RadioButton("ASK", side == Side::ASK)) {
      side = Side::ASK;
    }

    // Price and quantity input
    InputInt("Price", &price);
    InputInt("Quantity", &quantity);

    // Submit a trade button
    if (Button("Submit")) {
      orderbook->add(price, quantity, side == 0 ? BID : ASK);
    }

    Separator();

    auto table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    Columns(2);

    TextColored(ImVec4(1, 0, 0, 1), "Bids");
    if (BeginTable("bids", 2, table_flags)) {
      TableSetupColumn("Price");
      TableSetupColumn("Quantity");
      TableHeadersRow();
      for (size_t i = 0;
           i < std::min((size_t)MAX_ORDERS, orderbook->bid_prices.size());
           i++) {
        PushID(i);
        TableNextRow();
        size_t index = orderbook->bid_prices.size() - i - 1;
        auto bid = orderbook->bid_prices.data[index];

        size_t quantity = 0;
        for (auto& order : *bid.second) {
          quantity += order->quantity;
        }

        TableSetColumnIndex(0);

        ImGuiSelectableFlags selectable_flags =
            ImGuiSelectableFlags_SpanAllColumns |
            ImGuiSelectableFlags_AllowOverlap;

        char label[64];
        snprintf(label, 64, "%zu", bid.first);
        if (Selectable(label, false, selectable_flags)) {
          price = bid.first;
          side = Side::BID;
        }

        TableSetColumnIndex(1);
        Text("%zu", quantity);

        PopID();
      }

      EndTable();
    }

    NextColumn();

    TextColored(ImVec4(0, 1, 0, 1), "Asks");
    if (BeginTable("asks", 2, table_flags)) {
      TableSetupColumn("Price");
      TableSetupColumn("Quantity");
      TableHeadersRow();
      for (size_t i = 0;
           i < std::min((size_t)MAX_ORDERS, orderbook->ask_prices.size());
           i++) {
        PushID(i);
        TableNextRow();
        size_t index = orderbook->ask_prices.size() - i - 1;
        auto ask = orderbook->ask_prices.data[index];

        size_t quantity = 0;
        for (auto& order : *ask.second) {
          quantity += order->quantity;
        }

        TableSetColumnIndex(0);

        ImGuiSelectableFlags selectable_flags =
            ImGuiSelectableFlags_SpanAllColumns |
            ImGuiSelectableFlags_AllowOverlap;

        char label[64];
        snprintf(label, 64, "%zu", ask.first);
        if (Selectable(label, false, selectable_flags)) {
          price = ask.first;
          side = Side::ASK;
        }

        TableSetColumnIndex(1);
        Text("%zu", quantity);

        PopID();
      }

      EndTable();
    }

    Columns(1);
  }
  End();

  // Rendering
  {
    Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
}

}  // namespace renderer

#endif

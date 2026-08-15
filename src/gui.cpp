#include "gui.h"
#include "Font.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "state_manager.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <ctime>
#include <stdio.h>

#define Clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

struct GuiInfo {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event *event;

  int window_width;
  int window_height;
};

GuiInfo *Create_Gui_Info(SDL_Window *window, SDL_Renderer *renderer,
                         SDL_Event *event) {

  // Initalizing random seed
  std::srand(std::time(0));

  GuiInfo *gi = new GuiInfo{};
  gi->window = window;
  gi->renderer = renderer;
  gi->event = event;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.IniFilename = nullptr;               // Disable .ini files
  io.Fonts->AddFontFromMemoryCompressedTTF(
      FontData_compressed_data, sizeof(FontData_compressed_data), 18.0f);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale); // Bake a fixed style scale.
                                   // (until we have a solution for
                                   // dynamic style scaling,
                                   // changing this requires
                                   // resetting Style + calling
                                   // this again)
  style.FontScaleDpi = main_scale; // Set initial font scale. (in
                                   // docking branch: using
                                   // io.ConfigDpiScaleFonts=true
                                   // automatically overrides this
                                   // for every window depending on
                                   // the current monitor)

  SDL_GetWindowSize(window, &gi->window_width, &gi->window_height);
  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  return gi;
}

void Process_Gui_Event(GuiInfo *gi) { ImGui_ImplSDL3_ProcessEvent(gi->event); }

void Draw_Gui_Frame(State *state, GuiInfo *gi) {

  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  {

    ImGuiIO &io = ImGui::GetIO();
    static float f = 0.0f;
    static int counter = 0;

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 1. Define location and size
    ImVec2 size = ImVec2(GUI_WIDTH, gi->window_height);
    ImVec2 pos = ImVec2(gi->window_width - (size.x + 2), 2);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse;

    // To center the text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Maze Controls", nullptr, window_flags);
    ImGui::SetWindowFontScale(1.3f);
    ImGui::PopStyleVar();
  }

  ImGui::Spacing();
  //===============SECTION 1: Maze Configuration===============
  {
    if (ImGui::BeginChild("ConfigSection", ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders |
                              ImGuiChildFlags_AutoResizeY)) {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 150, 200, 255));
      ImGui::Text("CONFIGURATION");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      static int rows = DEFAULT_ROWS;
      static int columns = DEFAULT_COLUMNS;
      static int start_pos[2] = {0, 0};
      static int end_pos[2] = {DEFAULT_ROWS - 1, DEFAULT_COLUMNS - 1};

      if (ImGui::BeginTable("ConfigTable", 2,
                            ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed,
                                80.0f);
        ImGui::TableSetupColumn("Widget", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Rows");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::InputInt("##Rows", &rows, 1, 5)) {
          rows = Clamp(rows, MIN_ROWS, MAX_ROWS);
          end_pos[0] = rows - 1;
          printf("Rows Changed. Passing in: %d\n", rows);
          State_Set_MazeDimensions(state, rows, columns);
          State_Set_MazeEndpoints(state, start_pos[0], start_pos[1], end_pos[0],
                                  end_pos[1]);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Columns");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::InputInt("##Columns", &columns, 1, 5)) {
          columns = Clamp(columns, MIN_COLUMNS, MAX_COLUMNS);
          end_pos[1] = columns - 1;
          State_Set_MazeDimensions(state, rows, columns);
          State_Set_MazeEndpoints(state, start_pos[0], start_pos[1], end_pos[0],
                                  end_pos[1]);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Start Pos");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::DragInt2("##StartPos", start_pos, 1.0f, 0, 100)) {
          start_pos[0] = Clamp(start_pos[0], 0, rows - 1);
          start_pos[1] = Clamp(start_pos[1], 0, columns - 1);
          State_Set_MazeEndpoints(state, start_pos[0], start_pos[1], end_pos[0],
                                  end_pos[1]);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("End Pos");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::DragInt2("##EndPos", end_pos, 1.0f, 0, 100)) {
          end_pos[0] = Clamp(end_pos[0], 0, rows - 1);
          end_pos[1] = Clamp(end_pos[1], 0, columns - 1);
          State_Set_MazeEndpoints(state, start_pos[0], start_pos[1], end_pos[0],
                                  end_pos[1]);
        }

        ImGui::EndTable();
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
      if (ImGui::Button("Randomize Start & End", ImVec2(-FLT_MIN, 30.0f))) {
        int max_row = rows;
        int max_col = columns;
        start_pos[0] = std::rand() % (max_row);
        start_pos[1] = std::rand() % (max_col);
        end_pos[0] = std::rand() % (max_row);
        end_pos[0] = std::rand() % (max_col);

        State_Set_MazeEndpoints(state, start_pos[0], start_pos[1], end_pos[0],
                                end_pos[1]);
      }
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();
  }

  //===========================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Gap between boxes

  //===============SECTION 2: Animation Settings================
  {
    if (ImGui::BeginChild("AnimSection", ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders |
                              ImGuiChildFlags_AutoResizeY)) {

      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 150, 200, 255));
      ImGui::Text("ANIMATION");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      static bool isAnimated = false;
      static float speedVal = 1.0f;

      if (ImGui::BeginTable("AnimTable", 2,
                            ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed,
                                80.0f);
        ImGui::TableSetupColumn("Widget", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Animate");

        ImGui::TableSetColumnIndex(1);
        ImGui::Checkbox("##AnimateSteps", &isAnimated);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Speed");

        ImGui::TableSetColumnIndex(1);
        ImGui::BeginDisabled(!isAnimated);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##Speed", &speedVal, 0.1f, 5.0f, "%.1fx");
        ImGui::EndDisabled();

        ImGui::EndTable();
      }
    }
    ImGui::EndChild();
  }
  //=============================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f));

  //===============SECTION 3: Maze Generation================
  {
    if (ImGui::BeginChild("GenSection", ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders |
                              ImGuiChildFlags_AutoResizeY)) {

      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 150, 200, 255));
      ImGui::Text("GENERATION");
      ImGui::PopStyleColor();

      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      const char *genAlgorithms[] = {"Randomized DFS", "Randomized Prim's",
                                     "Kruskal's Algorithm"};
      static int currentGenAlgo = 0;

      ImGui::SetNextItemWidth(-FLT_MIN);
      if (ImGui::BeginCombo("##GenAlgorithm", genAlgorithms[currentGenAlgo])) {
        for (int i = 0; i < IM_ARRAYSIZE(genAlgorithms); i++) {
          const bool isSelected = (currentGenAlgo == i);
          if (ImGui::Selectable(genAlgorithms[i], isSelected)) {
            currentGenAlgo = i;
          }
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      float availableWidth = ImGui::GetContentRegionAvail().x;
      float buttonWidth =
          (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
      ImVec2 btnSize(buttonWidth, 30);

      if (ImGui::Button("Generate Maze", btnSize)) {
        // Trigger maze generation
      }

      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button,
                            (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
      if (ImGui::Button("Reset", btnSize)) {
        // Reset logic
      }
      ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();
  }
  //=============================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f));

  //===============SECTION 4: Maze Solving================
  {
    if (ImGui::BeginChild("SolveSection", ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders |
                              ImGuiChildFlags_AutoResizeY)) {

      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 150, 200, 255));
      ImGui::Text("SOLVING");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      const char *solveAlgorithms[] = {"Depth First Search",
                                       "Breadth First Search", "A Star"};
      static int currentSolveAlgo = 0;

      ImGui::SetNextItemWidth(-FLT_MIN);
      if (ImGui::BeginCombo("##SolveAlgorithm",
                            solveAlgorithms[currentSolveAlgo])) {
        for (int i = 0; i < IM_ARRAYSIZE(solveAlgorithms); i++) {
          const bool isSelected = (currentSolveAlgo == i);
          if (ImGui::Selectable(solveAlgorithms[i], isSelected)) {
            currentSolveAlgo = i;
          }
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      float availableWidth = ImGui::GetContentRegionAvail().x;
      float buttonWidth =
          (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
      ImVec2 btnSize(buttonWidth, 30);

      bool isLightMode = ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x > 0.5f;

      ImGui::PushStyleColor(ImGuiCol_Button,
                            (ImVec4)ImColor::HSV(0.38f, 0.56f, 0.45f));

      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            (ImVec4)ImColor::HSV(0.38f, 0.65f, 0.55f));

      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            (ImVec4)ImColor::HSV(0.38f, 0.70f, 0.65f));

      if (ImGui::Button("Solve Maze", btnSize)) {
        // Trigger solve logic
      }
      ImGui::PopStyleColor(3);

      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button,
                            (ImVec4)ImColor::HSV(0.08f, 0.65f, 0.50f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            (ImVec4)ImColor::HSV(0.08f, 0.75f, 0.60f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            (ImVec4)ImColor::HSV(0.08f, 0.85f, 0.70f));
      if (ImGui::Button("Clear Soln", btnSize)) {
        // Clear logic
      }
      ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();
  }
  //=============================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f));

  //===============SECTION 5: Light Mode Dark Mode================
  {
    bool static isDark = true;

    ImGui::SetWindowFontScale(0.89f);
    const char *text =
        isDark ? "Switch to Light Theme" : "Switch to Dark Theme";
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(text).x;
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

    float buttonWidth = ImGui::CalcTextSize(text).x + framePadding.x * 2.0f;

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    if (ImGui::Button(text, ImVec2(buttonWidth, 0.0f))) {
      isDark = !isDark;
    }
    ImGui::PopStyleVar();
  }

  ImGui::SetWindowFontScale(1.3f);
  //===============================================================

  //
  ImGui::End();
}

void Render_Gui_Frame(GuiInfo *gi) {
  ImGui::Render();

  ImGuiIO &io = ImGui::GetIO();

  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), gi->renderer);
}

void Destroy_Gui(GuiInfo *gi) {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  delete gi;
}

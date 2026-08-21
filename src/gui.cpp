#include "gui.h"
#include "Font.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "state_manager.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

#define Clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

void Push_ButtonStyle_Color(float hue, bool isDark) {
  if (!isDark) {
    ImGui::PushStyleColor(ImGuiCol_Button,
                          (ImVec4)ImColor::HSV(hue, 0.70f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          (ImVec4)ImColor::HSV(hue, 0.80f, 0.65f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          (ImVec4)ImColor::HSV(hue, 0.90f, 0.55f));
  } else {
    ImGui::PushStyleColor(ImGuiCol_Button,
                          (ImVec4)ImColor::HSV(hue, 0.56f, 0.45f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          (ImVec4)ImColor::HSV(hue, 0.65f, 0.55f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          (ImVec4)ImColor::HSV(hue, 0.70f, 0.65f));
  }
}

void Push_HeadingStyle_Color(bool isDark) {
  if (isDark) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 150, 200, 255));
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(45, 95, 150, 255));
  }
}

void GUI_Init(SDL_Window *window, SDL_Renderer *renderer) {

  // Initalizing random seed
  std::srand(std::time(0));

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

  // SDL_GetWindowSize(window, &gi->window_width, &gi->window_height);
  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
}

void GUI_Process_Event(SDL_Event *event) { ImGui_ImplSDL3_ProcessEvent(event); }

void GUI_Draw_Frame(MazeUIState *ui, MazeContext *ctx) {

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

    // // 1. Define location and size
    // ImVec2 size = ImVec2(GUI_WIDTH, window_height);
    // ImVec2 pos = ImVec2(window_width - (size.x + 2), 2);

    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImVec2 size = ImVec2(GUI_WIDTH, viewport->WorkSize.y);
    ImVec2 pos =
        ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - (size.x + 2),
               viewport->WorkPos.y + 2);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

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
      Push_HeadingStyle_Color(ui->dark_mode);
      ImGui::Text("CONFIGURATION");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

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

        if (ImGui::InputInt("##Rows", &ui->rows, 1, 5)) {
          ui->rows = Clamp(ui->rows, MIN_ROWS, MAX_ROWS);
          ui->end_pos[0] = ui->rows - 1;
          MazeContext_Event_SetMazeDimensions(ctx);
          MazeContext_Event_SetMazeEndpoints(ctx);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Columns");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::InputInt("##Columns", &ui->columns, 1, 5)) {
          ui->columns = Clamp(ui->columns, MIN_COLUMNS, MAX_COLUMNS);
          ui->end_pos[1] = ui->columns - 1;
          MazeContext_Event_SetMazeDimensions(ctx);
          MazeContext_Event_SetMazeEndpoints(ctx);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Start Pos");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::DragInt2("##StartPos", ui->start_pos, 1.0f, 0, 100)) {
          ui->start_pos[0] = Clamp(ui->start_pos[0], 0, ui->rows - 1);
          ui->start_pos[1] = Clamp(ui->start_pos[1], 0, ui->columns - 1);
          MazeContext_Event_SetMazeEndpoints(ctx);
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("End Pos");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::DragInt2("##EndPos", ui->end_pos, 1.0f, 0, 100)) {
          ui->end_pos[0] = Clamp(ui->end_pos[0], 0, ui->rows - 1);
          ui->end_pos[1] = Clamp(ui->end_pos[1], 0, ui->columns - 1);
          MazeContext_Event_SetMazeEndpoints(ctx);
        }

        ImGui::EndTable();
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
      if (ImGui::Button("Randomize Start & End", ImVec2(-FLT_MIN, 30.0f))) {
        int max_row = ui->rows;
        int max_col = ui->columns;
        ui->start_pos[0] = std::rand() % (max_row);
        ui->start_pos[1] = std::rand() % (max_col);
        ui->end_pos[0] = std::rand() % (max_row);
        ui->end_pos[1] = std::rand() % (max_col);

        MazeContext_Event_SetMazeEndpoints(ctx);
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

      Push_HeadingStyle_Color(ui->dark_mode);
      ImGui::Text("ANIMATION");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
        ImGui::Checkbox("##AnimateSteps", &ui->animate);

        ImGui::SameLine(0.0f, 15.0f);
        ImGui::BeginDisabled(!(ui->maze_mode == MAZE_SOLVING ||
                               ui->maze_mode == MAZE_GENERATING));
        if (ImGui::Button("Skip Animation")) {
          MazeContext_Event_SkipAnimation(ctx);
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!ui->animate);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Speed");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::SliderFloat("##Speed", &ui->speed, 0.1f, 5.0f, "%.1fx")) {

          // Exponential Decay
          if (ui->speed <= 1.0f) {

            float t = (ui->speed - 0.1f) / 0.9f; // Linear Mapping from 0 to 1

            ui->time_delay =
                500.0f * std::pow(0.2f, t); // 0.2 = MaxDelay / Min Delay

          } else {

            float t = (ui->speed - 1.0f) / 4.0f;
            ui->time_delay = 100.0f * std::pow(0.01f, t); // (1 / 100 = 0.01)
          }
        }

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

      Push_HeadingStyle_Color(ui->dark_mode);
      ImGui::Text("GENERATION");
      ImGui::PopStyleColor();

      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      ImGui::SetNextItemWidth(-FLT_MIN);

      int gen_algo = ui->gen_algo;
      if (ImGui::Combo("##GenAlgorithm", &gen_algo, ui->gen_algo_names,
                       IM_ARRAYSIZE(ui->gen_algo_names))) {
        ui->gen_algo = static_cast<GenAlgo>(gen_algo);
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      float availableWidth = ImGui::GetContentRegionAvail().x;
      float buttonWidth =
          (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
      ImVec2 btnSize(buttonWidth, 30);

      if (ImGui::Button("Generate Maze", btnSize)) {
        // Trigger maze generation
        MazeContext_Event_GenerateMaze(ctx);
      }

      ImGui::SameLine();

      Push_ButtonStyle_Color(0.0f, ui->dark_mode);
      if (ImGui::Button("Reset", btnSize)) {
        MazeContext_Event_ResetMaze(ctx);
      }
      ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();
  }
  //=============================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f));

  //===============SECTION 4: Maze Solving================
  {
    ImGui::BeginDisabled(ui->maze_mode == MAZE_BLANK ||
                         ui->maze_mode == MAZE_GENERATING);
    if (ImGui::BeginChild("SolveSection", ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders |
                              ImGuiChildFlags_AutoResizeY)) {

      Push_HeadingStyle_Color(ui->dark_mode);
      ImGui::Text("SOLVING");
      ImGui::PopStyleColor();
      ImGui::Separator();
      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      const char *solveAlgorithms[] = {"Depth First Search",
                                       "Breadth First Search", "A Star"};
      static int currentSolveAlgo = 0;

      ImGui::SetNextItemWidth(-FLT_MIN);

      int solve_algo = ui->solve_algo;
      if (ImGui::Combo("##SolveAlgorithm", &solve_algo, ui->solve_algo_names,
                       IM_ARRAYSIZE(ui->solve_algo_names))) {
        ui->solve_algo = static_cast<SolveAlgo>(solve_algo);
      }

      ImGui::Dummy(ImVec2(0.0f, 5.0f));

      float availableWidth = ImGui::GetContentRegionAvail().x;
      float buttonWidth =
          (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
      ImVec2 btnSize(buttonWidth, 30);

      bool isLightMode = ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x > 0.5f;

      Push_ButtonStyle_Color(0.38, ui->dark_mode);

      if (ImGui::Button("Solve Maze", btnSize)) {
        MazeContext_Event_SolveMaze(ctx);
      }
      ImGui::PopStyleColor(3);

      ImGui::SameLine();

      ImGui::BeginDisabled(ui->maze_mode != MAZE_SOLVED);

      Push_ButtonStyle_Color(0.08f, ui->dark_mode);
      if (ImGui::Button("Clear Soln", btnSize)) {
        // Clear logic
        MazeContext_Event_ClearSolution(ctx);
      }
      ImGui::PopStyleColor(3);

      ImGui::EndDisabled();
    }
    ImGui::EndChild();
    ImGui::EndDisabled();
  }
  //=============================================================

  ImGui::Dummy(ImVec2(0.0f, 10.0f));

  //===============SECTION 5: Light Mode Dark Mode================
  {

    ImGui::SetWindowFontScale(0.89f);
    const char *text =
        ui->dark_mode ? "Switch to Light Theme" : "Switch to Dark Theme";
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(text).x;
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

    float buttonWidth = ImGui::CalcTextSize(text).x + framePadding.x * 2.0f;

    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    if (ImGui::Button(text, ImVec2(buttonWidth, 0.0f))) {
      // Theme Change Logic
      ui->dark_mode = !ui->dark_mode;
      MazeContext_Event_ChangeTheme(ctx);

      if (ui->dark_mode) {
        ImGui::StyleColorsDark();

      } else {
        ImGui::StyleColorsLight();
      }
    }
    ImGui::PopStyleVar();
  }

  ImGui::SetWindowFontScale(1.3f);
  //===============================================================

  //
  ImGui::End();
}

void GUI_Render_Frame(SDL_Renderer *renderer) {
  ImGui::Render();

  ImGuiIO &io = ImGui::GetIO();

  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void GUI_Deinit() {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}

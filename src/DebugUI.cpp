#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "DebugUI.hpp"
#include "Settings.hpp"

DebugUI::DebugUI(GLFWwindow *window) : _isActive(false)
{
	IMGUI_CHECKVERSION();
	std::cerr << "ImGui version: " << IMGUI_VERSION << std::endl;
	ImGui::CreateContext();
	//ImGuiIO &io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

DebugUI::~DebugUI()
{
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::update()
{
	if (!isActive()) {
		return ;
	}
}

void DebugUI::render()
{
	if (!isActive()) {
		return ;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	setupUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::setupUI()
{
	ImGui::Begin("Debug");
	ImGui::End();
}

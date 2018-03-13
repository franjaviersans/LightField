#include "Menu.h"
#include <string>

Menu::Menu(){

}

Menu::~Menu(){

}

void Menu::init(GLFWwindow * glfwWindow){
	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(glfwWindow, false);
}

void Menu::destroy(){
	ImGui_ImplGlfwGL3_Shutdown();
}


void Menu::captureIO(){
	//check if imgui wants keyboard and mouse
	ImGuiIO& io = ImGui::GetIO();

	//if (io.WantCaptureMouse)	ConfigParameters::Inst().m_imguiCaptureMouse = true;
	//if (io.WantCaptureKeyboard) ConfigParameters::Inst().m_imguiCaptureKeyboard = true;

}

//function to create the GUI menu
void Menu::createGUI()
{

	//Init imgui frame
	ImGui_ImplGlfwGL3_NewFrame();

	if (m_show)
	{

		//Show error message
		static bool show_error = true;
		static std::string error_message = "Error";
		if (!show_error){
			ImGui::OpenPopup("Volume not loaded");
			if (ImGui::BeginPopupModal("Volume not loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(error_message.c_str());
				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_error = true; }
				ImGui::EndPopup();
			}
		}


		//main window
		{
			static float f = 0.0f;
			ImGui::Begin("Main menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
			
			//GUI to load volume
			if (ImGui::Button("Button1")) {}
			//GUI to load volume
			if (ImGui::Button("Button2")) {}

			//menu for saving transfer function
			if (ImGui::Button("Button3")) {}
			



			if (ImGui::Button("show another")) m_show_test_window ^= 1;

			//distance using GPU or CPU
			static int gpu_cpu4 = 0;
			ImGui::Text("Radio Button");
			ImGui::RadioButton("Option1", &gpu_cpu4, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Option2", &gpu_cpu4, 1);
			ImGui::End();
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (m_show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&m_show_test_window);
		}
	}
}

void Menu::loadVolume(){
	
}

void Menu::render(){
	//render imgui interface
	ImGui::Render();
}

void Menu::show(){
	m_show = !m_show;
}

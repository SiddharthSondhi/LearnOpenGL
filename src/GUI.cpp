#include "GUI.h"

namespace GUI {

	void initGUI(GLFWwindow* window){
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Style
		ImGui::StyleColorsDark();

		// Initialize ImGui for GLFW + OpenGL
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void setUpGUI(GUISettings& settings) {

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin(" ");

		// specify UI elements
		ImGui::Text("Frame time: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		if (settings.postProcessingModes)
			ImGui::Combo("Post-Processing Mode", &settings.postProcessingMode, settings.postProcessingModes, settings.numPostProcessingModes);
		ImGui::SliderFloat("1 / offset", &settings.convMatrixOffset, 1.0f, 5000.0f);
		
		if (settings.skyboxOptions)
			ImGui::Combo("SkyBox Texture", &settings.skyboxTextureIndex, settings.skyboxOptions, settings.numSkyBoxOptions);

		ImGui::End();
	}

	void shutDownGUI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};
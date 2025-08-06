#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI {
	struct GUISettings {
		// post processing options
		int numPostProcessingModes = 7;
		const char* postProcessingModes[7] = {"Regular", "Inverse", "Grey Scale", "Weighted Grey Scale", "Sharpen", "Emboss", "Test"};
		int postProcessingMode = 0;
		float convMatrixOffset = 500.0f;

		// skybox options
		int numSkyBoxOptions = 0;
		const char** skyboxOptions = nullptr;
		int skyboxTextureIndex = 0;
	};

	void initGUI(GLFWwindow* window);
	void setUpGUI(GUISettings& gui);
	void shutDownGUI();

};
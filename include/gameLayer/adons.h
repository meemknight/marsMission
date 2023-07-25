#pragma once
#include "imgui.h"

bool palettePanel(
	const ImVec4 *colors, int nColors, const ImVec2 &colorButtonSize,
	int *selectedPalIdx, int highlightPalIdx = -1, ImGuiWindowFlags flags = 0u);

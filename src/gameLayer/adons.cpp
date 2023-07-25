#include <adons.h>
#include "imgui.h"
#include <string>

bool palettePanel(const ImVec4 *colors, int nColors, const ImVec2 &colorButtonSize, 
	int *selectedPalIdx, int highlightPalIdx, ImGuiWindowFlags flags)
{
	const ImVec2 &pos = ImGui::GetCursorScreenPos();
	bool colorHovered = false;
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	const ImDrawListFlags backupFlags = drawList->Flags;
	drawList->Flags &= ~ImDrawListFlags_AntiAliasedLines;

	const ImU32 redColor = ImGui::GetColorU32(ImVec4(1,0,0,1));
	const ImU32 yellowColor = ImGui::GetColorU32(ImVec4(1, 1, 0, 1));
	const ImU32 darkRedColor = ImGui::GetColorU32(ImVec4(0.5, 0, 0, 1));

	const float windowWidth = ImGui::GetWindowContentRegionMax().x;
	ImVec2 globalCursorPos = ImGui::GetCursorScreenPos();
	for (int palIdx = 0; palIdx < nColors; ++palIdx)
	{
		const float borderWidth = 1.0f;
		const ImVec2 v1(globalCursorPos.x + borderWidth, globalCursorPos.y + borderWidth);
		const ImVec2 v2(globalCursorPos.x + colorButtonSize.x, globalCursorPos.y + colorButtonSize.y);

		drawList->AddRectFilled(v1, v2, ImGui::GetColorU32(colors[palIdx]));

		const std::string &id = "##palitem-" + std::to_string(palIdx);
		if (ImGui::InvisibleButton(id.c_str(), colorButtonSize))
		{
			*selectedPalIdx = palIdx;
		}
	#if 0
		const core::String &contextMenuId = core::string::format("Actions##context-palitem-%i", palIdx);
		if (ImGui::BeginPopupContextItem(contextMenuId.c_str()))
		{
			ImGui::EndPopup();
		}
	#endif
		if (!colorHovered && ImGui::IsItemHovered())
		{
			colorHovered = true;
			drawList->AddRect(v1, v2, redColor);
		}
		else if (palIdx == highlightPalIdx)
		{
			drawList->AddRect(v1, v2, yellowColor);
		}
		else if (palIdx == *selectedPalIdx)
		{
			drawList->AddRect(v1, v2, darkRedColor);
		}
		globalCursorPos.x += colorButtonSize.x;
		if (globalCursorPos.x > windowWidth - colorButtonSize.x)
		{
			globalCursorPos.x = pos.x;
			globalCursorPos.y += colorButtonSize.y;
		}
		ImGui::SetCursorScreenPos(globalCursorPos);
	}
	const ImVec2 cursorPos(pos.x, globalCursorPos.y + colorButtonSize.y);
	ImGui::SetCursorScreenPos(cursorPos);

	// restore the draw list flags from above
	drawList->Flags = backupFlags;
	return true;
	
}
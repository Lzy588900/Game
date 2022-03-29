#include "ImWin.h"
//#include "Matrix/Matrix.h"





void DrawOther() {
	//颜色
	ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	const ImU32 col = ImColor(colf);

	//绘制图形
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	draw_list->AddLine(ImVec2(0, 20), ImVec2(500, 500), col, 3.0f);
	//画线
	draw_list->AddLine(ImVec2(0, 20), ImVec2(500, 500), col, 3.0f);
	//画文字
	draw_list->AddText(ImVec2(30, 500), col,u8"游戏内部绘制");
	//画矩形
	draw_list->AddRect(ImVec2(50, 100), ImVec2(150, 230), col, 0.0f, 0, 3.0f);
}
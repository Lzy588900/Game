#include "ImWin.h"
#include "Matrix/Matrix.h"



void LoadMyWin()
{
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin(u8"�ڲ�����");
	ImGui::Text(u8"�ڲ�����");

	DrawOther();//���Ʒ���
	
	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


}

void DrawOther() {
	//��ɫ
	ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	const ImU32 col = ImColor(colf);

	//����ͼ��
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	draw_list->AddLine(ImVec2(0, 20), ImVec2(500, 500), col, 3.0f);
	//����
	draw_list->AddLine(ImVec2(0, 20), ImVec2(500, 500), col, 3.0f);
	//������
	draw_list->AddText(ImVec2(30, 500), col,u8"��Ϸ�ڲ�����");
	//������
	draw_list->AddRect(ImVec2(50, 100), ImVec2(150, 230), col, 0.0f, 0, 3.0f);
}
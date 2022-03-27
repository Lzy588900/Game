#pragma once
#include <windows.h>
constexpr int byte_length = 5;

class inline_hook
{

protected:
private:
	using UCHAR = unsigned char;
	UCHAR m_original_byte[byte_length];//ԭʼ�����Ļ��ָ��
	UCHAR m_self_byte[byte_length];//���ǹ���Ļ��ָ��

	int m_original_address;//ԭʼ������ַ
	int m_self_address;//���Ǻ����ĵ�ַ

	DWORD  motify_memory_attributes(int address, DWORD attributes = PAGE_EXECUTE_READWRITE)
	{
		DWORD old_attributes; 
		VirtualProtect(reinterpret_cast<void*>(address),byte_length, attributes, &old_attributes); //��ȡ��ַ������
		return old_attributes;
	}
public:
	inline_hook(int original_address, int self_address) :m_original_address(original_address), m_self_address(self_address)
	{
		m_self_byte[0] = '\xe9';
		int offset = self_address-(original_address+byte_length);//����ƫ��

		//������ת�����Ǻ�����byte
		memcpy(&m_self_byte[1], &offset, byte_length - 1);

		//�޸��ڴ�����
		DWORD attributes = motify_memory_attributes(original_address);
		//����ԭʼ������ַ��byte
		memcpy(&m_original_byte, reinterpret_cast<void*>(original_address), byte_length);
		//�ָ��ڴ�����
		motify_memory_attributes(original_address, attributes);

	}
	void motify_address()
	{
		DWORD attributes = motify_memory_attributes(m_original_address);//��ȡԭʼ��ַ������
		//д�����ǹ����byteʵ��inlinehook
		memcpy(reinterpret_cast<void*>(m_original_address),m_self_byte,byte_length);//��ԭʼ��ַ�����Ը���m_self_byte

		motify_memory_attributes(m_original_address, attributes);//�޸ĵ�ַ����
	}

	void restore_address()
	{
		DWORD attributes = motify_memory_attributes(m_original_address);

		memcpy(reinterpret_cast<void*>(m_original_address), m_original_byte, byte_length);
		motify_memory_attributes(m_original_address, attributes);
	}
};
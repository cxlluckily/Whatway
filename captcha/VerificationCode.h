#pragma once 
#include <string> 
#include <vector>   
using namespace std; 


class VerificationCode
{ 
public: 
	struct CmdInfo
	{ 
		CmdInfo();
		string m_ImageType;//ͼƬ����
		unsigned int m_Width;//ͼƬ���
		unsigned int m_Heigh;//ͼƬ�߶�
		string m_XcColor;//ͼƬ����ɫ
		string m_Font;//����
		unsigned int m_Pointsize;//��С
		string m_FontColor;//������ɫ
	};
	//�޸���֤��ͼƬ����
	void SetCmdInfo(CmdInfo in_CmdInfo);
	// ������֤��,��ȡͼƬ
	string VerificationCodeToImage(const string& in_VerificationCode	//������֤��
									,string& out_ImageBinaryData		//���������ͼƬ����
									,string& in_ImageType);				//������֤��ͼƬ����
private: 
	CmdInfo m_CmdInfo; //
	// ������֤��
	string GetCmd(string in_VerificationCode);
	//
	void SplitCmd(string in_CmdStr,vector<string>& out_SplitStringVector);
	//�������ʱ�����Ŵ���ո�,��ʾ�ַ�����һ���,�ڷֽ���Convert�����,��ԭ�ո�

	//�� ��  ����ɫ
	string GetSizeXc(unsigned int in_Width,unsigned int in_Heigh,string in_XcColor);
	//ע��        
	string GetAnnotate(string in_Font	//����
						, unsigned int in_Pointsize  //�����С
						, string in_Color		//��ɫ
						,unsigned int in_X_		//�Ƕ�
						,unsigned int in_Y_		//�Ƕ�
						,unsigned int in_X		//X����
						,unsigned int in_Y		//Y����
						,string in_Value);		//��ʾ�ַ�
	//�������
	string GetDrawLine(); 
	//��ȡ�����
	int GetRand(int in_Min,int in_Max); 
	//����ͼƬ// outImageData:���ӵĲ���,��ȡ����������,string& outImageData
	static int ConvertMain(int argc,char **argv,string& outImageData); 
};
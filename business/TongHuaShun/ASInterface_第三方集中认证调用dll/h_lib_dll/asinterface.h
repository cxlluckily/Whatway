#ifndef _ASINTERFACE_API_H
#define _ASINTERFACE_API_H
/** �������������֤���붯̬�� v1.0.1
 *  ��Ȩ���� @ 2009 ���ݺ�����������ɷ����޹�˾
 */

#ifdef __cplusplus
extern "C" 
{
#endif

/**��̬���ʼ������
 * ����˵���� pszLinkStr  �����ַ�����������֤ʹ�ã���������ֵ�����֤��̨ȷ�ϣ���ʱ��������asinterface
 *            pszAuthAddr ��Ҫ���ӵļ�����֤��������ַ������ͬʱ���Ӷ�����Դ����ӣ�ֱ��������Ϊֹ
 *                       ��ʽ��ip:port|ip2:port2...  �磺192.168.0.1:9901|192.168.0.2|9902
 * ����ֵ��  0
 * ���������֮��Ҫ��ʼ��һ�Σ������֤��ַ�ı䣬��ô�������µ��ô˺������Ա����Ӳ�ͬ����֤��ַ
 */
long  WINAPI ASInit(const char* pszLinkStr, const char* pszAuthAddr);


/**����������
 *����˵���� pszRequest    ���������ַ�����Ҫ���ID 35��ʼ������У��λID 10֮�������
 *           pszReturnData ������ŷ������ݵ��ڴ�ռ䣬���ص�����Ҳ�Ǵ�ID 35��ʼ����ID 10������������ID8��9��10������
 *           cbMaxSize     ָ��pszReturnData�����Դ�ŵ����ݳ��ȣ�
 *           nTimeOut      ����ʱʱ�䣬��λ�Ǻ��룬����Ϊ0����һֱ�ȴ�
 *����ֵ��  ����ֵ >= 0 Ϊ�ɹ�����ֵ�����ص����ݳ���
 *          < 0 ʧ��   -1 ������������
                       -2 ����pszRequestΪ���ַ���
					   -3 �������ݳ�ʱ
*/
long WINAPI ASRequest(const char* pszRequest, char* pszReturnData, int cbMaxSize, int nTimeOut);

/**�˳�����
��̬�������ͷŶ�̬��ǰ����øú����ͷ���Դ
*/
void WINAPI ASClose();

/**�˳�����2
��̬�������ͷŶ�̬��ǰ����øú����ͷ���Դ
*/
void WINAPI ASClose2();

#ifdef __cplusplus
}
#endif
#endif

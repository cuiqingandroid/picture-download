#pragma once

// ���º궨��Ҫ������ƽ̨��Ҫ������ƽ̨
// �Ǿ�������Ӧ�ó������蹦�ܵ� Windows��Internet Explorer �Ȳ�Ʒ��
// ����汾��ͨ����ָ���汾�����Ͱ汾��ƽ̨���������п��õĹ��ܣ������
// ����������

// �������Ҫ��Ե�������ָ���汾��ƽ̨�����޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0500           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0500     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif


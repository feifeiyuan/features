һ������
ץȡgpu loading����

����ʹ�÷���
��ͬ������ʹ�÷�����ͬ

1������usbץȡ������Ϣ
˫��gpu_loading.bat�ű����������½���ʱ������Դ���(Ĭ�ϲ���600��)��enter��ʼִ��
please input test times :
ִ�н�����console����,����������ʾ���밴���������. . .����ʱ��ͨ����������˳�

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�gpu_loading.sh�ű�push��/dataĿ¼��
adb root
adb remount
adb push xxxx/gpu_loading.sh(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/

ͨ�����ڹ���ִ�нű�
su root
sh /data/gpu_loading.sh ���Դ���(Ĭ�ϲ���600��)

3��ʾ��
C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>adb root
adbd is already running as root

C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>adb remount
remount succeeded

C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>set dir=C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading\gpu_loading.sh

C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>adb push C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading\gpu_loading.sh /data/
189 KB/s (3022 bytes in 0.015s)

C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>adb shell sh /data/gpu_loading.sh
133: 490     216: 13     320: 97     400: 0     510: 0     560: 0     600: 0

133 = 81.67%  GPU Utilisation: 25.28%
216 = 2.17%  GPU Utilisation: 8.85%
320 = 16.17%  GPU Utilisation: 5.23%

polling_interval:10
Test Times: 600   GPU Utilisation: 21.68
C:\Users\xxx\Desktop\�ű�\contrast_phone\����5\gpu_loading>pause
�밴���������. . .

������ע
1����ǰ����֤platform
����5

2�����������ޣ�loading��Ϣ�ڵ㲢��һ����ȫ��ȷ���ýű�ɨ����������ݽ����ο�







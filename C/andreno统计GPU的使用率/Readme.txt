һ������
ץȡ����5 gpu loading����

����ʹ�÷���
��ͬ������ʹ�÷�����ͬ

1������usbץȡ������Ϣ
˫��gpu_loading.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���ʱ����60s)��enter��ʼִ��
please input test times :
ִ�н�����console����,����������ʾ���밴���������. . .����ʱ��ͨ����������˳�

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�gpu_loading����push��/data/local/tmp/gpu_loading
adb push xxxx/gpu_loading(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/gpu_loading

ͨ�����ڹ������иù���
./data/local/tmp/gpu_loading -t 60 ����ʱ��(Ĭ�ϲ���ʱ����60s)

3��ʾ��
C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>adb shell "rm /data/local/tmp/gpu_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>adb push C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading\gpu_loading    /data/local/tmp/
2242 KB/s (643165 bytes in 0.280s)

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>adb shell "chmod 777 /data/local/tmp/gpu_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>adb shell "./data/local/tmp/gpu_loading -t 60"
accummulate_time is 60(s)
polling interval is 10(ms)
600: 0          560: 0          510: 0          400: 0          320: 0          216: 3492            133: 2508

216: 58.20%      utilisation:47.29%
133: 41.80%      utilisation:48.17%
Averag GPU Utilisation : 47.66%

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\gpu_loading>pause
�밴���������. . .


������ע
1����ǰ����֤platform
����5

2��������user��userdebug�汾

2�����������ޣ�loading��Ϣ�ڵ㲢��һ����ȫ��ȷ���ýű�ɨ����������ݽ����ο�







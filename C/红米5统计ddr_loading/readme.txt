���ܣ��������5 ddr_loading����

1������usbץȡ������Ϣ
˫��ddr_loading.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���60s)��enter��ʼִ��
please input test times(s) :
���ý������������ݽ��

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�sgm�ű�push��/data/local/tmp/Ŀ¼��
adb push xxxx/ddr_loading(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/

ͨ�����ڹ���ִ�нű�
chmod 777 /data/local/tmp/ddr_loading
./data/local/tmp/ddr_loading -t 60

3���ֶ�ץȡddr_loading����
��bat�ű���ȡ�Ļ���������windows������أ���˫��bat�ű�����errorʱ
������������������ֶ�ץȡ����
adb root
adb remount
adb push xxxx/ddr_loading(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/ddr_loading
./data/local/tmp/ddr_loading -t ����ʱ��(Ĭ�ϲ���60s)

4��ʾ������
C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb root
adbd is already running as root

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb remount
remount succeeded

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb shell "rm /data/local/tmp/ddr_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb push C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm\ddr_loading  /data/local/tmp/
1129 KB/s (641701 bytes in 0.554s)

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb shell "chmod 777 /data/local/tmp/ddr_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>adb shell "./data/local/tmp/ddr_loading -t 60"
accummulate_time is 60(s)
time_use is 60592.86ms
freq:768, perc:0.50%
freq:767, perc:0.83%
freq:422, perc:1.50%
freq:383, perc:0.17%
freq:278, perc:0.17%
freq:211, perc:96.83%

C:\Users\xxx\Desktop\power_script\contrast_phone\����5\ddr_freq_msm>pause
�밴���������. . .

��ע
1����ǰ�Ѿ���֤��ƽ̨
����5

2��ֻ������userdebug�汾

3���������ʣ����µ�3216��лл

���ܣ�ͳ��һ��ʱ����CPUƽ��ʹ����ǰN�Ľ���

1������usb��ȡ������Ϣ
˫��sgm.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���60s)��enter��ʼִ��
please input test times(s) :
ִ�н�����consule����ʱ������ʾ�Ľ�����(Ĭ����ʾCPU��ƽ��ʹ����ǰ10�Ľ���)��enter��ʼִ��
please input top number :
���ý������������ݽ��

2�����Ӵ���ץȡ
��normal״̬�½�sprd_top�ű�push��/data/local/tmp/Ŀ¼��
adb push xxxx/sprd_top(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/

ͨ�����ڹ���ִ�нű�
chmod 777 /data/local/tmp/sprd_top
./data/local/tmp/sprd_top -t 10 -n 10

3���ֶ�ִ�нű�
����bat�ű��еĻ���������windows������أ�������bat�ű�����ʱ
������ִ�������������ֶ�ִ�нű�
adb push C:\Users\xxx\Desktop\power_script\top\top /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/top
./data/local/tmp/top -t 10 -n 10
-t ����ӵ��ǲ���ʱ��(Ĭ����10s)
-n ����ӵ�����ʾ��CPUƽ��ʹ����ǰN�Ľ���(Ĭ����ǰ10�Ľ���)

4��ʾ������
C:\Users\xxx\Desktop\power_script\top>adb shell "rm /data/local/tmp/top"

C:\Users\xxx\Desktop\power_script\top>adb push C:\Users\xxx\Desktop\power_script\top\top /data/local/tmp/
2163 KB/s (656656 bytes in 0.296s)

C:\Users\xxx\Desktop\power_script\top>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\top>if "" == "" set test_times=10

C:\Users\xxx\Desktop\power_script\top>set /p num=please input top number :
please input top number :

C:\Users\xxx\Desktop\power_script\top>if "" == "" set num=10

C:\Users\xxx\Desktop\power_script\top>adb shell "chmod 777 /data/local/tmp/top"

C:\Users\xxx\Desktop\power_script\top>adb shell "./data/local/tmp/top -t 10 -n 10"
PID     %[CPU]  ARGS
1847    41.9    com.imangi.temp+
 1979   19.3    top -n 1 -d 3
 346    12.9    surfaceflinger
 328    3.2     android.hardwar+
 488    3.2     android.hardwar+
 500    3.2     [kworker/u16:9]
 678    3.2     system_server
 893    3.2     com.android.sys+
 1411   3.2     android.process+
 1954   3.2     com.android.key+


C:\Users\xxx\Desktop\power_script\top>pause
�밴���������. . .

��ע

1��������user�汾��userdebug�汾


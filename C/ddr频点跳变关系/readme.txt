���ܣ�ͳ��һ��ʱ��cpuƵ��任��

1������usbץȡ������Ϣ
˫��ddr_trans_table.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���60s)��enter��ʼִ��
please input test times(s) :
���ý���������ʱ�佫�������ݽ��

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�ddr_trans_table����push��/data/local/tmp/Ŀ¼��
adb push xxxx/ddr_trans_table(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/
ͨ�����ڹ���ִ�нű�
chmod 777 /data/local/tmp/ddr_trans_table
./data/local/tmp/ddr_trans_table -t 60

3���ֶ�ִ�иù���
��bat�ű��еĻ���������window������أ�����bat�ű�����ʱ���ֶ�ִ�����������ȡ���
adb push C:\Users\xxx\Desktop\power_script\cpu\ddr_trans_table\ddr_trans_table  /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/ddr_trans_table
./data/local/tmp/ddr_trans_table -t ����ʱ��(Ĭ�ϲ���60s)

4��ʾ������
C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb root
adbd is already running as root

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb remount
remount succeeded

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb shell "rm /data/local/tmp/ddr_trans_table"

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb push C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table\ddr_trans_table  /data/local/tmp/
2904 KB/s (642971 bytes in 0.216s)

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb shell "chmod 777 /data/local/tmp/ddr_trans_table"

C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>adb shell "./data/local/tmp/ddr_trans_table -t 60"
accumulate_time is 60
counts is 6000
        160     233     311     400     533     622     800     933
160     0       0       0       0       0       0       0       0
233     0       0       0       0       0       0       0       0
311     0       0       3042    994     59      5       0       0
400     0       0       982     645     74      2       0       0
533     0       0       72      62      47      3       1       0
622     0       0       3       3       4       0       0       0
800     0       0       0       0       1       0       0       0
933     0       0       0       0       0       0       0       0


C:\Users\xxx\Desktop\power_script\ddr\ddr_trans_table>pause
�밴���������. . .

��ע
1����ǰ�Ѿ���֤��ƽ̨
sharkle
sharkl3

2��ֻ����userdebug�汾��user�汾��Ȩ��

3���������ʣ����µ�3216��лл

4��ddr��Ƶ��仯��΢���ģ�����ͳ�����ݲ��Ǻ�׼ȷ

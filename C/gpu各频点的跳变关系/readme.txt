���ܣ�ͳ��һ��ʱ��cpuƵ��任��

1������usbץȡ������Ϣ
˫��gpu_trans_table.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���60s)��enter��ʼִ��
please input test times(s) :
���ý���������ʱ�佫�������ݽ��

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�gpu_trans_table����push��/data/local/tmp/Ŀ¼��
adb push xxxx/gpu_trans_table(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/
ͨ�����ڹ���ִ�нű�
chmod 777 /data/local/tmp/gpu_trans_table
./data/local/tmp/gpu_trans_table -t 60

3���ֶ�ִ�иù���
��bat�ű��еĻ���������window������أ�����bat�ű�����ʱ���ֶ�ִ�����������ȡ���
adb push C:\Users\xxx\Desktop\power_script\cpu\gpu_trans_table\gpu_trans_table  /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/gpu_trans_table
./data/local/tmp/gpu_trans_table -t ����ʱ��(Ĭ�ϲ���60s)

4��ʾ������
C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb root
adbd is already running as root

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb remount
remount succeeded

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb shell "rm /data/local/tmp/gpu_trans_table"

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb push C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table\gpu_trans_table  /data/local/tmp/
3031 KB/s (667478 bytes in 0.215s)

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb shell "chmod 777 /data/local/tmp/gpu_trans_table"

C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>adb shell "./data/local/tmp/gpu_trans_table -t 60"
accumulate_time is 60
counts is 3000
        256     384     550
256     2637    0       28
384     16      71      1
550     13      17      216


C:\Users\xxx\Desktop\power_script\gpu\gpu_trans_table>pause
�밴���������. . .

��ע
1����ǰ�Ѿ���֤��ƽ̨
sharkle
sharkl3

2��ֻ����userdebug�汾��user�汾��Ȩ��

3���������ʣ����µ�3216��лл

���ܣ�ͳ��һ��ʱ��cpuƵ��任��

1������usbץȡ������Ϣ
˫��cpu_trans_table.bat�ű����������½���ʱ�������ʱ��(Ĭ�ϲ���60s)��enter��ʼִ��
please input test times(s) :
���ý���������ʱ�佫�������ݽ��

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�cpu_trans_table����push��/data/local/tmp/Ŀ¼��
adb push xxxx/cpu_trans_table(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/
ͨ�����ڹ���ִ�нű�
chmod 777 /data/local/tmp/cpu_trans_table
./data/local/tmp/cpu_trans_table -t 60

3���ֶ�ִ�иù���
��bat�ű��еĻ���������window������أ�����bat�ű�����ʱ���ֶ�ִ�����������ȡ���
adb push C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table\cpu_trans_table  /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/cpu_trans_table
./data/local/tmp/cpu_trans_table -t ����ʱ��(Ĭ�ϲ���60s)

4��ʾ������
C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>adb shell "rm /data/local/tmp/cpu_trans_table"

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>adb push C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table\cpu_trans_table  /data/local/tmp/
2555 KB/s (644844 bytes in 0.246s)

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>adb shell "chmod 777 /data/local/tmp/cpu_trans_table"

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>adb shell "./data/local/tmp/cpu_trans_table -t 60 "
accumulate_time is 60
counts is 3000
        768000  884000  1000000 1100000 1200000
768000  2663    109     19      12      3
884000  108     25      6       0       2
1000000 16      6       0       2       3
1100000 12      0       2       0       1
1200000 7       1       0       1       1

        768000  1050000 1225000 1400000 1500000 1570000
768000  2928    11      5       5       1       0
1050000 5       1       2       8       0       0
1225000 4       2       0       0       1       0
1400000 9       2       0       6       1       1
1500000 2       0       0       0       1       1
1570000 2       0       0       0       0       1

C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table>pause
�밴���������. . .


��ע
1����ǰ�Ѿ���֤��ƽ̨
sharkle
sharkl3

2��������user�汾��userdebug�汾

3���������ʣ����µ�3216��лл

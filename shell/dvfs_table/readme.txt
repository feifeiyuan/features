һ������:�鿴dvfs_table

�����÷�:
1����������������
adb root				
adb remount
adb push �ļ�·��(��ֱ��ͨ������.sh�ļ��������л�ȡ·��) /data/
adb shell
sh /data/lookat_dvfs_table.sh	
����֮�󣬼�����ʾ���

2��ʾ��
Microsoft Windows [�汾 6.1.7601]
��Ȩ���� (c) 2009 Microsoft Corporation����������Ȩ����

C:\Users\xxx>adb push C:\Users\xxx\Desktop\power_script\dvfs_table\lookat_dvfs_table.sh /data/
272 KB/s (4352 bytes in 0.015s)

C:\Users\xxx>adb shell
sp9832e_1h10_go:/ # sh /data/lookat_dvfs_table.sh
sp9832e_1h10_go:/ # sh /data/lookat_dvfs_table.sh
    768000             868
    900000             921
   1100000            1000
   1300000            1081
   1400000            1121
sp9832e_1h10_go:/ #


������ע:

1����Ϊregulator  driver�����Ľڵ�����ÿ����Ŀÿ��board�����ܴ��ڲ���
ʹ��ʱ����ǰȷ�Ͻڵ����ƣ���Ҫʱ��Խű����ʵ��޸�

2��ֻ������userdebug�汾(user�汾Ȩ�޲���)

3�������������µ�3216����л֧�������



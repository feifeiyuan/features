һ������:cpu�Ȳ��

�����÷�:

1����������������
adb root				
adb remount
adb push �ļ�·��(��ֱ��ͨ������.sh�ļ��������л�ȡ·��) /data/
adb shell
����������USB��ǰ����
��������Ҳ����ֱ��ͨ��˫��cpu_hotplug.batʵ��
sh /data/cpu_hotplug.sh	
�ű�����֮�󣬰�����ʾ�����������

2��ʾ��
sp7731e_1h10:/data # sh cpu_hotplug.sh
sp7731e_1h10:/data # sh cpu_hotplug.sh
online_core:    0-3  				: online cpu
offline_core:	     				: offline cpu
please choose 0(off)/1(on)  		: 0���κˣ�/1����ˣ�
0									: ���аκ�
0
please choose operate core 0~3		: ѡ����Ҫ������cpu
2
2
online_core:    0-1,3				: �������
offline_core:   2
sp7731e_1h10:/data #

������ע:
1����ǰ�Ѿ���֤��ƽ̨
sharkle	
iWhale2
pike2
SharkL2
sharkleplus
����Դ���ޣ�δ��֤����ƽ̨����board��ԭ���ϼܹ����䣬����ƽ̨������

2��ֻ������userdebug�汾(user�汾Ȩ�޲���)

3��������������ϵ�µ�3216��лл



һ������:��ʾһ��ʱ��CPU������ж���Ϣ

�����÷�:
1������usbץȡ������Ϣ
interrupts.bat�ű����������½���ʱ������Դ���(Ĭ�ϲ���ʱ����3s)��enter��ʼִ��
please input test times :
�������½���ʱ�����Ƿ���ʾ�����ж���Ϊ0���ж�(Ĭ�ϲ���ʾ)��enter��ʼִ��
please input display zero interrupts(1/0) :0
ִ�н�����consule���棬����������ʾ���밴���������. . .����ʱ��ͨ����������˳�
interrupts.bat��ͬ��Ŀ¼�½����ɶ�Ӧ��interrupts�Ľ���ļ�

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�interrupts.sh�ű�push��/dataĿ¼��
adb root
adb remount
adb push xxxx/interrupts.sh(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/

ͨ�����ڹ���ִ�нű�
su root
sh /data/interrupts.sh ����ʱ��(Ĭ��3s) �Ƿ���ʾ�����ж���Ϊ0���ж�(Ĭ�ϲ���ʾ)

�ű����н�����������������/sdcard/interrupts_result.txt��
adb pull /sdcard/interrupts_result.txt ����·�� 
������ȡ���������

2��ʾ��:
C:\Users\xxx\Desktop\power_script\interrupts>adb root
adbd is already running as root

C:\Users\xxx\Desktop\power_script\interrupts>adb remount
remount succeeded

C:\Users\xxx\Desktop\power_script\interrupts>adb push C:\Users\xxx\Desktop\power_script\interrupts\interrupts.sh /data/
541 KB/s (8724 bytes in 0.015s)

C:\Users\xxx\Desktop\power_script\interrupts>set /p test_times=please input test times :
please input test times :1

C:\Users\xxx\Desktop\power_script\interrupts>set /p dis_zero_interrupts=please input display zero interrupts(1/0) :
please input display zero interrupts(1/0) :0

C:\Users\xxx\Desktop\power_script\interrupts>adb shell sh /data/interrupts.sh 1 0

C:\Users\xxx\Desktop\power_script\interrupts>adb pull /sdcard/interrupts_result.txt C:\Users\xxx\Desktop\power_script\interrupts\interrupts_result.txt
1 KB/s (1840 bytes in 1.000s)

C:\Users\xxx\Desktop\power_script\interrupts>pause
�밴���������. . .

ʾ���������
ID        	CPU3      	CPU2           	CPU1           	CPU0           	request_irq    	unknown        	acpi_sci       	dev_name                                	
IPI2:     	723356    	1939410        	52             	26             	Rescheduling   	interrupts                              	
IPI4:     	230214    	661886         	14             	0              	Single         	function       	call           	interrupts                              	
16:       	225810    	560953         	13             	121            	GIC            	60             	Level          	r4p0_bcevt_timer                        	
IPI1:     	257744    	484114         	6              	14             	Timer          	broadcast      	interrupts                              	
17:       	274483    	440941         	0              	31             	GIC            	30             	Edge           	arch_timer                              	
IPI6:     	6412      	9161           	0              	0              	IRQ            	work           	interrupts                              	
IPI3:     	908       	999            	0              	0              	Function       	call           	interrupts                              	
55:       	0         	9              	0              	0              	spi5.0         	6              	Edge           	chg_timer                               	
61:       	0         	1              	0              	0              	irq-pmic-eic   	0              	Level          	musbvbusdectectirq                      	
57:       	0         	1              	0              	0              	spi5.0         	4              	Edge           	403c0000.spi:pmic@0:gpio-controller@280 

������ע:
1����ǰ�Ѿ���֤��ƽ̨
sharkl3
�ýű������־����platform,ԭ�����Ǽܹ����䣬����ƽ̨������

2��ֻ������userdebug�汾(user�汾����֧��)

3������Դ���ޣ�δ��֤����ƽ̨����board�������������µ�3216����л֧�������



һ������:��ʾһ��ʱ��CPU������ж���Ϣ

�����÷�:
1������usbץȡ������Ϣ
interrupts.bat�ű����������½���ʱ������Դ���(Ĭ�ϲ���ʱ����10s)��enter��ʼִ��
please input test times :
ִ�н�����consule���棬����������ʾ���밴���������. . .����ʱ��ͨ����������˳�
�������ʱ��ʱ��ʾͳ�ƽ��

2�����Ӵ���ץȡ������Ϣ
��normal״̬�½�interrupts�ű�push��/data/local/tmp/interruptsĿ¼��
adb push xxxx/interrupts(��ֱ��ͨ����ȡ�ļ������������ɸ�·��) /data/local/tmp/interrupts
ͨ�����ڹ���ִ�нű�
./data/local/tmp/interrupts  -t ����ʱ��(Ĭ��10s) 
�������ʱ��ʱ��ʾͳ�ƽ��

3���ֶ�ִ�иù���
bat�ű���ȡ�Ļ���������window�����йأ���bat�ű����г���������ִ�����������ȡ���
adb push C:\Users\xxx\Desktop\power_script\interrupts\interrupts /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/interrupts
./data/local/tmp/interrupts  -t ����ʱ��(Ĭ��10s) 


4��ʾ��:
C:\Users\xxx\Desktop\power_script\interrupts>adb shell "rm /data/local/tmp/interrupts"

C:\Users\xxx\Desktop\power_script\interrupts>adb push C:\Users\xxx\Desktop\power_script\interrupts\interrupts /data/local/tmp/
3689 KB/s (648394 bytes in 0.171s)

C:\Users\xxx\Desktop\power_script\interrupts>adb shell "chmod 777 /data/local/tmp/interrupts"

C:\Users\xxx\Desktop\power_script\interrupts>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\power_script\interrupts>if "" == "" set test_times=10

C:\Users\xxx\Desktop\power_script\interrupts>adb shell "./data/local/tmp/interrupts  -t 10
accumulate_time is 10
              CPU0       CPU1[boot]     TOTAL
  16:          838           38670        838   GIC             60      Level           r4p0_bcevt_timer
   17:        1027           36284       1027   GIC             30      Edge            arch_timer
   24:         430               0        430   GIC             44      Level           70600000.i2c
   25:         128               0        128   GIC             45      Level           70700000.i2c
   28:           1               0          1   GIC             87      Level           musb-hdrc.0.auto
   29:          69               0         69   GIC             92      Level           mmc0
   32:         836               0        836   GIC             78      Level           DISPC
   39:         802               0        802   GIC             71      Level           60000000.gpu, 60000000.gpu, 60000000.gpu
   48:           3               0          3   GIC             101     Level           sprd-mailbox_target
   54:           3               0          3   GIC             70      Level           spi5.0
   55:           1               8          1   spi5.0          6       Edge            chg_timer
   57:           2               3          2   spi5.0          4       Edge            403c0000.spi:pmic@0:gpio-controller@280
   71:           2               2          2   irq-pmic-eic    1       Level           Power Key
   72:          33               0         33   irq-ap-gpio     64      Edge            adaptive_ts-irq
 IPI1:          87            4012         87   Timer broadcast interrupts
 IPI2:        4512          245168       4512   Rescheduling interrupts
 IPI4:         430            8312        430   Single function call interrupts

C:\Users\xxx\Desktop\power_script\interrupts>pause
�밴���������. . .

5 �����˵��
TOTAL��ʾ����ʱ�����ܹ�ͳ�Ƴ��ķ���������CPU�ϵ��ж�����
���ڲ��Թ����д��ڲ�κ˵������CPU,��CPUx[boot]��ʽ��ʾ��ͳ�Ƶ��ж���Ϊ��������������ж���
������Ϊ����ʱ���ڷ������ж�����TOTAL��Ҳ��������������

������ע:
1����ǰ�Ѿ���֤��ƽ̨
sharkl3
pike2
�ýű������־����platform,ԭ�����Ǽܹ����䣬����ƽ̨������

2��������user��userdebug�汾

3������Դ���ޣ�δ��֤����ƽ̨����board�������������µ�3216����л֧�������



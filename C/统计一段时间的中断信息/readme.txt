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


2��ʾ��:
C:\Users\xxx\Desktop\power_script\interrupts>adb shell "rm /data/local/tmp/interrupts"

C:\Users\xxx\Desktop\power_script\interrupts>adb push C:\Users\xxx\Desktop\power_script\interrupts\interrupts /data/local/tmp/
3689 KB/s (648394 bytes in 0.171s)

C:\Users\xxx\Desktop\power_script\interrupts>adb shell "chmod 777 /data/local/tmp/interrupts"

C:\Users\xxx\Desktop\power_script\interrupts>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\power_script\interrupts>if "" == "" set test_times=10

C:\Users\xxx\Desktop\power_script\interrupts>adb shell "./data/local/tmp/interrupts  -t 10
accumulate_time is 10
         CPU0    CPU1
  16:    1763   679376  681139  GIC             60      Level           r4p0_bcevt_timer
   17:   1581   590300  591881  GIC             30      Edge            arch_timer
   24:    714       0     714   GIC             44      Level           70600000.i2c
   25:    226       0     226   GIC             45      Level           70700000.i2c
   28:      1       0       1   GIC             87      Level           musb-hdrc.0.auto
   29:     74       0      74   GIC             92      Level           mmc0
   32:   1175       0    1175   GIC             78      Level           DISPC
   39:   1171       0    1171   GIC             71      Level           60000000.gpu, 60000000.gpu, 60000000.gpu
   48:      4       0       4   GIC             101     Level           sprd-mailbox_target
   54:      1       0       1   GIC             70      Level           spi5.0
   55:      1      15      16   spi5.0          6       Edge            chg_timer
   57:      0       1       1   spi5.0          4       Edge            403c0000.spi:pmic@0:gpio-controller@280
   61:      0       1       1   irq-pmic-eic    0       Level           musb vbus dectect irq
   72:     59       0      59   irq-ap-gpio     64      Edge            adaptive_ts-irq
 IPI1:    216   65638   65854   Timer broadcast interrupts
 IPI2:   6632   3925249 3931881 Rescheduling interrupts
 IPI3:      0      34      34   Function call interrupts
 IPI4:    400   177329  177729  Single function call interrupts
 IPI6:      0       9       9   IRQ work interrupts

C:\Users\xxx\Desktop\power_script\interrupts>pause
�밴���������. . .

������ע:
1����ǰ�Ѿ���֤��ƽ̨
sharkl3
pike2
�ýű������־����platform,ԭ�����Ǽܹ����䣬����ƽ̨������

2��������user��userdebug�汾

3������Դ���ޣ�δ��֤����ƽ̨����board�������������µ�3216����л֧�������



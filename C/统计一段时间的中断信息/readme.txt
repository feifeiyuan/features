一、功能:显示一段时间CPU处理的中断信息

二、用法:
1、连接usb抓取参数信息
interrupts.bat脚本，出现如下界面时输入测试次数(默认测试时间是10s)，enter开始执行
please input test times :
执行结束后consule界面，出现如下提示“请按任意键继续. . .”此时可通过按任意键退出
到达测试时间时显示统计结果

2、连接串口抓取参数信息
在normal状态下将interrupts脚本push到/data/local/tmp/interrupts目录下
adb push xxxx/interrupts(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/interrupts
通过串口工具执行脚本
./data/local/tmp/interrupts  -t 测试时间(默认10s) 
到达测试时间时显示统计结果

3、手动执行该工具
bat脚本获取的环境变量和window配置有关，如bat脚本运行出错，可依次执行如下命令获取结果
adb push C:\Users\xxx\Desktop\power_script\interrupts\interrupts /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/interrupts
./data/local/tmp/interrupts  -t 测试时间(默认10s) 


2、示例:
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
请按任意键继续. . .

三、备注:
1、当前已经验证的平台
sharkl3
pike2
该脚本不区分具体的platform,原则上是架构不变，所有平台均适用

2、适用于user和userdebug版本

3、因资源有限，未验证所有平台所有board，如有问题请致电3216，感谢支持与配合



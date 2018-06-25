一、功能:显示一段时间CPU处理的中断信息

二、用法:
1、连接usb抓取参数信息
interrupts.bat脚本，出现如下界面时输入测试次数(默认测试时间是3s)，enter开始执行
please input test times :
出现如下界面时输入是否显示包含中断数为0的中断(默认不显示)，enter开始执行
please input display zero interrupts(1/0) :0
执行结束后consule界面，出现如下提示“请按任意键继续. . .”此时可通过按任意键退出
interrupts.bat的同级目录下将生成对应的interrupts的结果文件

2、连接串口抓取参数信息
在normal状态下将interrupts.sh脚本push到/data目录下
adb root
adb remount
adb push xxxx/interrupts.sh(可直接通过拉取文件至命令行生成该路劲) /data/

通过串口工具执行脚本
su root
sh /data/interrupts.sh 测试时间(默认3s) 是否显示包含中断数为0的中断(默认不显示)

脚本运行结束后，输出结果保存在/sdcard/interrupts_result.txt中
adb pull /sdcard/interrupts_result.txt 本地路径 
即可提取结果到本地

2、示例:
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
请按任意键继续. . .

示例结果如下
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

三、备注:
1、当前已经验证的平台
sharkl3
该脚本不区分具体的platform,原则上是架构不变，所有平台均适用

2、只适用于userdebug版本(user版本后续支持)

3、因资源有限，未验证所有平台所有board，如有问题请致电3216，感谢支持与配合



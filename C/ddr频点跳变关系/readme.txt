功能：统计一段时间cpu频点变换表

1、连接usb抓取参数信息
双击ddr_trans_table.bat脚本，出现如下界面时输入测试时间(默认测试60s)，enter开始执行
please input test times(s) :
设置结束到结束时间将生成数据结果

2、连接串口抓取参数信息
在normal状态下将ddr_trans_table工具push到/data/local/tmp/目录下
adb push xxxx/ddr_trans_table(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/
通过串口工具执行脚本
chmod 777 /data/local/tmp/ddr_trans_table
./data/local/tmp/ddr_trans_table -t 60

3、手动执行该工具
因bat脚本中的环境变量和window配置相关，遇到bat脚本出错时可手动执行如下命令获取结果
adb push C:\Users\xxx\Desktop\power_script\cpu\ddr_trans_table\ddr_trans_table  /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/ddr_trans_table
./data/local/tmp/ddr_trans_table -t 测试时间(默认测试60s)

4、示例如下
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
请按任意键继续. . .

备注
1、当前已经验证的平台
sharkle
sharkl3

2、只适用userdebug版本，user版本无权限

3、如有疑问，请致电3216，谢谢

4、ddr的频点变化是微妙级别的，所以统计数据不是很准确

功能：统计一段时间cpu频点变换表

1、连接usb抓取参数信息
双击cpu_trans_table.bat脚本，出现如下界面时输入测试时间(默认测试60s)，enter开始执行
please input test times(s) :
设置结束到结束时间将生成数据结果

2、连接串口抓取参数信息
在normal状态下将cpu_trans_table工具push到/data/local/tmp/目录下
adb push xxxx/cpu_trans_table(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/
通过串口工具执行脚本
chmod 777 /data/local/tmp/cpu_trans_table
./data/local/tmp/cpu_trans_table -t 60

3、手动执行该工具
因bat脚本中的环境变量和window配置相关，遇到bat脚本出错时可手动执行如下命令获取结果
adb push C:\Users\xxx\Desktop\power_script\cpu\cpu_trans_table\cpu_trans_table  /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/cpu_trans_table
./data/local/tmp/cpu_trans_table -t 测试时间(默认测试60s)

4、示例如下
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
请按任意键继续. . .


备注
1、当前已经验证的平台
sharkle
sharkl3

2、适用于user版本和userdebug版本

3、如有疑问，请致电3216，谢谢

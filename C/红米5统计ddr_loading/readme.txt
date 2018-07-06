功能：计算红米5 ddr_loading数据

1、连接usb抓取参数信息
双击ddr_loading.bat脚本，出现如下界面时输入测试时间(默认测试60s)，enter开始执行
please input test times(s) :
设置结束将生成数据结果

2、连接串口抓取参数信息
在normal状态下将sgm脚本push到/data/local/tmp/目录下
adb push xxxx/ddr_loading(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/

通过串口工具执行脚本
chmod 777 /data/local/tmp/ddr_loading
./data/local/tmp/ddr_loading -t 60

3、手动抓取ddr_loading数据
因bat脚本获取的环境变量和windows配置相关，当双击bat脚本出现error时
可依次输入如下命令，手动抓取数据
adb root
adb remount
adb push xxxx/ddr_loading(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/ddr_loading
./data/local/tmp/ddr_loading -t 测试时间(默认测试60s)

4、示例如下
C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb root
adbd is already running as root

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb remount
remount succeeded

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb shell "rm /data/local/tmp/ddr_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb push C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm\ddr_loading  /data/local/tmp/
1129 KB/s (641701 bytes in 0.554s)

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb shell "chmod 777 /data/local/tmp/ddr_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>adb shell "./data/local/tmp/ddr_loading -t 60"
accummulate_time is 60(s)
time_use is 60592.86ms
freq:768, perc:0.50%
freq:767, perc:0.83%
freq:422, perc:1.50%
freq:383, perc:0.17%
freq:278, perc:0.17%
freq:211, perc:96.83%

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\ddr_freq_msm>pause
请按任意键继续. . .

备注
1、当前已经验证的平台
红米5

2、只适用于userdebug版本

3、如有疑问，请致电3216，谢谢

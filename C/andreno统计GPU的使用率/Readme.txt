一、功能
抓取红米5 gpu loading数据

二、使用方法
不同场景下使用方法不同

1、连接usb抓取参数信息
双击gpu_loading.bat脚本，出现如下界面时输入测试时长(默认测试时长是60s)，enter开始执行
please input test times :
执行结束后console界面,出现如下提示“请按任意键继续. . .”此时可通过按任意键退出

2、连接串口抓取参数信息
在normal状态下将gpu_loading工具push到/data/local/tmp/gpu_loading
adb push xxxx/gpu_loading(可直接通过拉取文件至命令行生成该路径) /data/local/tmp/gpu_loading

通过串口工具运行该工具
./data/local/tmp/gpu_loading -t 60 测试时长(默认测试时长是60s)

3、示例
C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>if "" == "" set test_times=60

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>adb shell "rm /data/local/tmp/gpu_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>adb push C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading\gpu_loading    /data/local/tmp/
2242 KB/s (643165 bytes in 0.280s)

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>adb shell "chmod 777 /data/local/tmp/gpu_loading"

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>adb shell "./data/local/tmp/gpu_loading -t 60"
accummulate_time is 60(s)
polling interval is 10(ms)
600: 0          560: 0          510: 0          400: 0          320: 0          216: 3492            133: 2508

216: 58.20%      utilisation:47.29%
133: 41.80%      utilisation:48.17%
Averag GPU Utilisation : 47.66%

C:\Users\xxx\Desktop\power_script\contrast_phone\红米5\gpu_loading>pause
请按任意键继续. . .


三、备注
1、当前已验证platform
红米5

2、适用于user和userdebug版本

2、因资料有限，loading信息节点并不一定完全正确，该脚本扫描出来的数据仅供参考







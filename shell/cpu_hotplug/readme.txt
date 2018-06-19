一、功能:cpu热插拔

二、用法:

1、命令行依次输入
adb root				
adb remount
adb push 文件路劲(可直接通过拖拉.sh文件至命令行获取路劲) /data/
adb shell
在正常链接USB的前提下
以上命令也可以直接通过双击cpu_hotplug.bat实现
sh /data/cpu_hotplug.sh	
脚本运行之后，按照提示输入参数即可

2、示例
sp7731e_1h10:/data # sh cpu_hotplug.sh
sp7731e_1h10:/data # sh cpu_hotplug.sh
online_core:    0-3  				: online cpu
offline_core:	     				: offline cpu
please choose 0(off)/1(on)  		: 0（拔核）/1（插核）
0									: 进行拔核
0
please choose operate core 0~3		: 选择需要操作的cpu
2
2
online_core:    0-1,3				: 操作结果
offline_core:   2
sp7731e_1h10:/data #

三、备注:
1、当前已经验证的平台
sharkle	
iWhale2
pike2
SharkL2
sharkleplus
因资源有限，未验证所有平台所有board，原则上架构不变，所有平台均适用

2、只适用于userdebug版本(user版本权限不足)

3、如有问题请联系致电3216，谢谢



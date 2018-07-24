实际上可以解析kernel log来查看，但是kernel log一般很容易被修改，
相对的ftrace就稳定很多

功能:
ftrace_log_start.bat	开启获取suspend_resume事件
ftrace_log_pull.bat		拉取trace文件至当前目录
ftrace_log_end.bat		关闭手机ftrace功能
ftrace_log.py			解析ftrace中的suspend_resume事件

使用说明：
1、开启手机的ftrace功能
双击ftrace_log_start.bat文件开启ftrace，并打开输出suspend_resume事件至文件trace

2、拉取trace文件至当前目录
双击ftrace_log_pull.bat文件拉取trace至当前目录，默认命名为trace

3、解析ftrace中的suspend_resume事件
打开cmd，在命令行输入
python ****\ftrace_log.py(ftrace_log.py文件的完整路劲) ****\trace(trace文件的完整路劲) ****\trace_result.txt(result文件的完整路劲)

4、关闭手机ftrace功能   
双击ftrace_log_end.bat文件关闭trace功能

备注：
1、该脚本基于python3.4开发，由于python不向前兼容，如有问题，请联系袁会

示例:
第 2 sleep:  160.973ms(sleep的总时间)
   system_server-828   [000] ...1  2327.329457: suspend_resume: suspend_enter[3] begin
   system_server-828   [000] ...1  2327.329470: suspend_resume: sync_filesystems[0] begin
   system_server-828   [000] ...1  2327.355390: suspend_resume: sync_filesystems[0] end             25.920ms(单个测试阶段的时间)
   system_server-828   [000] ...1  2327.356396: suspend_resume: freeze_processes[0] begin
   system_server-828   [000] ...1  2327.363637: suspend_resume: freeze_processes[0] end             7.241ms
   system_server-828   [000] ...1  2327.363646: suspend_resume: suspend_enter[3] end                34.189ms

suspend devices : 62.727ms(suspend devices的时间为以下四个时间段的综合)
   system_server-828   [000] ...1  2327.363677: suspend_resume: dpm_prepare[2] begin
   system_server-828   [000] ...1  2327.366882: suspend_resume: dpm_prepare[2] end                  3.205ms
   system_server-828   [000] ...1  2327.366887: suspend_resume: dpm_suspend[2] begin
   system_server-828   [000] ...1  2327.410910: suspend_resume: dpm_suspend[2] end                  44.023ms
   system_server-828   [000] ...1  2327.410923: suspend_resume: dpm_suspend_late[2] begin
   system_server-828   [000] ...1  2327.419094: suspend_resume: dpm_suspend_late[2] end             8.171ms
   system_server-828   [000] ...1  2327.419103: suspend_resume: dpm_suspend_noirq[2] begin
   system_server-828   [000] ...1  2327.426431: suspend_resume: dpm_suspend_noirq[2] end            7.328ms


disable cpu : 63.945ms(disable cpu为连续所有CPU OFF的时间段的总和)
   system_server-828   [000] ...1  2327.426447: suspend_resume: CPU_OFF[1] begin
   system_server-828   [003] ...1  2327.450192: suspend_resume: CPU_OFF[1] end                      23.745ms
   system_server-828   [003] ...1  2327.450202: suspend_resume: CPU_OFF[2] begin
   system_server-828   [003] ...1  2327.470140: suspend_resume: CPU_OFF[2] end                      19.938ms
   system_server-828   [003] ...1  2327.470149: suspend_resume: CPU_OFF[3] begin
   system_server-828   [000] ...1  2327.490411: suspend_resume: CPU_OFF[3] end                      20.262ms

   system_server-828   [000] d..1  2327.490424: suspend_resume: syscore_suspend[0] begin
   system_server-828   [000] d..1  2327.490430: suspend_resume: syscore_suspend[0] end              0.006ms
   system_server-828   [000] d..1  2327.490430: suspend_resume: machine_suspend[3] begin

第 2 resume:  47.182ms(resume的总时间)
   system_server-828   [000] d..1  2327.490430: suspend_resume: machine_suspend[3] end
   system_server-828   [000] d..1  2327.490430: suspend_resume: syscore_resume[0] begin
   system_server-828   [000] dn.1  2327.490439: suspend_resume: syscore_resume[0] end               0.009ms

enable cpu : 5.239ms(enable cpu为连续所有CPU ON的时间段的总和)
   system_server-828   [000] ...1  2327.490658: suspend_resume: CPU_ON[1] begin
   system_server-828   [000] ...1  2327.492203: suspend_resume: CPU_ON[1] end                       1.545ms
   system_server-828   [000] ...1  2327.492468: suspend_resume: CPU_ON[2] begin
   system_server-828   [000] ...1  2327.494127: suspend_resume: CPU_ON[2] end                       1.659ms
   system_server-828   [000] ...1  2327.494365: suspend_resume: CPU_ON[3] begin
   system_server-828   [000] ...1  2327.496400: suspend_resume: CPU_ON[3] end                       2.035ms


resume devices : 25.047ms(resume devices的时间为以下四个时间段的综合)
   system_server-828   [000] ...1  2327.496637: suspend_resume: dpm_resume_noirq[16] begin
   system_server-828   [000] ...1  2327.500424: suspend_resume: dpm_resume_noirq[16] end            3.787ms
   system_server-828   [000] ...1  2327.500433: suspend_resume: dpm_resume_early[16] begin
   system_server-828   [000] ...1  2327.504924: suspend_resume: dpm_resume_early[16] end            4.491ms
   system_server-828   [000] ...1  2327.504934: suspend_resume: dpm_resume[16] begin
   system_server-828   [000] .n.1  2327.516851: suspend_resume: dpm_resume[16] end                  11.917ms
   system_server-828   [000] ...1  2327.516938: suspend_resume: dpm_complete[16] begin
   system_server-828   [000] ...1  2327.521790: suspend_resume: dpm_complete[16] end                4.852ms

   system_server-828   [000] ...1  2327.521797: suspend_resume: resume_console[3] begin
   system_server-828   [000] ...1  2327.523027: suspend_resume: resume_console[3] end               1.230ms
   system_server-828   [000] ...1  2327.523050: suspend_resume: thaw_processes[0] begin
   system_server-828   [000] ...1  2327.537612: suspend_resume: thaw_processes[0] end               14.562ms
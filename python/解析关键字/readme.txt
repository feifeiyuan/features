ʵ���Ͽ��Խ���kernel log���鿴������kernel logһ������ױ��޸ģ�
��Ե�ftrace���ȶ��ܶ�

����:
ftrace_log_start.bat	������ȡsuspend_resume�¼�
ftrace_log_pull.bat		��ȡtrace�ļ�����ǰĿ¼
ftrace_log_end.bat		�ر��ֻ�ftrace����
ftrace_log.py			����ftrace�е�suspend_resume�¼�

ʹ��˵����
1�������ֻ���ftrace����
˫��ftrace_log_start.bat�ļ�����ftrace���������suspend_resume�¼����ļ�trace

2����ȡtrace�ļ�����ǰĿ¼
˫��ftrace_log_pull.bat�ļ���ȡtrace����ǰĿ¼��Ĭ������Ϊtrace

3������ftrace�е�suspend_resume�¼�
��cmd��������������
python ****\ftrace_log.py(ftrace_log.py�ļ�������·��) ****\trace(trace�ļ�������·��) ****\trace_result.txt(result�ļ�������·��)

4���ر��ֻ�ftrace����   
˫��ftrace_log_end.bat�ļ��ر�trace����

��ע��
1���ýű�����python3.4����������python����ǰ���ݣ��������⣬����ϵԬ��

ʾ��:
�� 2 sleep:  160.973ms(sleep����ʱ��)
   system_server-828   [000] ...1  2327.329457: suspend_resume: suspend_enter[3] begin
   system_server-828   [000] ...1  2327.329470: suspend_resume: sync_filesystems[0] begin
   system_server-828   [000] ...1  2327.355390: suspend_resume: sync_filesystems[0] end             25.920ms(�������Խ׶ε�ʱ��)
   system_server-828   [000] ...1  2327.356396: suspend_resume: freeze_processes[0] begin
   system_server-828   [000] ...1  2327.363637: suspend_resume: freeze_processes[0] end             7.241ms
   system_server-828   [000] ...1  2327.363646: suspend_resume: suspend_enter[3] end                34.189ms

suspend devices : 62.727ms(suspend devices��ʱ��Ϊ�����ĸ�ʱ��ε��ۺ�)
   system_server-828   [000] ...1  2327.363677: suspend_resume: dpm_prepare[2] begin
   system_server-828   [000] ...1  2327.366882: suspend_resume: dpm_prepare[2] end                  3.205ms
   system_server-828   [000] ...1  2327.366887: suspend_resume: dpm_suspend[2] begin
   system_server-828   [000] ...1  2327.410910: suspend_resume: dpm_suspend[2] end                  44.023ms
   system_server-828   [000] ...1  2327.410923: suspend_resume: dpm_suspend_late[2] begin
   system_server-828   [000] ...1  2327.419094: suspend_resume: dpm_suspend_late[2] end             8.171ms
   system_server-828   [000] ...1  2327.419103: suspend_resume: dpm_suspend_noirq[2] begin
   system_server-828   [000] ...1  2327.426431: suspend_resume: dpm_suspend_noirq[2] end            7.328ms


disable cpu : 63.945ms(disable cpuΪ��������CPU OFF��ʱ��ε��ܺ�)
   system_server-828   [000] ...1  2327.426447: suspend_resume: CPU_OFF[1] begin
   system_server-828   [003] ...1  2327.450192: suspend_resume: CPU_OFF[1] end                      23.745ms
   system_server-828   [003] ...1  2327.450202: suspend_resume: CPU_OFF[2] begin
   system_server-828   [003] ...1  2327.470140: suspend_resume: CPU_OFF[2] end                      19.938ms
   system_server-828   [003] ...1  2327.470149: suspend_resume: CPU_OFF[3] begin
   system_server-828   [000] ...1  2327.490411: suspend_resume: CPU_OFF[3] end                      20.262ms

   system_server-828   [000] d..1  2327.490424: suspend_resume: syscore_suspend[0] begin
   system_server-828   [000] d..1  2327.490430: suspend_resume: syscore_suspend[0] end              0.006ms
   system_server-828   [000] d..1  2327.490430: suspend_resume: machine_suspend[3] begin

�� 2 resume:  47.182ms(resume����ʱ��)
   system_server-828   [000] d..1  2327.490430: suspend_resume: machine_suspend[3] end
   system_server-828   [000] d..1  2327.490430: suspend_resume: syscore_resume[0] begin
   system_server-828   [000] dn.1  2327.490439: suspend_resume: syscore_resume[0] end               0.009ms

enable cpu : 5.239ms(enable cpuΪ��������CPU ON��ʱ��ε��ܺ�)
   system_server-828   [000] ...1  2327.490658: suspend_resume: CPU_ON[1] begin
   system_server-828   [000] ...1  2327.492203: suspend_resume: CPU_ON[1] end                       1.545ms
   system_server-828   [000] ...1  2327.492468: suspend_resume: CPU_ON[2] begin
   system_server-828   [000] ...1  2327.494127: suspend_resume: CPU_ON[2] end                       1.659ms
   system_server-828   [000] ...1  2327.494365: suspend_resume: CPU_ON[3] begin
   system_server-828   [000] ...1  2327.496400: suspend_resume: CPU_ON[3] end                       2.035ms


resume devices : 25.047ms(resume devices��ʱ��Ϊ�����ĸ�ʱ��ε��ۺ�)
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
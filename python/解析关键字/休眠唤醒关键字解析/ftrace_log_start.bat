adb root
adb remount
adb shell "rm -rf /sdcard/debugfs"
adb shell mkdir /sdcard/debugfs/
adb shell mount -t debugfs none /sdcard/debugfs/
adb shell "echo 1 > /sdcard/debugfs/tracing/events/power/suspend_resume/enable"
adb shell "cat /sdcard/debugfs/tracing/events/power/suspend_resume/enable"
adb shell "echo 1 > /sdcard/debugfs/tracing/tracing_on"
adb shell "cat /sdcard/debugfs/tracing/tracing_on"
pause

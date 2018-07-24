adb root
adb remount
adb shell "mv /sdcard/debugfs/tracing/trace	/sdcard/trace"
adb shell "echo 0 > /sdcard/debugfs/tracing/events/power/suspend_resume/enable"
adb shell "cat /sdcard/debugfs/tracing/events/power/suspend_resume/enable"
adb shell "echo 0 > /sdcard/debugfs/tracing/tracing_on"
adb shell "cat /sdcard/debugfs/tracing/tracing_on"
adb shell "umount /sdcard/debugfs"
adb shell "rm -rf /sdcard/debugfs"
pause


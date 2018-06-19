@echo on
adb root
adb remount
adb push %cd%\cpu_hotplug.sh /data/
adb shell





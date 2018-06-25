@echo on
adb root
adb remount
adb push %cd%\interrupts.sh /sdcard/
set /p test_times=please input test times :
if "%test_times%"=="" set test_times=10
set /p dis_zero_interrupts=please input display zero interrupts(1/0) :
if "%dis_zero_interrupts%"=="" set dis_zero_interrupts=0
adb shell sh /sdcard/interrupts.sh %test_times% %dis_zero_interrupts%
adb pull /sdcard/interrupts_result.txt %cd%\interrupts_result.txt
pause





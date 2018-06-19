set /p test_times=please input test times :
if "%test_times%"=="" set test_times=60
set dir=%cd%\tool\cpu_data.sh 
adb push %dir% /sdcard/
adb shell sh /sdcard/cpu_data.sh %test_times%
adb pull /sdcard/cpu_loading	%cd%\tool\

pause

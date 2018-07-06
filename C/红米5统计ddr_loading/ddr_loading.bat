adb root
adb remount
set /p test_times=please input test times(s) :
if "%test_times%"=="" set test_times=60
adb shell "rm /data/local/tmp/ddr_loading"
adb push %cd%\ddr_loading	/data/local/tmp/
adb shell "chmod 777 /data/local/tmp/ddr_loading"
adb shell "./data/local/tmp/ddr_loading -t %test_times%"
pause

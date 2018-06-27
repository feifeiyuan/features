adb root
adb remount
adb shell "rm /data/local/tmp/gpu_trans_table"
set /p test_times=please input test times(s) :
if "%test_times%"=="" set test_times=60
adb push %cd%\gpu_trans_table	/data/local/tmp/
adb shell "chmod 777 /data/local/tmp/gpu_trans_table"
adb shell "./data/local/tmp/gpu_trans_table -t %test_times%"
pause

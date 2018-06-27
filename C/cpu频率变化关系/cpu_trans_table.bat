adb shell "rm /data/local/tmp/cpu_trans_table"
set /p test_times=please input test times(s) :
if "%test_times%"=="" set test_times=60
adb push %cd%\cpu_trans_table	/data/local/tmp/
adb shell "chmod 777 /data/local/tmp/cpu_trans_table"
adb shell "./data/local/tmp/cpu_trans_table -t %test_times%"
pause

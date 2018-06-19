
adb shell "rm /data/local/tmp/top"
adb push %cd%\top	/data/local/tmp/
set /p test_times=please input test times(s) :
if "%test_times%"=="" set test_times=10
set /p num=please input top number :
if "%num%"=="" set num=10
adb shell "chmod 777 /data/local/tmp/top"
adb shell "./data/local/tmp/top -t %test_times% -n %num%"
pause

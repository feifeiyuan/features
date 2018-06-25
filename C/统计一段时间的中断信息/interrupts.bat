adb shell "rm /data/local/tmp/interrupts"
adb push %cd%\interrupts	/data/local/tmp/
adb shell "chmod 777 /data/local/tmp/interrupts"
set /p test_times=please input test times :
if "%test_times%"=="" set test_times=10
adb shell "./data/local/tmp/interrupts  -t %test_times% 
pause





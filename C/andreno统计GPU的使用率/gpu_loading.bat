set /p test_times=please input test times :
if "%test_times%"=="" set test_times=60
adb shell "rm /data/local/tmp/gpu_loading"
adb push %cd%\gpu_loading	/data/local/tmp/
adb shell "chmod 777 /data/local/tmp/gpu_loading"
adb shell "./data/local/tmp/gpu_loading -t %test_times%"
pause

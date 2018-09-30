adb root
adb remount
adb push Y:\gpu_proportion /data/local/tmp/
adb push %cd%\setting\gpu_path.txt /sdcard/
adb shell "chmod 777 /data/local/tmp/gpu_proportion"
adb shell "./data/local/tmp/gpu_proportion -t 4"
adb pull /sdcard/gpu_proportion/ %cd%\datas
pause
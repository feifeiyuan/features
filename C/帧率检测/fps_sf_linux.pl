use Time::HiRes qw( usleep time );
					  
$r=0;
$f1=0;
$f2=0;

$t = time;

$f1=`adb shell dumpsys SurfaceFlinger | grep flips`;
$f1 =~ /flips=(\d+)/;
$f1 = $1;
$t1 = time;

while(1)
{
	usleep(890000);   # Shoot for printing a sample every 1 second.  
	$f2=`adb shell dumpsys SurfaceFlinger | grep flips`;
	$t2 = time;

	$f2 =~ /flips=(\d+)/;
	$f2 = $1;

    $frames = $f2 - $f1;
    $f1 = $f2;
	$elapsed_time = $t2 - $t1;
	$fps = $frames / $elapsed_time;
	printf( "Elapsed Time: %5.3f    Frames: %d   FPS: %6.2f\n", $elapsed_time, $frames, $fps);

	$t1 = $t2;
}


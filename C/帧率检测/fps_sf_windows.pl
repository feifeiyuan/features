
use Time::HiRes qw( usleep time );
					  
$r=0;
$f1=0;
$f2=0;

$t = time;

$f1=`adb shell dumpsys SurfaceFlinger | findstr flips`;
$f1 =~ /flips=(\d+)/;
$f1 = $1;
$t1 = time;
$total_time = 1;
if ($ARGV[0] != 0){
	$total_time = $ARGV[0];
}
$total_time *=60;
$count_time = 0;
$secound_average = 0;
@arry = ();
@secound_array=();
$secound_cound = 0;
while(1)
{
	usleep(890000);   # Shoot for printing a sample every 1 second.  
	$f2=`adb shell dumpsys SurfaceFlinger | findstr flips`;
	$t2 = time;
 
	$f2 =~ /flips=(\d+)/;
	$f2 = $1;

    $frames = $f2 - $f1;
    $f1 = $f2;
	$elapsed_time = $t2 - $t1;
	$fps = $frames / $elapsed_time;
	printf( "Elapsed Time: %5.3f    Frames: %d   FPS: %6.2f\n", $elapsed_time, $frames, $fps);
	$t1 = $t2;
	$count_time += $elapsed_time;
	push(@secound_array, sprintf "%6.2f",$fps);
	$secound_average += $_ foreach @secound_array;
	if($total_time%60==0){
		push(@arry, sprintf "%6.2f",$secound_average/scalar(@secound_array));
		$secound_average = 0;
	}
	if( $total_time < $count_time   ){
		last;
	}
}
$sum = 0;
foreach $a (@arry){
	$sum +=$a;
}
printf(" the average  FPS : %6.2f", $sum / scalar(@secound_array));




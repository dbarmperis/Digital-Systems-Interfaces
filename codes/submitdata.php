<?php
	// Checks if the variables exist and then
	// creates php code in the file data.php that stores the current values of the variables
  // but also the last time they were triggered (set to 1)
if(isset($_GET['temp'],$_GET['door'],$_GET['motion'],$_GET['smoke'])) {
	if(file_exists('data.php')){
		include 'data.php';
	} else {
		$last_door = 0;
		$last_motion = 0;
		$last_smoke = 0;
	}
	$temp = $_GET['temp'];
	$door = $_GET['door'];
	$smoke = $_GET['smoke'];
	$motion = $_GET['motion'];
	$filename = 'data.php';
	date_default_timezone_set('Europe/Athens');
	$now = date('l jS \of F Y h:i:s A');
	if ($door)
		$last_door = $now;
	if ($smoke)
		$last_smoke = $now;
	if ($motion)
		$last_motion = $now;
	$var = "<?php\n" . '$time = ' . "'" . $now . "'" . ';$temp = ' . $temp . ';$door = ' . $door . ';$smoke = ' . $smoke . ';$motion = ' . $motion;
	$var = $var  . ';$last_door = \'' . $last_door . '\';$last_smoke=\'' . $last_smoke . '\';$last_motion=\'' . $last_motion . "';\n?>";
	file_put_contents($filename, $var);
	echo 'Data Submited';
	return 0;
}else {
	echo 'Error';
	}
?>

<?php
// Imports the data from data.php and prints them
include('data.php');
echo $time . "\r\n";
echo '<br>Temperature: ' . $temp . "\r\n";
echo '<br>Door: ' . $door . "\r\n";
echo '<br>Smoke: ' . $smoke . "\r\n";
echo '<br>Motion: ' . $motion . "\r\n";
echo '<br>Last_Door: ' . $last_door . "\r\n";
echo '<br>Last_Smoke: ' . $last_smoke . "\r\n";
echo '<br>Last_Motion: ' . $last_motion . "\r\n";
?>

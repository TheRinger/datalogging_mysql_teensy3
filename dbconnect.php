<?php
$MyUsername = "[your mysql username (root)]";  // enter your username for mysql
$MyPassword = "[your mysql database password]";  // enter your password for mysql
$MyHostname = "localhost";      // this is usually "localhost" unless your database resides on a different server

$dbh = mysql_pconnect($MyHostname , $MyUsername, $MyPassword);
$selected = mysql_select_db("data",$dbh);
?>

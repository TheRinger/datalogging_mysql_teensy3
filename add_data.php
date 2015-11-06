<?php
    // Connect to MySQL
    include("dbconnect.php");

    // Prepare the SQL statement
    $SQL = "INSERT INTO data.sensors (fahrenheit ,humidity ,co2 ,light ,temp2) VALUES ('".$_GET["fahrenheit"]."', '".$_GET["humidity"]."', '".$_GET["co2"]."', '".$_GET["light"]."', '".$_GET["temp2"]."')";     

    // Execute SQL statement
    mysql_query($SQL);

    // Go to the review_data.php (optional)
    header("Location: review_data.php");
?>

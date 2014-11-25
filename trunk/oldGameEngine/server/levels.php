<?php

function getConnection() {
  $hostname = "mysql4.000webhost.com";
  $user     = "a6150053_smf9";
  $password = "12345qwert";
  return mysql_connect($hostname, $user, $password);
}

$link = getConnection();
$db = mysql_select_db($dbname, $link);

function generateUserName() {
  $f="QWERTYUIOPASDFGHJKLZXCVBNM1234567890qwertyuiopasdfghjklzxcvbnm";
  $s = "";
  $l = strlen($f);
  for ($i = 0; $i < 15; $i++) {
    $s .= $f[rand() % $l];
  }
  return $s;
}

function isInBase($username) {
  global $table;
  $query = "SELECT * FROM `a6150053_smf9`.`UserList` WHERE `username`='".$username."';";
  $result = mysql_query($query);
  if ($result) {
    return (mysql_numrows($result) > 0);
  }
  return false;
}

function newUserName() {
  $newName = generateUserName();
  while (isInBase($newName)) { $newName = generateUserName(); }
  return $newName;
}

function errorMsg($msg) {
  print $msg;
}

if (isset($_GET['new'])) {
  $name = newUserName();
  $query = 'INSERT INTO `a6150053_smf9`.`UserList` (`username`) VALUES (\''.$name.'\');';
  $result = mysql_query($query);
  If (!$result) {
    print $query;
    errorMsg("can not insert data");
  } else {
    print $name;
  } 
} else if (isset($_GET['save'])) {
  $name = $_GET['username'];
  if (isInBase($name)) {
    $f = fopen("files/".$name.".xml", "wt");
    if ($f) {
      if (!fwrite($f, $_GET['content'])) {
        print "can not save levels";
      } else {
        print "saving levels - ok";
      }
      fclose($f);
    }
  } else {
    print "Fuck off!";
  }
} else {
  errorMsg("Wrong link!");
}

mysql_close($link);

?>
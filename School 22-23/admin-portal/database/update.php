<?php if($_SERVER['REQUEST_METHOD'] == "POST")
	{
	$newName = $_POST['name'];
	$newEmail = $_POST['email'];
	$newUsername = $_POST['username'];
	$password = $_POST['password'];
	$password2 = $_POST['password2'];


if(($password == $password2) && $password == $dbPassword){
	if(!empty($newName)){
		$query = "UPDATE `users` SET `name`='$newName' WHERE username = '$_SESSION[username]' ";
			mysqli_query($con, $query);
	}

	if(!empty($newUsername)){
		$query = "UPDATE `users` SET `username`='$newUsername' WHERE username = '$_SESSION[username]'";
			mysqli_query($con, $query);
			header('location: logIn.php');
			die;

	}
	if($newEmail_value == true){
		$query = "UPDATE `users` SET `email`='$newEmail' WHERE username = '$_SESSION[username]'";
			mysqli_query($con, $query);
	}
	
	header("Location: edit.php");
	
	die;

}
else{
	echo "Your profile was not updated";
}
}
?>

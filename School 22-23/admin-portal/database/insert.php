<?php
	if($_SERVER['REQUEST_METHOD'] == "POST")
	{
	
		$name = $_POST['name'];
		$email = $_POST['email'];
		$username = $_POST['username'];
		$password = $_POST['password'];
		$password2 = $_POST['password2'];
		

			$query = "insert into users (name, email, username, password) values ('$name','$email','$username','$password')";
			
			$result = mysqli_query($con, $query);
			if($result){
			header("Location: logIn.php");
			die;
			}
			if(!result){
				$errorMessage = "Data could not be uploaded";
			}
		}
		?>
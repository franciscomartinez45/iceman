<?php 
if($_SERVER['REQUEST_METHOD'] == "POST")
	{
		$username = $_POST['username'];
		$password = $_POST['password'];
		
		
			$query = "select * from users where username = '$username' limit 1  ";
			$result = mysqli_query($con, $query);
	

			if($result)
			{
				if($result && mysqli_num_rows($result) > 0)
				{
						
					$user_data = mysqli_fetch_assoc($result);
					
					
					if($user_data['password'] === $password)
					{
						if ($user_data['user_type'] == 'admin'){
							$_SESSION['admin']= $username;
							header('location: admin.php');
							die;
						}
							else if ($user_data['user_type']=='user'){
							$_SESSION['name']=$name;
							$_SESSION['username'] = $username;
							$_SESSION['id'] = $user_data['id'];
							header('location:startpage.php');
							die;
					}
				}
				else if($user_data['password']!== $password)
				{
					$invalidPas = "Invalid Password and/or Email";
				}
			}
			
			}
			else if (!result){
	
			}
		}
		?>
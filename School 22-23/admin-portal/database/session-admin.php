<?php $sql = "SELECT * FROM users WHERE user_type = 'admin';";
$result = mysqli_query($con,$sql);
$resultCheck = mysqli_num_rows($result);
	if ($resultCheck>0){
	while($row = mysqli_fetch_assoc($result)){
		$adminname=$row['name'];
	}
	}
	?>
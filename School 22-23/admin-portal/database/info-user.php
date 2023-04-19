<?php $sql = "SELECT * FROM users WHERE username = '$_SESSION[admin]'";
$result = mysqli_query($con,$sql);
if($result){
$resultCheck = mysqli_num_rows($result);

	if ($resultCheck>0){
	while($row = mysqli_fetch_assoc($result)){
		$name=$row['name'];
		$email =$row['email'];
		$username =$row['username'];
		$dbPassword = $row['password'];
		$user_type = $row['user_type'];
	}};
}
?>		
<?php include 'header.php'?>
<?php include 'database/connection.php'?>

<?php 
$sql = "SELECT * FROM `users` WHERE user_type = 'admin';";
$result = mysqli_query($con,$sql);
$resultCheck = mysqli_num_rows($result);
	if ($resultCheck>0){
	while($row = mysqli_fetch_assoc($result)){
		$adminname=$row['name'];
	}
	}
?>
<div id ="admin_page">
<div id ="table-container">
<h1>Welcome Administrator: <?php echo $adminname?></h1>
<table class= "table-style">
<thead>
			<tr>
			<th>Name</th>
			<th>Email</th>
			<th>Username</th>
			<th>Action</th>
			</tr>
</thead>

<?php 
$sql = "SELECT * FROM `users` WHERE user_type = 'user' ;";
$result = mysqli_query($con,$sql);
$resultCheck = mysqli_num_rows($result);

	if ($resultCheck>0){
	while($row = mysqli_fetch_assoc($result)){
		$name=$row['name'];
		$email=$row['email'];
		$username=$row['username'];
		$id = $row['id'];
		

	?>

<tbody>
	<tr>
	<td><?php echo $name?></td>
	<td><?php echo $email?></td>
	<td><?php echo $username?></td>
	<td><button onclick="return alert('Are your sure you want to delete')" type="submit" id="click" name="delete" ><a href = "?id=<?php echo $id?>">Delete</a></button></td>
	</tr>


<?php 
	}
	}
?>

</tbody>
</table>
</div>
</div>	

<?php 

if (isset($_GET["id"])){
	$id = $_GET["id"];
	
	$delete = mysqli_query($con,"DELETE FROM users where id = '$id'");
	if($delete){
		header("location: admin.php");
		die();
	}
}

?>



</table>

</div>
<script>
function dropTab() {
            var courses = document.getElementById("list-items");
 
            if (courses.style.display == "block") {
                courses.style.display = "none";
            } else {
                courses.style.display = "block";
            }
        }
        window.onclick = function (event) {
            if (!event.target.matches('.dropdown-button')) {
                document.getElementById('list-items')
                    .style.display = "none";
            }
        }
</script>
<?php include "footer.php"?>
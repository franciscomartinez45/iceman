<?php 

session_start();

?>
<!DOCTYPE html>
<html lang="en">
<head>
<title>Backend Programing using PHP and MYSQLI</title>
<meta charset="utf-8">
<link rel = "stylesheet" href = "css/body.css">
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Mono&family=IBM+Plex+Sans:ital@1&display=swap" rel="stylesheet">
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
<script src="https://kit.fontawesome.com/2b27522cda.js" crossorigin="anonymous"></script>



</head>

<body>

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






<div id = "header">
Nameless Website

</div>

<?php if (isset($_SESSION['admin'])){?>
  <div id ="navigation">
  <div id ="nav">
<a href = "startpage.php">GET STARTED</a>
</div>
<div id ="nav">
<a href = "#footer">CONTACT US <i class="fa-solid fa-phone"></i></a>
</div>
<div id ="nav">
 <div id="dropdown-list">
  <button onclick="dropTab()" class ="dropdown-button" >MY ACCOUNT <i class="fa-solid fa-user"></i></button>  
      <div id="list-items">  
        <li><a href="admin.php"> VIEW TABLE <i class="fa-solid fa-table-list"></i></a></li>
        <li><a href ="edit.php"> EDIT INFO</a></li>
         <li><a href="logout.php"> LOG OUT <i class="fa-solid fa-arrow-right-from-bracket"></i></a></li>
       
      </div>   
 </div>
  </div>
</div>
<?php } else{?>
  <div id ="navigation">
  <div id ="nav">
<a href = "startpage.php">GET STARTED</a>
</div>
<div id ="nav">
<a href = "#footer">CONTACT US <i class="fa-solid fa-phone"></i></a>
</div>
<div id ="nav">
<a href = "signUp.php">SIGN UP</a>
</div>
<div id ="nav">
<a href = "logIn.php">LOG IN</a>
</div>
</div>


<?php }?>
</div>





<script>
window.onscroll = function() {myFunction()};
var navbar = document.getElementById("navigation");
var sticky = navbar.offsetTop;
function myFunction() {
  if (window.pageYOffset >= sticky) {
    navbar.classList.add("sticky")
  } else {
    navbar.classList.remove("sticky");
  }
}	  
 
</script>

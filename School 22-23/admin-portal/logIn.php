<?php require 'header.php'?>

<?php 
	include 'database/connection.php';
  include 'database/session.php'

		?>
<div id  = "page_wrapper">
<div id = "login-form">

<form id="verification-form" method="post" action="">
		
		<h1>log in</h1>
				<?php if (isset($invalidPas)){?>
		<small class ="red"><?php echo $invalidPas;}?></small>
		
		<div class = "form-control">

            <label for="username">Username:</label>
            <input type="text" name="username" id="username" autocomplete="off"></input>
			<div class="message">
			<small id ="user-error" class ="red"></small>
		</div>
		</div>
		<div class ="form-control">
			<label for="password">Password:</label>	
			<input type="password" name="password" id="password" autocomplete="off"></input>
	<div class="message">
			<small id ="password-error" class = "red"></small>
		</div>
		</div>


		<div class ="form-control">
<button type="submit" id="submit" name="button" disabled>Log In</button>
</div>
		
<small> If you're not already registered, hit the register link below to be redirected to the register screeen!</small><br>

<div id ="start">
<a href = "signUp.php">Register</a><br>
		</div>
		



</form>
</div>
</div>

<script>
'use strict';


$('#verification-form').on('submit', function(e) {
  
  if ($(this).hasClass('form-submitted')) {
    e.preventDefault(); 
  } else {
    $(this).addClass('form-submitted'); 
    $('#username').attr('readonly', true); 
	$('#password').attr('readonly', true);
    $('#submit').attr('disabled', true); 
  }
});


var formValid = {
  username: false, 
  password: false 
  
};


function checkValidation() {
  
  if (formValid.username && formValid.password) {
    $('#submit').removeAttr('disabled'); 
  } else {
    $('#submit').attr('disabled', true); 
  }
}


$('#username').on('input', function() {
  var username = $(this).val(); 

  
  function msg(body) {
    $('#user-error').text(body).show(); 
  };

  
  function hide() {
    $('#user-error').hide();
  };

   function error(){
  $('input[name=username]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=username]').removeClass("error").addClass("success");
  }

  if (username.length < 1) {
    msg('This field is required.'); 
    formValid.username = false; 
    checkValidation();
	error();
  } else {
	 error();
    hide(); 
    formValid.username = true; 
    checkValidation(); 
    var testExp = new RegExp(/^[a-zA-Z0-9]+$/); 
   
    if (!testExp.test(username)) {
	error();
      msg('Username does not contain special characters'); 
      formValid.username = false; 
      checkValidation(); 
    } else {
	error();
      hide(); 
      formValid.username = true;
      checkValidation();
      
      if (username.length < 5 ) {
		 error();
        msg('Must be at least 5 characters');
        formValid.username = false; 
        checkValidation(); 
      } else {
		success();
        hide(); 
        formValid.username = true; 
        checkValidation(); 
      }
    }
  }
});




$('#password').on('input', function() {
  var password = $('#password').val(); 


 
  function msg(body) {
    $('#password-error').text(body).show(); 
  };

  
  function hide() {
    $('#password-error').hide(); 
  };
		function error(){
  $('input[name=password]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=password]').removeClass("error").addClass("success");
  }


  if (password.length < 1) {
	  error();
    msg('*This field is required.'); 
    formValid.password = false; 
    checkValidation(); 
  } else {
	   if (password.length < 5) {
        msg('Must be at least 5 characters'); 
        formValid.password = false; 
        checkValidation(); 
		error();
      } else {
        hide();
        formValid.password = true; 
        checkValidation(); 
    hide(); 
    formValid.password = true; 
	var testExp = new RegExp(/^(?=.*[A-Za-z])(?=.*\d)(?=.*[@$!%*#?&])[A-Za-z\d@$!%*#?&]{8,}$/); 
   
    if (!testExp.test(password)) {
      msg('Must contain AlphaNumeric values, an uppercase and a special character'); 
      formValid.password = false; 
      checkValidation(); 
	  error();
    } else {
      hide(); 
	  success();
      formValid.password = true;
      checkValidation();
     
    }
	
  
  }}
});
</script>

<?php require 'footer.php'?>
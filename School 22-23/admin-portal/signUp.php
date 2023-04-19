<?php require 'header.php' ?>
<?php 
	include 'database/connection.php';
	include 'database/insert.php';


		
?>

<div id="page_wrapper">
<div id ="sidebar">
</div>
<div id ="container">
<div id = "signup-form">
	<h1>SIGN UP</h1>
        <form id="verification-form" method = "POST" action = "">
            

<div class ="form-control">

                <label for="name">FULL NAME:</label>
                <input type="text" name="name" id="name" autocomplete="off"></input>
				<div class="message">
				<small id = "name-error" class="red"></small>
				</div>
</div>	
<div class ="form-control">	
                <label for="username">USERNAME:</label>
                <input type="text" name="username" id="username" autocomplete="off"></input>
				<div class="message">
				<small id = "username-error" class ="red"></small>
				</div>
</div>
<div class ="form-control"> 
                <label for="email">EMAIL:</label>
                <input type="text" name="email" id="email" autocomplete="off"></input>
				<div class="message">
				<small id = "email-error" class="red"></small>
				</div>
</div>
<div class ="form-control">
                <label for="password">PASSWORD:</label>
                <input type="password" name="password" id="password" autocomplete="off"></input>
				<div class="message">
				<small  id = "password-error" class ="red"></small>
				</div>
</div>
<div class ="form-control">         
                <label for="password2">RE-ENTER PASSWORD:</label>
                <input type="password" name="password2" id="password2" autocomplete="off"></input>
				<div class="message">
				<small id = "password2-error" class ="red"></small>
				</div>
</div> 


<br><small>By hitting submit, you allow us to collect your information. You will be directed to the log in page </small>
		<div class ="form-control">
<button type="submit" id="submit" name="button" disabled>REGISTER</button>
</div>
</form>
 </div>
<small>If you're already registered, hit the Log In button below to be redirected to the log in screeen! </small><br>
<div id ="start">
<a href = "logIn.php">Log In</a><br>
		</div>


        


  
  </div>
  <div id ="faq">
  </div>
   </div>

<script>
'use strict';


$('#verification-form').on('submit', function(e) {
  
  if ($(this).hasClass('form-submitted')) {
    e.preventDefault(); 
  } else {
    $(this).addClass('form-submitted'); 
	$('#name').attr('readonly', true); 
    $('#username').attr('readonly', true); 
	$('#password').attr('readonly', true);
	$('#email').attr('readonly', true);
    $('#submit').attr('disabled', true); 
  }
});


var formValid = {
  name: false,
  username: false, 
  email: false,
  password: false,
  password2: false

  
};


function checkValidation() {
  
  if (formValid.username && formValid.password && formValid.email && formValid.name && formValid.password2) {
    $('#submit').removeAttr('disabled'); 
  } else {
    $('#submit').attr('disabled', true); 
  }
}


$('#name').on('input', function() {
  var name = $(this).val(); 

  
  function msg(body) {
    $('#name-error').text(body).show(); 
  };

  
  function hide() {
    $('#name-error').hide();
  };
  
  function error(){
  $('input[name=name]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=name]').removeClass("error").addClass("success");
  }

 
  if (name.length < 1) {
	error();
    msg('*This field is required.'); 
    formValid.name = false; 
    checkValidation();
	
	
  } else {
	error();
    hide(); 
    formValid.name = true; 
    checkValidation(); 
    var testExp = new RegExp(/^[a-z,',-]+(\s)[a-z,',-]+$/i); 
   
    if (!testExp.test(name)) {
      msg('Please include first and lastname'); 
      formValid.name = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.name = true;
      checkValidation();
      
      if (name.length <5) {
        msg('Must be at least 5 characters');
        formValid.name = false; 
        checkValidation(); 
      } else {
        hide(); 
        formValid.name = true; 
        checkValidation(); 
		success();
      }
    }
  }
});


$('#username').on('input', function() {
  var username = $(this).val(); 

  
  function msg(body) {
    $('#username-error').text(body).show(); 
  };

  
  function hide() {
    $('#username-error').hide();
  };
  function error(){
  $('input[name=username]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=username]').removeClass("error").addClass("success");
  }
 
  if (username.length < 1) {
    msg('*This field is required.'); 
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
      msg('Username cannot contain special characters'); 
      formValid.username = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.username = true;
      checkValidation();
      
      if (username.length < 5) {
        msg('Must be at least 5 characters');
        formValid.username = false; 
        checkValidation(); 
      } else {
        hide(); 
        formValid.username = true; 
        checkValidation(); 
		success();
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
	 error();
    hide(); 
    formValid.password = true; 
	var testExp = new RegExp(/^(?=.*[A-Za-z])(?=.*\d)(?=.*[@$!%*#?&])[A-Za-z\d@$!%*#?&]{8,}$/); 
   
    if (!testExp.test(password)) {
      msg('Must contain AlphaNumeric values, an uppercase and a special character'); 
      formValid.password = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.password = true;
      checkValidation();
	  
	  
      if (password.length < 8) {
        msg('Must be at least 8 characters'); 
        formValid.password = false; 
        checkValidation(); 
      } else {
        hide();
        formValid.password = true; 
        checkValidation(); 
		success();
      }
    }
	
  
}
});


$('#password2').on('input', function() {
var password = $('#password').val();
var password2 = $(this).val(); 

 
  function msg(body) {
    $('#password2-error').text(body).show(); 
  };

  
  function hide() {
    $('#password2-error').hide(); 
  };

 function error(){
  $('input[name=password2]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=password2]').removeClass("error").addClass("success");
  }

  if (password2.length < 1) {
	error();
    msg('*This field is required.'); 
    formValid.password2 = false; 
    checkValidation(); 
  } else {
	error();
    hide(); 
    formValid.password2 = true; 
	  
	  
      if (password2.length < 5) {
        msg('Must be at least 5 characters'); 
        formValid.password2 = false; 
        checkValidation(); 
      } else {
        hide();
        formValid.password2 = true; 
        checkValidation(); 
			if(password2 !== password){
				msg('Passwords do not match'); 
				formValid.password2 = false; 
				checkValidation(); 
			} else {
				hide();
				formValid.password2 = true; 
				checkValidation(); 
				success();
			}
	  
	  
	  
	  }
    }
	
  
});


$('#email').on('input', function() {
  var email = $(this).val();


  function msg(body) {
    $('#email-error').text(body).show(); 
  };

 
  function hide() {
    $('#email-error').hide(); 
  };
 function error(){
  $('input[name=email]').removeClass("success").addClass("error");
  }
  function success(){
  $('input[name=email]').removeClass("error").addClass("success");
  }


  if (email.length < 1) {
	error();
    msg('*This field is required.'); 
    formValid.email = false;
    checkValidation();
  } else {
	error();
    hide(); 
    formValid.email = true; 
    checkValidation();
    var testExp = new RegExp(/[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,3}$/); 
    if (!testExp.test(email)) {
      msg('Must be a valid e-mail'); 
      formValid.email = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.email = true; 
      checkValidation(); 
		success();
	  }
  
}});
</script>
<?php include 'footer.php' ?>
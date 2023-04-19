<?php 
include 'header.php' ;
include 'database/connection.php';
include 'database/info-user.php';
include 'database/update.php';

	?>




<div id ="page_wrapper">
<div id = "edit-form">
        <form id="verification-form" method = "POST" action = "">
            
<h2>Edit your information</h2>

<div class ="form-control">

                <label for="name">Full Name:</label>
                <input type="text" name="name" id="name" autocomplete="off" placeholder ="<?php echo $name;?>"></input>
				<div class="message">
				<small id = "name-error" class="red"></small>
				</div>
</div>	
<div class ="form-control">	
                <label for="username">Username:</label>
                <input type="text" name="username" id="username" autocomplete="off" placeholder ="<?php echo $username;?>"></input>
				<div class="message">
				<small id = "username-error" class ="red"></small>
				</div>
</div>
<div class ="form-control"> 
                <label for="email">Email:</label>
                <input type="text" name="email" id="email" autocomplete="off" placeholder ="<?php echo $email;?>"></input>
				<div class="message">
				<small id = "email-error" class="red"></small>
				</div>
</div>


<h3>Please enter your password on file</h3>

<small>Changing your username will redirect you to the log in page!</small>

<div class ="form-control">
				<label for="password">Password:</label>
                <input type="password" name="password" id="password" autocomplete="off"></input>
				<div class="message">
				<small  id = "password-error" class ="red"></small>
				</div>    
</div>
<div class ="form-control">				
                <label for="password2">Re-enter Password:</label>
                <input type="password" name="password2" id="password2" autocomplete="off"></input>
				<div class="message">
				<small id = "password2-error" class ="red"></small>
				</div>
</div>

<div class ="form-control">    

      <button type="submit" id="submit" name="update" class="btn btn-lg btn-success" disabled>Update</button>
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
  
  if ((formValid.username || formValid.email || formValid.name) && ( formValid.password && formValid.password2)) {
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
  

 if(name.length==""){
	 hide();
	  formValid.name = true; 
 }else{
      if (name.length > 1 && name.length < 5) {
        msg('Must be at least 5 characters');
        formValid.name = false; 
        checkValidation(); 
	
      } else {
        hide(); 
        formValid.name = true; 
        checkValidation(); 
		
     
  
    var testExp = new RegExp(/^[a-z,',-]+(\s)[a-z,',-]+$/i); 
   
    if (!testExp.test(name)) {
	
      msg('Please Enter a valid name ex. "John Doe"'); 
      formValid.name = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.name = true;
      checkValidation();
      
    }
	  }
  }
}
);


$('#username').on('input', function() {
  var username = $(this).val(); 

  
  function msg(body) {
    $('#username-error').text(body).show(); 
  };

  
  function hide() {
    $('#username-error').hide();
  };
 
if(username.length==""){
	 hide();
      formValid.username = true;
 }else{
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
  
 }}
);



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
	var testExp = new RegExp(/^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?=.*[a-zA-Z]).{8,}$/); 
   
    if (!testExp.test(password)) {
      msg('Must contain AlphaNumeric values, uppercase, and special characters'); 
      formValid.password = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.password = true;
      checkValidation();
	  
	  
      if (password.length < 5 || password.length > 30) {
        msg('Must be at least 5 characters'); 
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
	  
	  
      if (password2.length < 5 || password2.length > 30) {
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
if(email.length==""){
	 hide();
	
 }else{

    var testExp = new RegExp(/[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,3}$/); 
    if (!testExp.test(email)) {
      msg('Must be a valid e-mail'); 
      formValid.email = false; 
      checkValidation(); 
    } else {
      hide(); 
      formValid.email = true; 
      checkValidation(); 
    
	  
	 
    
	

  }
  
}
});
</script>
<?php include 'footer.php'?>
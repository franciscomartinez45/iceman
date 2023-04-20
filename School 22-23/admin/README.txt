INSTRUCTIONS FOR VIEWING THE CONTENT:

This application was created using XAMPP localhost. This folder must be located inside of the htdocs folder.
At SQL WorkBench, (localhost/phpmyadmin), the database name is 'portal', a table of name 'users' must be created with 6 columns.
columns name, email, username, password and user_type in this order of type var char. Set all user_type for 'user'. The final column is named id 
and it is of type int(100) with an AUTO_INCREMENT extra. 

To Access

client-side: Create a profile that requires a name, email, username and password. Main content will be available upon logging in.

Admin-side: In workbench, create a user with only username and password of type name 'admin'. This will give access to the 
administrator content that displays the tables from portal.
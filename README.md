# OHA Database Migrator
A simple database migrator wirten by C.

### Supported data storage
* mysql

### Supported column handler
* column name map (e.g. id => ID)
* fixed value (e.g. age=0)
* fixed value to NULL (e.g. birthday=NULL)
* execute query to set column value (e.g. UserMark = SELECT MD5(CONCAT({id},{name},"abc")))
* execute php function to set column value (e.g. Password = encrypt_user_password({name}, {password}))

### Note
* when set column value handler to php function, you have to make sure that the `php` command is executable. 
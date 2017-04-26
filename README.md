# OHA Database Migrator
A simple database migrator wirten by C.

### Supported data storage
* mysql

### Supported column handler
* column name map (e.g. id => ID)
* fixed value (e.g. age=0)
* fixed value to NULL (e.g. birthday=NULL)
* execute query to set column value (e.g. UserMark = SELECT MD5(CONCAT({id},{name},"abc")))

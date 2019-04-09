Steps to run/test this project:

REQUIREMENTS:
1. Make sure to have MySQL Server/Workbench.
2. Import gyft_v1.sql file into DB or MySQLServer/Workbench


BUILD:
3.1. After building the project, paste inside the build folder:
 	a. libmysql.dll 
	b. qsqlmysql.dll
	
	Note: The project will fail to connect if both files is not insided  the build folder.
3.2. Also delete .user when building fresh project.
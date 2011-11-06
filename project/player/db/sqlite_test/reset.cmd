:: reset.cmd 9/12/2011
set DB=universe.sqlite
if exist "%DB%" del /q "%DB%"
sqlite3 "%DB%" < create_table.sql
sqlite3 "%DB%" < insert_table.sql
pause

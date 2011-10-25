rm -f universe.db
sqlite3 universe.db < create_table.sql
sqlite3 universe.db < insert_table.sql

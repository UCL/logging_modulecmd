logging_modulecmd
=================

A wrapper for modulecmd to log module usage, and an example tool put the logs into a MySQL database.

If you want to use this, you'll have to edit the paths at the top of modulecmd, and the dbDetails.c file for the MySQL injector tool.

You'll also need to set the MODULECMD_PATH environment variable for the init scripts.

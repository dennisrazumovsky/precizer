#include "precizer.h"

/**
 *
 * Initialize SQLite database
 *
 */
Return db_init(void)
{
	/// The status that will be passed to return() before exiting.
	/// By default, the function worked without errors.
	Return status = SUCCESS;

	int rc;

	// Don't do anything with default database in case of compare
	if(config->compare != true)
	{

#if 0 // Old multiPATH solution
const char *sql = "PRAGMA foreign_keys=OFF;" \
		                  "BEGIN TRANSACTION;" \
		                  "CREATE TABLE IF NOT EXISTS files("  \
		                  "ID INTEGER PRIMARY KEY NOT NULL," \
		                  "offset INTEGER DEFAULT NULL," \
		                  "path_prefix_index INTEGER NOT NULL," \
		                  "relative_path TEXT NOT NULL," \
		                  "sha512 BLOB DEFAULT NULL," \
		                  "stat BLOB DEFAULT NULL," \
		                  "mdContext BLOB DEFAULT NULL," \
		                  "CONSTRAINT full_path UNIQUE (path_prefix_index, relative_path) ON CONFLICT FAIL);" \
		                  "CREATE INDEX IF NOT EXISTS full_path_ASC ON files (path_prefix_index, relative_path ASC);" \
		                  "CREATE TABLE IF NOT EXISTS paths (" \
		                  "ID INTEGER PRIMARY KEY UNIQUE NOT NULL," \
		                  "prefix TEXT NOT NULL UNIQUE);" \
		                  "COMMIT;";
#endif

		/* Full runtime path is stored in the table 'paths' */
		const char *sql = "PRAGMA foreign_keys=OFF;" \
		                  "BEGIN TRANSACTION;" \
		                  "CREATE TABLE IF NOT EXISTS files("  \
		                  "ID INTEGER PRIMARY KEY NOT NULL," \
		                  "offset INTEGER DEFAULT NULL," \
		                  "relative_path TEXT UNIQUE NOT NULL," \
		                  "sha512 BLOB DEFAULT NULL," \
		                  "stat BLOB DEFAULT NULL," \
		                  "mdContext BLOB DEFAULT NULL);" \
		                  "CREATE UNIQUE INDEX IF NOT EXISTS 'TEXT_ASC' ON 'files' ('relative_path' ASC);" \
		                  "CREATE TABLE IF NOT EXISTS paths (" \
		                  "ID INTEGER PRIMARY KEY UNIQUE NOT NULL," \
		                  "prefix TEXT NOT NULL UNIQUE);" \
		                  "COMMIT;";

		/* Open database */
		if(sqlite3_open(config->db_file_name, &config->db)){
			slog(false,"Can't open database: %s\n", sqlite3_errmsg(config->db));
			status = FAILURE;
		}

		/* Execute SQL statement */
		rc = sqlite3_exec(config->db, sql, NULL, NULL, NULL);
		if(rc!= SQLITE_OK ){
			slog(false,"Can't execute (%i): %s\n", rc, sqlite3_errmsg(config->db));
			status = FAILURE;
		} else {
			slog(true,"The database has been successfully initialized\n");
		}

		sqlite3_close(config->db);

	}

	/* Open database to insert values */
	rc = sqlite3_open(config->db_file_name, &config->db);
	if(rc) {
		slog(false,"Can't execute (%i): %s\n", rc, sqlite3_errmsg(config->db));
		status = FAILURE;
	} else if(config->compare != true)
	{
		slog(true,"Opened database successfully\n");
	}

	const char *pragma_sql = "PRAGMA page_size = 4096;" \
	                         "PRAGMA cache_size = 524288;" \
	                         "PRAGMA journal_mode = OFF;" \
	                         "PRAGMA synchronous = OFF;";

	// Set SQLite pragmas
	rc = sqlite3_exec(config->db, pragma_sql, NULL, NULL, NULL);
	if(rc!= SQLITE_OK ){
		slog(false,"Can't execute (%i): %s\n", rc, sqlite3_errmsg(config->db));
		status = FAILURE;
	}

	const char *inmemory_db = "ATTACH DATABASE ':memory:' AS runtime_paths_id;" \
	                          "CREATE TABLE if not exists runtime_paths_id.the_path_id_does_not_exists" \
	                          "(path_id INTEGER UNIQUE NOT NULL);";

	rc = sqlite3_exec(config->db, inmemory_db, NULL, NULL, NULL);
	if(rc!= SQLITE_OK ){
		slog(false,"Can't execute (%i): %s\n", rc, sqlite3_errmsg(config->db));
		status = FAILURE;
	}

	return(status);
}

#include "precizer.h"

/**
 *
 * The structure Config where all runtime settings will be stored.
 * Initialization the structure elements by zerro.
 *
 */
void init_config(void)
{

	// Fill out with zerroes
	memset(config,0,sizeof(Config));

	// Max available size of a path
	config->running_dir_size = 0;

	// The pointer to the main database
	config->db = NULL;

	// Total size of all scanned files
	config->total_size_in_bytes = 0;

	// Absolute path name of the working directory
	// A directory where the program was executed
	config->running_dir = NULL;

	// Show progress bar
	config->progress = false;

	// Forse update of the database
	config->force = false;

	// Extra output
	config->verbose = false;

	// Force update of the database with new, changed or deleted files
	config->update = false;

	// Parameter to compare database
	config->compare = false;

	// An array of paths to scan
	config->paths = NULL;

	// Name of DB file
	config->db_file_name = NULL;

	// The flag meens that the DB already exists
	config->db_already_exists = false;

	// Flag that reflects the presence of any changes
	// since the last research
	config->something_has_been_changed = false;

	// Pointers to the array with database paths
	config->databases_to_compare = NULL;

	/// Pointers to the array with database file names
	config->filenames = NULL;

	//// Don't produce any output
	config->silent = false;
}
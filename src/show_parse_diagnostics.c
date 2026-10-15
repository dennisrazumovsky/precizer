#include "precizer.h"

/**
 * @brief Return the selected color mode name for configuration diagnostics
 *
 * The name describes the configured policy, not whether a particular output
 * stream currently permits terminal styling
 *
 * @return Static mode name, or "unknown" for an unsupported mode value
 */
static const char *color_mode_name(void)
{
	switch(rational_color_mode)
	{
		case COLOR_MODE_AUTO:
			return("auto");
		case COLOR_MODE_ALWAYS:
			return("always");
		case COLOR_MODE_NEVER:
			return("never");
		default:
			return("unknown");
	}
}

#ifdef TESTITALL_TEST_HOOKS
/**
 * @brief Show parsed configuration details for TESTING logger output
 *
 * @details
 * Test templates use these lines as stable key-value diagnostics. The function
 * prints only options that were explicitly set or values that are useful for
 * validating argument parsing behavior
 */
void show_testing_diagnostics(void)
{
	slog(TESTING,"rational_logger_mode=%s\n",rational_reconvert(rational_logger_mode));

	if(config->config_file_path != NULL)
	{
		slog(TESTING,"argument:config=%s\n",config->config_file_path);
	}

	slog(TESTING,"argument:color=%s\n",color_mode_name());

	if(config->roots.length != 0)
	{
		bool first_root = true;

		slog(TESTING,"argument:roots=");

		m_string_array_foreach(conf(roots),root)
		{
			slog(TESTING|UNDECOR,first_root == true ? "%s" : ", %s",m_text(root));
			first_root = false;
		}
		slog(TESTING|UNDECOR,"\n");
	}

	// Print configured database path when available
	if(conf(db_primary_file_path)->string_length > 0)
	{
		slog(TESTING,"argument:database=%s\n",confstr(db_primary_file_path));
	}

	// Print configured database file name when available
	if(conf(db_file_name)->string_length > 0)
	{
		slog(TESTING,"argument:db_file_name=%s\n",confstr(db_file_name));
	}

	if(config->db_file_paths.length != 0)
	{
		bool first_db_file_path = true;

		slog(TESTING,"argument:db_file_paths=");

		m_string_array_foreach(conf(db_file_paths),db_file_path)
		{
			slog(TESTING|UNDECOR,first_db_file_path == true ? "%s" : ", %s",m_text(db_file_path));
			first_db_file_path = false;
		}
		slog(TESTING|UNDECOR,"\n");
	}

	if(config->db_file_names != NULL)
	{
		slog(TESTING,"argument:db_file_names=");

		for(int i = 0; config->db_file_names[i]; i++)
		{
			slog(TESTING|UNDECOR,i == 0 ? "%s" : ", %s",config->db_file_names[i]);
		}
		slog(TESTING|UNDECOR,"\n");
	}

	if(config->ignore != NULL)
	{
		slog(TESTING,"argument:ignore=");

		// Print string-array contents
		for(int i = 0; config->ignore[i] != NULL; ++i)
		{
			slog(TESTING|UNDECOR,i == 0 ? "%s" : ", %s",config->ignore[i]);
		}
		slog(TESTING|UNDECOR,"\n");
	}

	if(config->include != NULL)
	{
		slog(TESTING,"argument:include=");

		// Print string-array contents
		for(int i = 0; config->include[i] != NULL; ++i)
		{
			slog(TESTING|UNDECOR,i == 0 ? "%s" : ", %s",config->include[i]);
		}
		slog(TESTING|UNDECOR,"\n");
	}

	if(config->lock_checksum != NULL)
	{
		slog(TESTING,"argument:lock-checksum=");

		// Print string-array contents
		for(int i = 0; config->lock_checksum[i] != NULL; ++i)
		{
			slog(TESTING|UNDECOR,i == 0 ? "%s" : ", %s",config->lock_checksum[i]);
		}
		slog(TESTING|UNDECOR,"\n");
	}

	if(config->db_check_level != FULL)
	{
		slog(TESTING,"argument:check-level=%s\n",config->db_check_level == QUICK ? "QUICK" : "FULL");
	}

	if(config->maxdepth > 0)
	{
		slog(TESTING,"argument:maxdepth=%d\n",config->maxdepth);
	}

	if(config->verbose)
	{
		slog(TESTING,"argument:verbose=%s\n",config->verbose ? "yes" : "no");
	}

	if(config->quiet_ignored)
	{
		slog(TESTING,"argument:quiet-ignored=%s\n",config->quiet_ignored ? "yes" : "no");
	}

	if(config->watch_timestamps)
	{
		slog(TESTING,"argument:watch-timestamps=%s\n",config->watch_timestamps ? "yes" : "no");
	}

	if(config->rehash_locked == true)
	{
		slog(TESTING,"argument:rehash-locked=%s\n",config->rehash_locked ? "yes" : "no");
	}

	if(config->force)
	{
		slog(TESTING,"argument:force=%s\n",config->force ? "yes" : "no");
	}

	if(config->update)
	{
		slog(TESTING,"argument:update=%s\n",config->update ? "yes" : "no");
	}

	if(config->progress)
	{
		slog(TESTING,"argument:progress=%s\n",config->progress ? "yes" : "no");
	}

	if(config->compare)
	{
		slog(TESTING,"argument:compare=%s\n",config->compare ? "yes" : "no");
	}

	if(config->compare_filter)
	{
		bool first_compare_filter = true;

		slog(TESTING,"argument:compare-filter=");

		if((config->compare_filter & CF_CHECKSUM_MISMATCH) != 0u)
		{
			slog(TESTING|UNDECOR,"%schecksum-mismatch",first_compare_filter ? "" : ", ");
			first_compare_filter = false;
		}

		if((config->compare_filter & CF_FIRST_SOURCE) != 0u)
		{
			slog(TESTING|UNDECOR,"%sfirst-source",first_compare_filter ? "" : ", ");
			first_compare_filter = false;
		}

		if((config->compare_filter & CF_SECOND_SOURCE) != 0u)
		{
			slog(TESTING|UNDECOR,"%ssecond-source",first_compare_filter ? "" : ", ");
		}

		slog(TESTING|UNDECOR,"\n");
	}

	if(config->db_drop_ignored)
	{
		slog(TESTING,"argument:db-drop-ignored=%s\n",config->db_drop_ignored ? "yes" : "no");
	}

	if(config->db_drop_inaccessible)
	{
		slog(TESTING,"argument:db-drop-inaccessible=%s\n",config->db_drop_inaccessible ? "yes" : "no");
	}

	if(config->dry_run)
	{
		slog(TESTING,"argument:dry-run=%s\n",config->dry_run_with_checksums ? "with-checksums" : "yes");
	}

	if(config->one_file_system)
	{
		slog(TESTING,"argument:one-file-system=%s\n",config->one_file_system ? "yes" : "no");
	}
}
#endif

/**
 * @brief Show parsed configuration details for verbose user output
 *
 * @details
 * Verbose output is a compact, single-line configuration summary for people
 * running the program interactively. It reports the same configuration values
 * as the TESTING diagnostics
 */
void show_verbose_diagnostics(void)
{
	slog(VERBOSE,"Configuration: ");
	slog(VERBOSE|UNDECOR,"rational_logger_mode=%s\n",rational_reconvert(rational_logger_mode));

	if(config->config_file_path != NULL)
	{
		slog(VERBOSE|UNDECOR,"config=%s; ",config->config_file_path);
	}

	if(config->roots.length != 0)
	{
		bool first_root = true;

		slog(VERBOSE|UNDECOR,"roots=");

		m_string_array_foreach(conf(roots),root)
		{
			slog(VERBOSE|UNDECOR,first_root == true ? "%s" : ", %s",m_text(root));
			first_root = false;
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	// Print configured database path when available
	if(conf(db_primary_file_path)->string_length > 0)
	{
		slog(VERBOSE|UNDECOR,"database=%s; ",confstr(db_primary_file_path));
	}

	// Print configured database file name when available
	if(conf(db_file_name)->string_length > 0)
	{
		slog(VERBOSE|UNDECOR,"db_file_name=%s; ",confstr(db_file_name));
	}

	if(config->db_file_paths.length != 0)
	{
		bool first_db_file_path = true;

		slog(VERBOSE|UNDECOR,"db_file_paths=");

		m_string_array_foreach(conf(db_file_paths),db_file_path)
		{
			slog(VERBOSE|UNDECOR,first_db_file_path == true ? "%s" : ", %s",m_text(db_file_path));
			first_db_file_path = false;
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	if(config->db_file_names != NULL)
	{
		slog(VERBOSE|UNDECOR,"db_file_names=");

		for(int i = 0; config->db_file_names[i]; i++)
		{
			slog(VERBOSE|UNDECOR,i == 0 ? "%s" : ", %s",config->db_file_names[i]);
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	if(config->ignore != NULL)
	{
		slog(VERBOSE|UNDECOR,"ignore=");

		// Print string-array contents
		for(int i = 0; config->ignore[i] != NULL; ++i)
		{
			slog(VERBOSE|UNDECOR,i == 0 ? "%s" : ", %s",config->ignore[i]);
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	if(config->include != NULL)
	{
		slog(VERBOSE|UNDECOR,"include=");

		// Print string-array contents
		for(int i = 0; config->include[i] != NULL; ++i)
		{
			slog(VERBOSE|UNDECOR,i == 0 ? "%s" : ", %s",config->include[i]);
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	if(config->lock_checksum != NULL)
	{
		slog(VERBOSE|UNDECOR,"lock-checksum=");

		// Print string-array contents
		for(int i = 0; config->lock_checksum[i] != NULL; ++i)
		{
			slog(VERBOSE|UNDECOR,i == 0 ? "%s" : ", %s",config->lock_checksum[i]);
		}
		slog(VERBOSE|UNDECOR,"; ");
	}

	const char *dry_run_mode = "no";

	if(config->dry_run == true)
	{
		if(config->dry_run_with_checksums == true)
		{
			dry_run_mode = "with-checksums";
		} else {
			dry_run_mode = "yes";
		}
	}

	slog(VERBOSE|UNDECOR,"verbose=%s; maxdepth=%d; silent=no; quiet-ignored=%s; force=%s; update=%s; watch-timestamps=%s; rehash-locked=%s; progress=%s; compare=%s, db-drop-ignored=%s, db-drop-inaccessible=%s, dry-run=%s, one-file-system=%s, check-level=%s, rational_logger_mode=%s, color=%s",
		config->verbose ? "yes" : "no",
		config->maxdepth,
		config->quiet_ignored ? "yes" : "no",
		config->force ? "yes" : "no",
		config->update ? "yes" : "no",
		config->watch_timestamps ? "yes" : "no",
		config->rehash_locked ? "yes" : "no",
		config->progress ? "yes" : "no",
		config->compare ? "yes" : "no",
		config->db_drop_ignored ? "yes" : "no",
		config->db_drop_inaccessible ? "yes" : "no",
		dry_run_mode,
		config->one_file_system ? "yes" : "no",
		config->db_check_level == QUICK ? "QUICK" : "FULL",
		rational_reconvert(rational_logger_mode),
		color_mode_name());

	if(config->compare_filter)
	{
		bool first_compare_filter = true;

		slog(VERBOSE|UNDECOR,"; compare-filter=");

		if((config->compare_filter & CF_CHECKSUM_MISMATCH) != 0u)
		{
			slog(VERBOSE|UNDECOR,"%schecksum-mismatch",first_compare_filter ? "" : ", ");
			first_compare_filter = false;
		}

		if((config->compare_filter & CF_FIRST_SOURCE) != 0u)
		{
			slog(VERBOSE|UNDECOR,"%sfirst-source",first_compare_filter ? "" : ", ");
			first_compare_filter = false;
		}

		if((config->compare_filter & CF_SECOND_SOURCE) != 0u)
		{
			slog(VERBOSE|UNDECOR,"%ssecond-source",first_compare_filter ? "" : ", ");
		}
	}

	slog(VERBOSE|UNDECOR,"\n");
}

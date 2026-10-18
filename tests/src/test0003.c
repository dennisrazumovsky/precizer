#include "sute.h"

/**
 * @brief Check the default hostname.db name and ignore scan roots during comparison
 *
 * @return Return status for database creation and both comparison scenarios
 */
Return test0003_1(void)
{
	INITTEST;

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	const char *arguments = "--progress tests/fixtures/diffs/diff1";

	ASSERT(SUCCESS == runit(arguments,NULL,NULL,COMPLETED,ALLOW_BOTH));

	arguments = "--progress --database=database2.db "
	        "tests/fixtures/diffs/diff2";

	ASSERT(SUCCESS == runit(arguments,NULL,NULL,COMPLETED,ALLOW_BOTH));

	// Get the output of the application
	const struct argument_test_case cases[] = {
		{"--compare $DBNAME database2.db",COMPLETED,"templates/0003_001.txt",STDERR_MUST_BE_EMPTY},
		/* Named roots must be discarded before compare validation and diagnostics */
		{"--path=/definitely/nonexistent/first --compare $DBNAME "
		 "--path=/definitely/nonexistent/second database2.db",COMPLETED,"templates/0003_001.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	const char *template = "%DB_NAME%";

	const char *replacement = getenv("DBNAME");  // Database name

	ASSERT(replacement != NULL);

	ASSERT(SUCCESS == run_and_match_templates(cases,template,replacement));

	// Clean up test results
	ASSERT(SUCCESS == delete_path(replacement));
	ASSERT(SUCCESS == delete_path("database2.db"));

	RETURN_STATUS;
}

/**
 * @brief Check usage output and successful completion without arguments
 *
 * @return Return status for the no-argument invocation
 */
Return test0003_2(void)
{
	INITTEST;

	const struct argument_test_case cases[] = {
		{"",COMPLETED,"templates/0003_003.txt","templates/0003_002.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	RETURN_STATUS;

}

/**
 * @brief Check that information modes complete successfully without PATH
 *
 * Run each command first without a default configuration file, then with an
 * invalid default file. Both runs must produce the same output and status.
 * Explicit file selections are also ignored for missing and malformed files
 *
 * @return Return status for help, usage, and version output
 */
Return test0003_3(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"-h",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"'-?'",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--usage",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"-z",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"-V",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"--version --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	const struct argument_test_case config_cases[] = {
		{"--config=0003_003.conf --help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--help --config=0003_003.conf",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"-g0003_003.conf -h",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"'-?g0003_003.conf'",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--config=0003_003.conf --usage",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"-z -g 0003_003.conf",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--config=0003_003.conf --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"-Vg0003_003.conf",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"--config=0003_003.conf --help --version",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--config=0003_003.conf --compare --help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--path=/definitely/nonexistent/path --config=0003_003.conf --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	/* Informational CLI requests must not read a malformed default file */
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	/* An explicit selection must not cause a conflict or a file access */
	ASSERT(SUCCESS == run_and_match_templates(config_cases,NULL,NULL));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"0003_003.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(config_cases,NULL,NULL));
	call(delete_path_if_present("0003_003.conf"));
	call(delete_path_if_present("precizer.conf"));

	RETURN_STATUS;
}

/**
 * @brief Check that information modes ignore PATH and stop before the main workflow
 *
 * @return Return status for information modes combined with paths and comparison
 */
Return test0003_4(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--help /definitely/nonexistent/path",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"-h /definitely/nonexistent/path",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--usage /definitely/nonexistent/path",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"-z /definitely/nonexistent/path",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--version /definitely/nonexistent/path",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"--compare --help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--compare --usage",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--compare -z",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--compare --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"--help --version",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--path=/definitely/nonexistent/path --help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--path=/definitely/nonexistent/path --usage",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--path=/definitely/nonexistent/path --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	RETURN_STATUS;
}

/**
 * @brief Require named and positional roots to produce the same traversal output
 *
 * Run each command first without a default configuration file, then with an
 * invalid default file. Both runs must produce the same output and status
 *
 * @return Return status for long and short named options and positional roots
 */
static Return test0003_5(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--dry-run --database=dry_run_regular.db tests/fixtures/diffs/diff1",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db --path=tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db --path tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db -P tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	/* Ordinary CLI operations must not read a malformed default file */
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	call(delete_path_if_present("precizer.conf"));

	RETURN_STATUS;
}

/**
 * @brief Preserve repeated roots and combine named roots with positional roots
 *
 * Named options are processed before positional arguments by argp. The full
 * output must match the equivalent positional root array, including duplicates
 *
 * @return Return status for repeated, mixed, and normalized root spellings
 */
static Return test0003_6(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--dry-run --database=0003_paths.db "
		 "--path=tests/fixtures/diffs/diff1/1/ --path=tests/fixtures/diffs/diff1/4 "
		 "--path=tests/fixtures/diffs/diff1/1",COMPLETED,"templates/0003_007.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=0003_paths.db "
		 "--path=tests/fixtures/diffs/diff1/1/ --path tests/fixtures/diffs/diff1/4/ "
		 "tests/fixtures/diffs/diff1/1/",COMPLETED,"templates/0003_007.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=0003_paths.db tests/fixtures/diffs/diff1/1/ "
		 "--path=tests/fixtures/diffs/diff1/1 --path=tests/fixtures/diffs/diff1/4",COMPLETED,"templates/0003_007.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	RETURN_STATUS;
}

/**
 * @brief Reject missing roots, missing path values, and missing compare operands
 *
 * Empty named and positional roots are invalid. Named roots cannot substitute
 * for either positional database operand required by compare mode. Missing
 * scan roots produce a hint for positional paths and --path
 *
 * @return Return status for complete stdout and stderr diagnostics
 */
static Return test0003_7(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--dry-run",FAILURE,"templates/0028_006_14.txt","templates/0003_007_1.txt"},
		{"--",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"-- tests/fixtures/diffs/diff1",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--path=tests/fixtures/diffs/diff1 --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--compare --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--version --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--path",FAILURE,"templates/0040_003.txt","templates/0003_008.txt"},
		{"--path=",FAILURE,"templates/0040_003.txt","templates/0003_010.txt"},
		{"--dry-run ''",FAILURE,"templates/0040_003.txt","templates/0003_010.txt"},
		{"--compare --path='' first.db second.db",FAILURE,"templates/0040_003.txt","templates/0003_010.txt"},
		{"--compare --path=first.db --path=second.db",FAILURE,"templates/0028_006_14.txt","templates/0003_002.txt"},
		{"--compare --path=second.db first.db",FAILURE,"templates/0028_006_14.txt","templates/0003_011.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	RETURN_STATUS;
}

/**
 * @brief Select the default file only for an argument-free invocation
 *
 * Missing scan roots produce a hint to add the path key to the configuration
 * file, whether it was selected automatically or explicitly
 *
 * @return Return status for empty, documented, incomplete, and populated files
 */
static Return test0003_8(void)
{
	INITTEST;
	const struct argument_test_case empty_file[] = {
		{"",COMPLETED,"templates/0003_003.txt","templates/0003_002.txt"},
		{"--config=precizer.conf",FAILURE,"templates/0028_006_14.txt","templates/0003_008_1.txt"},
		{0}
	};
	const struct argument_test_case missing_root[] = {
		{"",FAILURE,"templates/0028_006_14.txt","templates/0003_008_1.txt"},
		{"--config=precizer.conf",FAILURE,"templates/0028_006_14.txt","templates/0003_008_1.txt"},
		{0}
	};
	const struct argument_test_case configured_root[] = {
		{"",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_008_1.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(empty_file,NULL,NULL));
	/* The documented default must remain safe until a setting is uncommented */
	ASSERT(SUCCESS == copy_from_origin("precizer.conf","precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(empty_file,NULL,NULL));
	/* A false flag is still a supplied setting, so a scan root is required */
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_008_2.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(missing_root,NULL,NULL));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(configured_root,"%CONFIG_PATH%","precizer[.]conf"));

	call(delete_path_if_present("precizer.conf"));
	RETURN_STATUS;
}

/**
 * @brief Read only the last explicit file and resolve its relative roots from the working directory
 * @return Return status for long, short, attached, separate, and repeated selectors
 */
static Return test0003_9(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--config=tests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"--config tests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"-g tests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"-gtests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"--config=0003_absent.conf -gtests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"-gprecizer.conf --config=tests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"--config -- -g tests/templates/0003_012.conf",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","tests/templates/0003_012.conf",REQUIRE_SOURCE_EXISTS));
	/* A root relative to the config directory would incorrectly find this directory */
	ASSERT(SUCCESS == create_directory("tests/templates/0003_missing_root"));
	ASSERT(SUCCESS == run_and_match_templates(cases,"%CONFIG_PATH%","tests/templates/0003_012[.]conf"));

	call(delete_path_if_present("tests/templates/0003_missing_root"));
	call(delete_path_if_present("tests/templates/0003_012.conf"));
	call(delete_path_if_present("precizer.conf"));
	RETURN_STATUS;
}

/**
 * @brief Accept config filenames that resemble options or contain spaces
 * @return Return status for literal option-like values and quoted paths
 */
static Return test0003_10(void)
{
	INITTEST;
	const struct argument_test_case delimiter_name[] = {
		{"--config=--",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{"-g--",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	const struct argument_test_case relative_name[] = {
		{"--config ./--",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	const struct argument_test_case option_name[] = {
		{"--config --help",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	const struct argument_test_case spaced_name[] = {
		{"-g '0003 config.conf'",FAILURE,"templates/0003_012.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","--",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(delimiter_name,"%CONFIG_PATH%","--"));
	ASSERT(SUCCESS == run_and_match_templates(relative_name,"%CONFIG_PATH%","[.]/--"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","--help",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(option_name,"%CONFIG_PATH%","--help"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","0003 config.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(spaced_name,"%CONFIG_PATH%","0003 config[.]conf"));

	call(delete_path_if_present("0003 config.conf"));
	call(delete_path_if_present("--help"));
	call(delete_path_if_present("--"));
	RETURN_STATUS;
}

/**
 * @brief Reject explicit file selection combined with other CLI arguments before opening the file
 *
 * Check all selector forms, both argument orders, repeated selections, and a
 * short-option group. A missing file and a malformed file must produce the
 * same source-conflict diagnostic without attempting to read settings
 *
 * @return Return status for operational and positional conflicts
 */
static Return test0003_11(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--config=0003_011.conf --path=0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--path=0003_missing_root --config=0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--config=0003_011.conf 0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"0003_missing_root --config=0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--config 0003_011.conf --path=0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--path=0003_missing_root --config 0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--config 0003_011.conf 0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"0003_missing_root --config 0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"-g 0003_011.conf --path=0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--path=0003_missing_root -g 0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"-g 0003_011.conf 0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"0003_missing_root -g 0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"-g0003_011.conf --path=0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--path=0003_missing_root -g0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"-g0003_011.conf 0003_missing_root",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"0003_missing_root -g0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"-vg0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"--config=0003_absent.conf --path=0003_missing_root -g0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{"0003_missing_root -g0003_absent.conf --config=0003_011.conf",FAILURE,"templates/0003_013.txt","templates/0003_014.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

	/* Invalid settings must not replace the conflict with a configuration error */
	ASSERT(SUCCESS == copy_path("tests/templates/0003_011.conf","0003_011.conf"));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	call(delete_path_if_present("0003_011.conf"));

	RETURN_STATUS;
}

/**
 * @brief Defer invalid CLI diagnostics to the main parser without reading a file
 * @return Return status for missing values, unknown options, forbidden option values, and argp-marked --
 */
static Return test0003_12(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--config",FAILURE,"templates/0040_003.txt","templates/0003_015.txt"},
		{"-g",FAILURE,"templates/0040_003.txt","templates/0003_016.txt"},
		{"--config=0003_absent.conf --path",FAILURE,"templates/0040_003.txt","templates/0003_008.txt"},
		{"--config=0003_absent.conf --unknown",FAILURE,"templates/0040_003.txt","templates/0003_017.txt"},
		{"--version --config=0003_absent.conf --unknown",FAILURE,"templates/0040_003.txt","templates/0003_017.txt"},
		{"--config=0003_absent.conf --version --color=invalid-value",FAILURE,"templates/0040_003.txt","templates/0040_004.txt"},
		{"--config=0003_absent.conf --progress=no",FAILURE,"templates/0040_003.txt","templates/0003_018.txt"},
		{"--",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"-- tests/fixtures/diffs/diff1",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--path=tests/fixtures/diffs/diff1 --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--compare --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--version --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--config --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"-g --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--config=0003_absent.conf --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--config -- --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"-g -- --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"--config=-- --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{"-g-- --",FAILURE,"templates/0028_006_14.txt","templates/0003_024.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	call(delete_path_if_present("precizer.conf"));
	RETURN_STATUS;
}

/**
 * @brief Stop startup on configuration open, read, and parsing errors without fallback
 * @return Return status for default and explicitly selected file errors
 */
static Return test0003_13(void)
{
	INITTEST;
	const struct argument_test_case missing_file[] = {
		{"--config=0003_absent.conf",FAILURE,"templates/0003_013.txt","templates/0003_019.txt"},
		{"--config=",FAILURE,"templates/0003_013.txt","templates/0003_020.txt"},
		{0}
	};
	const struct argument_test_case invalid_file[] = {
		{"",FAILURE,"templates/0003_013.txt","templates/0003_021.txt"},
		{"--config=precizer.conf",FAILURE,"templates/0003_013.txt","templates/0003_021.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_012.conf","precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(missing_file,NULL,NULL));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_003.conf",
		"precizer.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(invalid_file,NULL,NULL));
	call(delete_path_if_present("precizer.conf"));

	/* A directory opens successfully but causes a read error when inih reads it */
	const struct argument_test_case unreadable_file[] = {
		{"",FAILURE,"templates/0003_013.txt","templates/0003_022.txt"},
		{"--config=precizer.conf",FAILURE,"templates/0003_013.txt","templates/0003_022.txt"},
		{"--version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};
	ASSERT(SUCCESS == create_directory("precizer.conf"));
	ASSERT(SUCCESS == run_and_match_templates(unreadable_file,NULL,NULL));
	call(delete_path_if_present("precizer.conf"));

	RETURN_STATUS;
}

/**
 * @brief Include the selected file path in verbose configuration diagnostics
 * @return Return status for the complete verbose startup output
 */
static Return test0003_14(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--config=tests/templates/0003_014.conf",FAILURE,"templates/0003_023.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0003_014.conf","tests/templates/0003_014.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	call(delete_path_if_present("tests/templates/0003_014.conf"));
	RETURN_STATUS;
}

Return test0003(void)
{
	INITTEST;

	TEST(test0003_1,"Comply default DB name to \"hostname.db\" template");
	TEST(test0003_2,"Running the application with no arguments at all");
	TEST(test0003_3,"Information modes without PATH return success");
	TEST(test0003_4,"Information modes ignore PATH and stop early");
	TEST(test0003_5,"Named roots match positional traversal");
	TEST(test0003_6,"Repeated and positional roots are combined without removing duplicates");
	TEST(test0003_7,"Missing or empty roots and missing compare operands are rejected");
	TEST(test0003_8,"Default configuration distinguishes empty files from incomplete settings");
	TEST(test0003_9,"Explicit configuration forms select only the last file");
	TEST(test0003_10,"Config values accept option-like filenames and spaces");
	TEST(test0003_11,"Explicit configuration conflicts with other CLI arguments before reading");
	TEST(test0003_12,"Invalid CLI arguments and argp-marked -- are reported without reading configuration");
	TEST(test0003_13,"Configuration errors stop startup without fallback");
	TEST(test0003_14,"Verbose diagnostics identify the selected configuration file");

	RETURN_STATUS;
}

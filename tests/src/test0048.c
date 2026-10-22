#include "sute.h"

#define OUTPUT_DRY_RUN_ARGUMENTS "--color=never --dry-run --database=0040.db tests/fixtures/diffs/diff1/2"
#define MACHINE_DRY_RUN_ARGUMENTS "--color=never --dry-run --database=0048.db 0048_empty"

/**
 * @brief Accept long and short output options with separate or attached values
 * @return Return status for complete dry-run output and empty stderr
 */
static Return test0048_1(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--output=full " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output=FULL " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"-OFULL " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output FuLl " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output full " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output=machine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"--output=MACHINE " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"-OMaChInE " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"--output MaChInE " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"--output machine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"--output=machine --output=full " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output=full --output=machine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"-O full " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"-Ofull " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"-O machine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"-Omachine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{"-Omachine --output=full " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0040_002.txt",STDERR_MUST_BE_EMPTY},
		{"--output=full -Omachine " OUTPUT_DRY_RUN_ARGUMENTS,COMPLETED,"templates/0045_033.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == run_and_match_templates(cases,"%COLOR_MODE%","never"));
	RETURN_STATUS;
}

/**
 * @brief Reject unknown, empty, and missing CLI output values
 * @return Return status for complete parsing diagnostics on both streams
 */
static Return test0048_2(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--output=invalid-value",FAILURE,"templates/0040_003.txt","templates/0048_001.txt"},
		{"--output invalid-value",FAILURE,"templates/0040_003.txt","templates/0048_001.txt"},
		{"--output=",FAILURE,"templates/0040_003.txt","templates/0048_002.txt"},
		{"--output ''",FAILURE,"templates/0040_003.txt","templates/0048_002.txt"},
		{"--output",FAILURE,"templates/0040_003.txt","templates/0048_004.txt"},
		{"-O invalid-value",FAILURE,"templates/0040_003.txt","templates/0048_001.txt"},
		{"-Oinvalid-value",FAILURE,"templates/0040_003.txt","templates/0048_001.txt"},
		{"-O ''",FAILURE,"templates/0040_003.txt","templates/0048_002.txt"},
		{"-O",FAILURE,"templates/0040_003.txt","templates/0048_005.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	RETURN_STATUS;
}

/**
 * @brief Hide startup, traversal headings, and final statistics in ordinary machine runs
 *
 * An empty root produces no file events. TESTING=false exercises normal
 * initialization without the test-only trace emitted before argument parsing
 *
 * @return Return status for empty streams and dry-run database absence
 */
static Return test0048_3(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--output=machine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-sSNeVeR -oOMaChInE " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-vOMaChInE --silent " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--verbose --output=MaChInE " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-Omachine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=machine --verbose " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--verbose --output=machine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-vOmachine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=machine --silent " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--silent --output=machine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=machine --verbose --silent " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=machine --silent --verbose " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--verbose --silent --output=machine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--silent --verbose --output=machine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=full -Omachine " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-Omachine --dry-run=with-checksums --color=never --database=0048.db 0048_empty",COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=full --silent --verbose " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"--output=full --verbose --silent " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-sv " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"-vs " MACHINE_DRY_RUN_ARGUMENTS,COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == create_directory("0048_empty"));
	ASSERT(SUCCESS == set_environment_variable("TESTING","false"));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	ASSERT(SUCCESS == expect_file_access_from_root(".","0048.db",F_OK,FILE_NOT_FOUND));
	call(set_environment_variable("TESTING","true"));
	call(delete_path_if_present("0048_empty"));
	RETURN_STATUS;
}

/**
 * @brief Apply output priorities before startup for explicit and default configuration files
 * @return Return status for empty output from configured dry runs
 */
static Return test0048_4(void)
{
	INITTEST;
	const char *config_templates[] = {
		"tests/templates/0048_006.conf",
		"tests/templates/0048_007.conf",
		"tests/templates/0048_008.conf",
		"tests/templates/0048_009.conf",
		"tests/templates/0048_010.conf",
		"tests/templates/0048_011.conf",
		"tests/templates/0048_012.conf",
		"tests/templates/0048_013.conf"
	};
	const struct argument_test_case cases[] = {
		{"--config=precizer.conf",COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{"",COMPLETED,STDOUT_MUST_BE_EMPTY,STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == create_directory("0048_empty"));
	ASSERT(SUCCESS == set_environment_variable("TESTING","false"));

	for(size_t i = 0; i < sizeof(config_templates) / sizeof(config_templates[0]) && SUCCESS == status; ++i)
	{
		ASSERT(SUCCESS == copy_path(config_templates[i],"precizer.conf"));
		ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	}
	ASSERT(SUCCESS == expect_file_access_from_root(".","0048.db",F_OK,FILE_NOT_FOUND));
	call(set_environment_variable("TESTING","true"));
	call(delete_path_if_present("precizer.conf"));
	call(delete_path_if_present("0048_empty"));
	RETURN_STATUS;
}

/**
 * @brief Retain technical errors while machine output suppresses service messages
 *
 * Silent takes priority regardless of option order and suppresses logger
 * errors, while argp diagnostics remain on stderr
 *
 * @return Return status for complete diagnostics and successful return to full output
 */
static Return test0048_5(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--output=machine --color=never --output=invalid-value",FAILURE,"templates/0048_006.txt","templates/0048_001.txt"},
		{"--verbose -Omachine --color=never --output=invalid-value",FAILURE,"templates/0048_006.txt","templates/0048_001.txt"},
		{"-Omachine --verbose --color=never --output=invalid-value",FAILURE,"templates/0048_006.txt","templates/0048_001.txt"},
		{"-Omachine --verbose --silent --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"-Omachine --silent --verbose --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--silent -Omachine --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--verbose -Omachine --silent --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--verbose --silent -Omachine --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--silent -Omachine --verbose --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--silent --verbose -Omachine --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--output=full --silent --verbose --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"--output=full --verbose --silent --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"-sv --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"-vs --color=never --output=invalid-value",FAILURE,STDOUT_MUST_BE_EMPTY,"templates/0048_001.txt"},
		{"-Omachine --output=full --color=never --version",COMPLETED,"templates/0048_007.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","false"));
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	call(set_environment_variable("TESTING","true"));
	RETURN_STATUS;
}

/**
 * @brief Exercise output format selection through application startup
 * @return Return status for format selection, logger filtering, and parsing diagnostics
 */
Return test0048(void)
{
	INITTEST;
	TEST(test0048_1,"Long and short output options accept repeated selections");
	TEST(test0048_2,"CLI output formats reject unknown, empty, and missing values");
	TEST(test0048_3,"Machine CLI output hides service messages during dry runs");
	TEST(test0048_4,"Explicit and default configuration select output priorities before startup");
	TEST(test0048_5,"Machine errors honor logger priorities and full output can be restored");
	RETURN_STATUS;
}

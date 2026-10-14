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
 * @return Return status for help, usage, and version output
 */
Return test0003_3(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--help",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"-h",COMPLETED,"templates/0003_004.txt",STDERR_MUST_BE_EMPTY},
		{"--usage",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"-z",COMPLETED,"templates/0003_005.txt",STDERR_MUST_BE_EMPTY},
		{"--version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{"--version --version",COMPLETED,"templates/0003_006.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

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
 * @return Return status for both named value spellings and the option terminator
 */
static Return test0003_5(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--dry-run --database=dry_run_regular.db tests/fixtures/diffs/diff1",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db --path=tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db --path tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db -P tests/fixtures/diffs/diff1/",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{"--dry-run --database=dry_run_regular.db --path=tests/fixtures/diffs/diff1 --",COMPLETED,"templates/0013_001_1.txt",STDERR_MUST_BE_EMPTY},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));

	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));

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
 * for either positional database operand required by compare mode
 *
 * @return Return status for complete stdout and stderr diagnostics
 */
static Return test0003_7(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--dry-run",FAILURE,"templates/0028_006_14.txt","templates/0003_002.txt"},
		{"--",FAILURE,"templates/0028_006_14.txt","templates/0003_002.txt"},
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

Return test0003(void)
{
	INITTEST;

	TEST(test0003_1,"Comply default DB name to \"hostname.db\" template");
	TEST(test0003_2,"Running the application with no arguments at all");
	TEST(test0003_3,"Information modes without PATH return success");
	TEST(test0003_4,"Information modes ignore PATH and stop early");
	TEST(test0003_5,"Named roots match positional traversal and accept an option terminator");
	TEST(test0003_6,"Repeated and positional roots are combined without removing duplicates");
	TEST(test0003_7,"Missing or empty roots and missing compare operands are rejected");

	RETURN_STATUS;
}

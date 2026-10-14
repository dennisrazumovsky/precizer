#include "sute.h"

/**
 *
 * Testing symlink operations
 *
 * Test Scenario:
 * 1. Add symlinks, create database
 * 2. Remove symlinks, update database
 * 3. Add symlinks, update database
 *
 */
Return test0019(void)
{
	INITTEST;

	const char *update_arguments = "--update --check-level=QUICK --database=database1.db tests/fixtures/diffs/diff1";

	// Preparation for the test
	ASSERT(SUCCESS == prepare_mutable_fixture("tests/fixtures/diffs/diff1"));

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == create_symlink("../../../../1/AAA/BCB/CCC/a.txt","tests/fixtures/diffs/diff1/path1/AAA/BCB/CCC/symlink_to_the_file_a.txt"));
	ASSERT(SUCCESS == create_symlink("../../../../AAA/ZAW/D/e/f","tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/symlink_to_dir_f"));
	ASSERT(SUCCESS == create_symlink("/to/nowhere","tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/broken_symlink"));

	const char *arguments = "--database=database1.db tests/fixtures/diffs/diff1";

	const struct argument_test_case initial_cases[] = {
		{arguments,COMPLETED,"templates/0019_001.txt","templates/empty_regex_pattern.txt"},
		{0}
	};

	ASSERT(SUCCESS == run_and_match_templates(initial_cases,NULL,NULL));

	ASSERT(SUCCESS == delete_path("tests/fixtures/diffs/diff1/path1/AAA/BCB/CCC/symlink_to_the_file_a.txt"));
	ASSERT(SUCCESS == delete_path("tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/symlink_to_dir_f"));
	ASSERT(SUCCESS == delete_path("tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/broken_symlink"));

	const struct argument_test_case removed_symlinks_cases[] = {
		{update_arguments,COMPLETED,"templates/0019_002.txt","templates/empty_regex_pattern.txt"},
		{0}
	};

	ASSERT(SUCCESS == run_and_match_templates(removed_symlinks_cases,NULL,NULL));

	ASSERT(SUCCESS == create_symlink("../../../../1/AAA/BCB/CCC/a.txt","tests/fixtures/diffs/diff1/path1/AAA/BCB/CCC/symlink_to_the_file_a.txt"));
	ASSERT(SUCCESS == create_symlink("../../../../AAA/ZAW/D/e/f","tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/symlink_to_dir_f"));
	ASSERT(SUCCESS == create_symlink("/to/nowhere","tests/fixtures/diffs/diff1/path1/AAA/ZAW/A/b/broken_symlink"));

	const struct argument_test_case restored_symlinks_cases[] = {
		{update_arguments,COMPLETED,"templates/0019_003.txt","templates/empty_regex_pattern.txt"},
		{0}
	};

	ASSERT(SUCCESS == run_and_match_templates(restored_symlinks_cases,NULL,NULL));

	// Clean up test results
	ASSERT(SUCCESS == delete_path("database1.db"));
	ASSERT(SUCCESS == restore_mutable_fixture("tests/fixtures/diffs/diff1"));

	RETURN_STATUS;
}

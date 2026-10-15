#include "sute.h"

#ifdef __linux__
/**
 * @brief Stop startup when a read error follows valid configuration settings
 *
 * The fixture represents the prefix read before an I/O failure. The ferror
 * mock reports that failure after inih applies the prefix. Complete output
 * matching rejects any traversal output, and the database must remain absent
 *
 * @return Return status for startup failure without a database being created
 */
Return test0046(void)
{
	INITTEST;
	const struct argument_test_case cases[] = {
		{"--config=0046_001.conf",FAILURE,"templates/0003_013.txt","templates/0046_001.txt"},
		{0}
	};

	ASSERT(SUCCESS == set_environment_variable("TESTING","true"));
	ASSERT(SUCCESS == copy_from_origin("tests/templates/0046_001.conf",
		"0046_001.conf",REQUIRE_SOURCE_EXISTS));
	ASSERT(SUCCESS == expect_file_access_from_root(".","0046_001.db",F_OK,FILE_NOT_FOUND));

	mocks_ferror_reset();
	mocks_ferror_set_target_suffix("0046_001.conf");
	ASSERT(SUCCESS == run_and_match_templates(cases,NULL,NULL));
	ASSERT(mocks_ferror_call_count() == 1U);
	mocks_ferror_reset();

	ASSERT(SUCCESS == expect_file_access_from_root(".","0046_001.db",F_OK,FILE_NOT_FOUND));
	call(delete_path_if_present("0046_001.db"));
	call(delete_path_if_present("0046_001.conf"));
	RETURN_STATUS;
}
#endif

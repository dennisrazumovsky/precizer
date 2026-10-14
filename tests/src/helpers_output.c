#include "sute.h"

/**
 * @brief Run application scenarios in order and check their exit codes and selected output streams
 *
 * The cases array must end with a zero-initialized record whose arguments is
 * NULL. An empty arguments string is a valid scenario that runs the application
 * without arguments. Execution stops at the terminator or the first failure
 *
 * Each non-NULL stdout_template or stderr_template names a file containing
 * the expected PCRE2 pattern. NULL skips comparison of that stream. The shared
 * templates/empty_regex_pattern.txt pattern requires the selected stream to contain no bytes.
 * Optional substitution applies to all scenarios and changes each loaded pattern
 * in memory, leaving the template files unchanged. Environment and fixture setup
 * belong to the caller
 *
 * @param[in] cases Scenario array terminated by a record with NULL arguments
 * @param[in] template Text to replace in the loaded patterns, or NULL to skip substitution
 * @param[in] replacement Replacement text, required when template is not NULL
 * @return SUCCESS when all scenarios match their expected exit codes and selected output streams
 */
Return run_and_match_templates(
	const struct argument_test_case *cases,
	const char                      *template,
	const char                      *replacement)
{
	/* Status returned by this function through deliver()
	   Default value assumes successful completion */
	Return status = SUCCESS;

	for(size_t i = 0; cases[i].arguments != NULL && SUCCESS == status; i++)
	{
		const struct argument_test_case *test_case = &cases[i];

		m_create(char,stdout_result,MEMORY_STRING);
		m_create(char,stderr_result,MEMORY_STRING);
		m_create(char,pattern,MEMORY_STRING);

		run(runit(test_case->arguments,stdout_result,stderr_result,
			test_case->expected_return_code,STDERR_ALLOW));

		if(test_case->stdout_template != NULL)
		{
			run(get_file_content(test_case->stdout_template,pattern));

			if(template != NULL)
			{
				run(replace_placeholder(pattern,template,replacement));
			}
			run(match_pattern(stdout_result,pattern,test_case->stdout_template));
		}

		if(test_case->stderr_template != NULL)
		{
			run(get_file_content(test_case->stderr_template,pattern));

			if(template != NULL)
			{
				run(replace_placeholder(pattern,template,replacement));
			}
			run(match_pattern(stderr_result,pattern,test_case->stderr_template));
		}

		call(m_del(pattern));
		call(m_del(stderr_result));
		call(m_del(stdout_result));
	}

	deliver(status);
}

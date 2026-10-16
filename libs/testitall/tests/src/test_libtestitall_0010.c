#include "test_libtestitall_all.h"
#include <wordexp.h>

/**
 * @brief Replace repeated file placeholders while preserving the remaining text
 *
 * Longer, shorter, and empty replacements exercise file growth and truncation.
 * Unrelated placeholders, comments, and the missing final newline stay intact.
 * Internal quoting preserves spaces, quotes, and backslashes in filenames and
 * replacement text while allowing environment expansion. Missing files and
 * command substitutions must fail without writing a file
 *
 * @return Return status code
 */
Return test_libtestitall_0010(void)
{
	INITTEST;

	const char original_text[] =
	        "# Keep this comment\npath = %ROOT_PATH%\npath = %ROOT_PATH%\nother = %OTHER%";
	const char *filename_expression = "$TESTITALL_REPLACEMENT_FILE";
	const char *previous_value = getenv("TESTITALL_REPLACEMENT_FILE");
	char *saved_value = NULL;
	FILE *file = NULL;
	bool file_created = false;
	m_create(char,temporary_directory,MEMORY_STRING);
	m_create(char,file_path,MEMORY_STRING);
	m_create(char,actual_text,MEMORY_STRING);
	m_create(char,expected_error,MEMORY_STRING);
	m_create(char,expected_expanded_text,MEMORY_STRING);

	if(previous_value != NULL)
	{
		saved_value = strdup(previous_value);
		ASSERT(saved_value != NULL);
	}
	const bool environment_saved = SUCCESS == status;
	ASSERT(SUCCESS == create_tmpdir(temporary_directory));
	const bool directory_created = SUCCESS == status;
	ASSERT(SUCCESS == m_formatted_string(file_path,"%s/settings \"quoted\"\\file.conf",m_text(temporary_directory)));
	ASSERT(setenv("TESTITALL_REPLACEMENT_FILE",m_text(file_path),1) == 0);
	ASSERT(SUCCESS == m_formatted_string(expected_expanded_text,
		"# Keep this comment\npath = %s\npath = %s\nother = %%OTHER%%",
		m_text(file_path),m_text(file_path)));

	/* Reading a missing file must fail before any output file is opened */
	call(m_del(STDERR));
	ASSERT(FAILURE & replace_placeholder_in_file(filename_expression,"%ROOT_PATH%","x"));
	ASSERT(SUCCESS == m_formatted_string(expected_error,
		"Failed to open pattern file %s\n",m_text(file_path)));
	ASSERT(strcmp(m_text(STDERR),m_text(expected_error)) == 0);
	ASSERT(access(m_text(file_path),F_OK) == -1);
	if(SUCCESS == status)
	{
		call(m_del(STDERR));
	}

	const struct {
		const char *filename;
		const char *replacement;
		const char *expected_text;
	} cases[] = {
		{m_text(file_path),"/tmp/storage with spaces/данные",
		 "# Keep this comment\npath = /tmp/storage with spaces/данные\n"
		 "path = /tmp/storage with spaces/данные\nother = %OTHER%"},
		{m_text(file_path),"x","# Keep this comment\npath = x\npath = x\nother = %OTHER%"},
		{m_text(file_path),"","# Keep this comment\npath = \npath = \nother = %OTHER%"},
		{m_text(file_path),"\"double\" 'single' \\tail\\",
		 "# Keep this comment\npath = \"double\" 'single' \\tail\\\n"
		 "path = \"double\" 'single' \\tail\\\nother = %OTHER%"},
		{m_text(file_path),"*?[ab] ; | < > &",
		 "# Keep this comment\npath = *?[ab] ; | < > &\n"
		 "path = *?[ab] ; | < > &\nother = %OTHER%"},
		{filename_expression,"$TESTITALL_REPLACEMENT_FILE",m_text(expected_expanded_text)}
	};

	for(size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]) && SUCCESS == status; ++i)
	{
		file = fopen(m_text(file_path),"wb");
		ASSERT(file != NULL);
		if(file != NULL)
		{
			file_created = true;
			ASSERT(fputs(original_text,file) != EOF);
			const int close_status = fclose(file);
			file = NULL;
			ASSERT(close_status == 0);
		}

		ASSERT(SUCCESS == replace_placeholder_in_file(cases[i].filename,"%ROOT_PATH%",cases[i].replacement));
		ASSERT(SUCCESS == get_file_content(m_text(file_path),actual_text));
		ASSERT(strcmp(m_text(actual_text),cases[i].expected_text) == 0);
	}

	/* Rejected expressions must leave the existing file unchanged */
	const char *invalid_cases[] = {
		"$(printf forbidden)",
		"`printf forbidden`"
	};
	for(size_t i = 0U; i < sizeof(invalid_cases) / sizeof(invalid_cases[0]) && SUCCESS == status; ++i)
	{
		const char *expression = invalid_cases[i];
		ASSERT(FAILURE & replace_placeholder_in_file(filename_expression,"%ROOT_PATH%",expression));
		ASSERT(SUCCESS == m_formatted_string(expected_error,
			"Failed to expand expression \"%s\" (wordexp code %d)\n",expression,WRDE_CMDSUB));
		ASSERT(strcmp(m_text(STDERR),m_text(expected_error)) == 0);
		if(SUCCESS == status)
		{
			call(m_del(STDERR));
		}
		ASSERT(SUCCESS == get_file_content(m_text(file_path),actual_text));
		ASSERT(strcmp(m_text(actual_text),m_text(expected_expanded_text)) == 0);
	}

	if(file_created == true)
	{
		const int remove_status = unlink(m_text(file_path));
		ASSERT(remove_status == 0);
	}
	if(directory_created == true)
	{
		const int remove_status = rmdir(m_text(temporary_directory));
		ASSERT(remove_status == 0);
	}

	if(environment_saved == true)
	{
		int restore_status;
		if(saved_value != NULL)
		{
			restore_status = setenv("TESTITALL_REPLACEMENT_FILE",saved_value,1);
		} else {
			restore_status = unsetenv("TESTITALL_REPLACEMENT_FILE");
		}
		ASSERT(restore_status == 0);
	}
	free(saved_value);
	call(m_del(expected_expanded_text));
	call(m_del(expected_error));
	call(m_del(actual_text));
	call(m_del(file_path));
	call(m_del(temporary_directory));
	RETURN_STATUS;
}

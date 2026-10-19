#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A volatile pointer keeps the symbol reference in the linked probe even
   when optimization is enabled. Probes are compiled and linked, never run */
#define SYMBOL(name) \
	"int main(void) { __typeof__(&" #name ") volatile address = &" #name "; return address == 0; }\n"

/* Match the POSIX declarations used by argp-compat.c */
#define POSIX_DECLARATIONS \
	"#undef _GNU_SOURCE\n#undef _POSIX_C_SOURCE\n#define _POSIX_C_SOURCE 200112L\n"

/* Each template setting has one self-contained compilation probe */
static const struct {
	const char *name;
	const char *source;
} checks[] = {
	{"HAVE_SYSEXITS_H","#include <sysexits.h>\nint main(void) { return 0; }\n"},
	{"HAVE_UNISTD_H","#include <unistd.h>\nint main(void) { return 0; }\n"},
	{"HAVE_DECL_PUTC_UNLOCKED","#include <stdio.h>\n" SYMBOL(putc_unlocked)},
	{"HAVE_DECL_FPUTS_UNLOCKED","#include <stdio.h>\n" SYMBOL(fputs_unlocked)},
	{"HAVE_DECL_FWRITE_UNLOCKED","#include <stdio.h>\n" SYMBOL(fwrite_unlocked)},
	{"HAVE_STRERROR_R",POSIX_DECLARATIONS "extern int strerror_r(void);\n" SYMBOL(strerror_r)},
	{"HAVE_DECL_STRERROR_R",POSIX_DECLARATIONS "#include <string.h>\n" SYMBOL(strerror_r)},
	{"HAVE_DECL_STRERROR",POSIX_DECLARATIONS "#include <string.h>\n" SYMBOL(strerror)},
	{"HAVE_DECL_STRERROR_S",POSIX_DECLARATIONS "#include <string.h>\n" SYMBOL(strerror_s)},
	{"HAVE_XASPRINTF","extern char *xasprintf(const char *, ...);\n" SYMBOL(xasprintf)},
	{"HAVE_MEMPCPY","#include <string.h>\n" SYMBOL(mempcpy)},
	{"HAVE_RANDOM","#include <stdlib.h>\n" SYMBOL(random)},
	{"HAVE_SLEEP","#include <unistd.h>\n" SYMBOL(sleep)},
	{"HAVE_STRCASECMP","#include <string.h>\n" SYMBOL(strcasecmp)},
	{"HAVE_STRCHRNUL","#include <string.h>\n" SYMBOL(strchrnul)},
	{"HAVE_STRNDUP","#include <string.h>\n" SYMBOL(strndup)},
	{"HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME","#include <errno.h>\n" SYMBOL(program_invocation_short_name)},
	{"HAVE_DECL_PROGRAM_INVOCATION_NAME","#include <errno.h>\n" SYMBOL(program_invocation_name)},
	{"HAVE_DECL___ARGV","#include <stdlib.h>\n" SYMBOL(__argv)},
	{"HAVE___PROGNAME","extern char *__progname;\n" SYMBOL(__progname)},
	{"HAVE_SSIZE_T","#include <sys/types.h>\nint main(void) { return sizeof(ssize_t) == 0; }\n"}
};

/**
 * @brief Compile and link a feature probe in the current build directory
 *
 * GNU declarations match the upstream feature checks unless the probe selects
 * POSIX declarations. Apple availability warnings reject functions unavailable
 * at the requested deployment target. Compiler output remains in configure.log
 *
 * @param command Target compiler command with flags and output redirection
 * @param source C source for the feature being checked
 * @return 1 for an available feature, 0 for a rejected probe, or -1 for an I/O or launch error
 */
static int compile_check(
	const char *command,
	const char *source)
{
	FILE *file = fopen("conftest.c","w");

	if(file == NULL)
	{
		perror("conftest.c");
		return -1;
	}

	int written = fprintf(file,
		"#undef _GNU_SOURCE\n#define _GNU_SOURCE 1\n"
		"#if defined(__APPLE__) && defined(__clang__)\n"
		"#pragma clang diagnostic error \"-Wunguarded-availability\"\n"
		"#pragma clang diagnostic error \"-Wunguarded-availability-new\"\n"
		"#endif\n%s",source);
	int closed = fclose(file);

	if(written < 0 || closed != 0)
	{
		perror("conftest.c");
		return -1;
	}

	int result = system(command);

	if(result == -1)
	{
		perror("compiler");
		return -1;
	}
	return result == 0;
}

/**
 * @brief Write config.h to stdout using the upstream template and compiler probes
 *
 * The first argument is the template path. The second is a complete shell command
 * containing the target compiler, its flags, and the probe input and output
 * paths. The working directory holds all probe artifacts. A baseline probe
 * detects a broken toolchain before feature failures can be mistaken for
 * unavailable functions
 *
 * @param argc Number of command-line arguments
 * @param argv Program name, template path, and complete compiler command
 * @return EXIT_SUCCESS for a complete header or EXIT_FAILURE for a configuration error
 */
int main(
	int  argc,
	char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr,"Usage: %s config.h.in 'compiler command'\n",argv[0]);
		return EXIT_FAILURE;
	}

	int status = EXIT_SUCCESS;

	if(compile_check(argv[2],"int main(void) { return 0; }\n") != 1)
	{
		fprintf(stderr,"Cannot compile and link a program; see configure.log\n");
		status = EXIT_FAILURE;
	}

	FILE *template = NULL;

	if(status == EXIT_SUCCESS)
	{
		template = fopen(argv[1],"r");

		if(template == NULL)
		{
			perror(argv[1]);
			status = EXIT_FAILURE;
		}
	}

	char line[512];

	while(status == EXIT_SUCCESS && fgets(line,sizeof(line),template) != NULL)
	{
		char name[128];

		if(sscanf(line,"#cmakedefine01 %127s",name) != 1)
		{
			fputs(line,stdout);
			continue;
		}

		size_t index = 0;

		while(index < sizeof(checks) / sizeof(checks[0]) && strcmp(name,checks[index].name) != 0)
		{
			++index;
		}

		if(index == sizeof(checks) / sizeof(checks[0]))
		{
			fprintf(stderr,"No compiler probe for %s\n",name);
			status = EXIT_FAILURE;
			break;
		}

		int available = compile_check(argv[2],checks[index].source);

		if(available < 0)
		{
			status = EXIT_FAILURE;
			break;
		}
		printf("#define %s %d\n",name,available);
	}

	if(template != NULL)
	{
		if(ferror(template) != 0)
		{
			perror(argv[1]);
			status = EXIT_FAILURE;
		}

		if(fclose(template) != 0)
		{
			perror(argv[1]);
			status = EXIT_FAILURE;
		}
	}

	if(fflush(stdout) != 0 || ferror(stdout) != 0)
	{
		perror("config.h");
		status = EXIT_FAILURE;
	}
	remove("conftest.c");
	remove("conftest.exe");
	return status;
}

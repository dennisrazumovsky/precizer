#include "precizer.h"
#include <argp.h>

/**
 *
 * @file parse_arguments.c
 * @brief Parse arguments
 *
*/

const char *argp_program_version = "precizer 1.0";

/* Program documentation. */
static char doc[] =
"\033[1mprecizer\033[m is a CLI application designed to traverse directories recursively \
and build a database of files and their checksums.\n\
\n\
The program is focused on the traversing of file systems of gigantic size \
with subsequent quick comparison. Using the program it is possible to find \
synchronization errors by comparing data with files and their checksums \
from different sources. Or it can be used to explore historical changes \
by comparing databases from the same sources over different times.\n" \
"\vBASIC EXAMPLE\n\
Assuming there are two hosts with large disks and identical contents mounted in /mnt1 and /mnt2 accordingly. The general task is to check whether the content is absolutely identical or whether there are differences.\n" \
"Run the program on the first machine with host name, for example “host1”:\n\
\n\
precizer --progress /mnt1\n\
\n\
As a result of the program running all directories starting from /mnt1 will be recursively traversed and the host1.db database will be created in the current directory. The --progress option visualizes progress and will show the amount of space and the number of files being examined.\n\
\n\
Run the program on a second machine with a host name, for example host2:\n\
\n\
precizer --progress /mnt2\n\
\n\
As a result, the host2.db database will be created in the current directory.\n\
\n\
Copy the files with the host1.db and host2.db databases to one of the machines and run the program with the appropriate parameters to compare the databases:\n\
\n\
precizer --compare host1.db host2.db\n\
\n\
Note that precizer only writes relative paths to the database. The example file /mnt1/abc/def/aaa.txt will be written to the database as \"abc/def/aaa.txt\" without /mnt1. The same thing will happen with the file /mnt2/abc/def/aaa.txt. Despite different mount points and different sources the files can be compared with each other under the same names \"abc/def/aaa.txt\" with the corresponding checksums.\n\
\n\
As a result of the program running, the following information will be displayed on the screen:\n\
* Which files are missing on host1 but present on host2 and vice versa.\n\
* For which files, present on both hosts, the checksums do NOT match.\n\
\n" \
"All other technical details could be found in README file of the project\n" \
"Glory to Ukraine!";

/* A description of the arguments we accept. */
static char args_doc[] = "PATH";

/* The options we understand. */
static struct argp_option options[] = {
	{ 0, 0, 0, 0, "Build database options:", 2},
	{"force",    'f', 0, 0, "Use this option only in case when the PATHs that were written into " \
	                        "the database as a result of the last scaning really need to be " \
	                        "renewed. Warning! If this option will be used in incorrect way, " \
	                        "information about files and their checksums against the database would " \
	                        "be lost.", 0 },
	{"update",   'u', 0, 0, "Force update of the database with new, changed and deleted files. "\
	                        "This option would greatly affects the security of the database. " \
	                        "Please use it only in exceptional cases and not add thoughtlessly " \
	                        "to a script. If use this parameter in incorrect way information against " \
	                        "the database containing file names and their checksums may be lost. " \
	                         "When this argument is specified, it is important to specify the same " \
	                         "PATH source (starting directory) that has been specified when the database " \
	                         "had been created. Otherwise, updating the data makes no sense — the " \
	                         "old data will be deleted from the database and completely " \
	                         "overwritten by new ones.", 0 },
	{"database", 'd', "FILE", 0, "Database file name. By default name of the local host will be used: ${HOST}.db", 0 },
	{ 0, 0, 0, 0, "Compare databases options:", 1},
	{"compare",  'c', 0, 0, "Compare two databases from different sourses. Two extra arguments should be " \
	                        "specified as paths to the databases files to compare. For example: \033[1m--compare database1.db database2.db\033[0m", 0 },
	{ 0, 0, 0, 0, "Visualizations options:", -1},
	{"silent",   's', 0, 0, "Don't produce any output. Option cannot be used with \033[1m--compare\033[0m", 0 },
	{"verbose",  'v', 0, 0, "Produce verbose output.", 0 },
	{"progress", 'p', 0, 0, "Show progress bar. This assume a preliminary count of files and the space they occupy " \
	                        "to predict execution time. It is strongly recommended not to specify this option " \
	                        "if the program is called from a script. This will reduce execution time " \
	                        "(sometimes significantly) and reduce screen output.", 0 },
	{ 0 }
};

/* Used to communicate with parse_opt. */
struct arguments
{
	char **paths;                 /* [path…] */
	char **filenames;             /* part of path */
	char *db_file_name;           /* ‘-d’ */
	int silent;                   /* ‘-s’ */
	int verbose;                  /* ‘-v’ */
	int update;                   /* ‘-u’ */
	int force;                    /* ‘-f’ */
	int progress;                 /* '-p' */
	int compare;                  /* '-c' */
};

/* Parse a single option. */
static error_t parse_opt
(
	int key,
	char *arg,
	struct argp_state *state
)
{
	/* Get the input argument from argp_parse, which we
	know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key)
	{
		case 'd':
			arguments->db_file_name = (char *)calloc(strlen(arg) + 1,sizeof(char));
			if(arguments->db_file_name == NULL)
			{
				slog(false,"ERROR: Memory allocation did not complete successfully!\n");
				exit(ARGP_ERR_UNKNOWN);
			}
			strcpy(arguments->db_file_name,arg);
			break;
		case 'c':
			arguments->compare = true;
			break;
		case 'p':
			arguments->progress = true;
			break;
		case 'u':
			arguments->update = true;
			break;
		case 'f':
			arguments->force = true;
			break;
		case 'q': case 's':
			arguments->silent = true;
			break;
		case 'v':
			arguments->verbose = true;
			break;
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;
		case ARGP_KEY_ARG:
			arguments->paths = &state->argv[state->next - 1];
			arguments->filenames = arguments->paths;
			state->next = state->argc;
			break;
		case ARGP_KEY_END:
			if(arguments->compare == true)
			{
				if(state->arg_num < 2)
				{
					argp_failure(state, 1, 0, "Too few arguments\n--compare require two arguments with paths to database files. See --help for more information");
					exit(ARGP_ERR_UNKNOWN);
				} else if (state->arg_num > 2)
				{
					argp_failure(state, 1, 0, "Too many arguments\n--compare require just two arguments with paths to database files. See --help for more information");
					exit(ARGP_ERR_UNKNOWN);
				}
			} else {
				if(state->arg_num > 1)
				{
					argp_failure(state, 1, 0, "Too many arguments\nOnly one PATH argument can be used for traversing file hierarchy. See --help for more information");
					exit(ARGP_ERR_UNKNOWN);
				}
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

Return parse_arguments
(
	const int argc,
	char * argv[]
){
	/// The status that will be passed to return() before exiting.
	/// By default, the function worked without errors.
	Return status = SUCCESS;

	struct arguments arguments;

	/* Default values to 0. */
	memset(&arguments,0,sizeof(struct arguments));

	/* Parse our arguments; every option seen by parse_opt will be
	reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if(arguments.verbose == true
		&& arguments.silent == false)
	{
		printf("Configuration: ");
		printf("paths=");
		for (int j = 0; arguments.paths[j]; j++)
		{
			printf(j == 0 ? "%s" : ", %s", arguments.paths[j]);
		}
		printf("; ");
		printf("verbose=%s; silent=%s; force=%s; update=%s; progress=%s; compare=%s",
		arguments.verbose ? "yes" : "no",
		arguments.silent ? "yes" : "no",
		arguments.force ? "yes" : "no",
		arguments.update ? "yes" : "no",
		arguments.progress ? "yes" : "no",
		arguments.compare ? "yes" : "no");
		printf("\n");
	}

	config->paths = arguments.paths;

	if (arguments.compare == true)
	{
		// The array with database names
		config->databases_to_compare = arguments.paths;
		for (int j = 0; arguments.paths[j]; j++)
		{
			// Extract file name from a path
			arguments.filenames[j] = basename(config->databases_to_compare[j]);
		}

	} else {
		for (int j = 0; arguments.paths[j]; j++)
		{
			// Remove unnecessary trailing slash at the end of the directory path
			remove_trailing_slash(arguments.paths[j]);
		}
	}

	config->filenames = arguments.filenames;
	config->progress = arguments.progress;
	config->force = arguments.force;
	config->verbose = arguments.verbose;
	config->compare = arguments.compare;
	config->update = arguments.update;
	config->silent = arguments.silent;
	if(arguments.db_file_name != NULL){
		config->db_file_name = arguments.db_file_name;
	}

	return(status);
}
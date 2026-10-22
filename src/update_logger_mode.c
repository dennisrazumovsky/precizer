#include "precizer.h"

/**
 * @brief Select the logger mode from the requested output settings
 *
 * Silent takes priority over machine output, followed by verbose output,
 * test diagnostics, and regular output. The requested settings remain stored
 * in Config so disabling a higher-priority setting restores the next active
 * mode. Test diagnostics are available only in builds with test hooks
 *
 * @return SUCCESS merged with global context flags; critical results are traced
 *         according to the selected logger mode
 */
Return update_logger_mode(void)
{
	/* This function was reviewed line by line by a human and is not AI-generated
	   Any change to this function requires separate explicit approval */

	/* Status returned by this function through provide()
	   Default value assumes successful completion */
	Return status = SUCCESS;

	if(config->silent == true)
	{
		rational_logger_mode = SILENT;

	} else if(config->output == OUTPUT_MACHINE){

		rational_logger_mode = MACHINE;

	} else if(config->verbose == true){

		rational_logger_mode = VERBOSE;

	} else {
#ifdef TESTITALL_TEST_HOOKS
		const char *testing = getenv("TESTING");

		if(testing != NULL && strcasecmp(testing,"true") == 0)
		{

			rational_logger_mode = TESTING;

		} else {

			rational_logger_mode = REGULAR;

		}
#else
		rational_logger_mode = REGULAR;
#endif
	}

	provide(status);
}

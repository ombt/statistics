//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
/* debugging functions */

/* header files */
#include "hdr/Logging.h"

namespace ombt {

/* globals */
static FILE *dbgfd = NULL;
static char dbgfilename[BUFSIZ];

/* open debug file */
void 
dbgopen(FILE *logfd)
{
	/* given an exiting file descriptor */
	if (logfd == NULL) return;

	/* open file for write */
	if (dbgfd != NULL && dbgfd != stdout && 
            dbgfd != stderr && dbgfd != logfd)
	{
		// close previous log file.
		fclose(dbgfd);
		dbgfd = NULL;
	}
	dbgfd = logfd;

	/* all done */
	return;
}

void 
dbgopen(const char *dbgfnm)
{
	/* if a bad file name, just return and do nothing */
	if (dbgfnm == NULL || *dbgfnm == '\0')
		return;

	/* check if debug file exists */
	if (access(dbgfnm, F_OK) != 0)
	{
		/* debug file is not there, don't trace */
		dbgfd = NULL;
		return;
	}

	/* add pid to debug file name and save */
	sprintf(dbgfilename, "%s.%d", dbgfnm, getpid());

	/* open file for write */
	if (dbgfd != NULL)
	{
		// close previous log file.
		fclose(dbgfd);
		dbgfd = NULL;
	}
	if (access(dbgfilename, F_OK) == 0)
	{
		/* file exists, append to it */
		if ((dbgfd = fopen(dbgfilename, "a")) == NULL)
			return;
	}
	else
	{
		/* create a new file */
		if ((dbgfd = fopen(dbgfilename, "w")) == NULL)
			return;
	}

	/* set permissions */
	(void)fchmod(fileno(dbgfd), 0777);

	/* turn off all buffering */
	setbuf(dbgfd, NULL);

	/* all done */
	return;
}

/* write to debug file */
void 
dbgprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (dbgfd != NULL)
		vfprintf(dbgfd, fmt, ap);
	va_end(ap);
	return;
}

/* close debug file */
void 
dbgclose()
{
	if (dbgfd != NULL)
		fclose(dbgfd);
	dbgfd = NULL;
	return;
}

}

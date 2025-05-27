#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h> /* Why do I need this? What are AT_* constants in the man page? */
#include <linux/limits.h> /* where NAME_MAX and PATH_MAX are defined, not from book */

void fsize(char *);
void dirwalk(char *, void (*)(char *));

int main(int argc, char **argv)
{
	if(argc == 1) /* default: current directory */
		fsize(".");
	else
		while(--argc > 0)
			fsize(*++argv);
	return 0;
}

/* fsize: print size of file "name" */
void fsize(char *name)
{
	struct stat stbuf;

	if(stat(name, &stbuf) == -1)
	{
		fprintf(stderr, "fsize: can't access %s\n", name);
		return;
	}
	if((stbuf.st_mode & S_IFMT) == S_IFDIR)
		dirwalk(name, fsize);
	printf("%8ld %s\n", (long)stbuf.st_size, name);
}

/* dirwalk: apply fcn to all files in dir */
void dirwalk(char *dir, void (*fcn)(char *))
{
	char name[PATH_MAX];
	struct dirent *dp;
	DIR *dfd;

	if((dfd = opendir(dir)) == NULL)
	{
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return;
	}
	while((dp = readdir(dfd)) != NULL)
	{
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;
		if(strlen(dir)+strlen(dp->d_name)+2 > sizeof(name))
			fprintf(stderr, "dirwalk: name %s/%s too long\n", dir, dp->d_name);
		else
		{
			sprintf(name, "%s/%s", dir, dp->d_name);
			(*fcn)(name);
		}
	}
	closedir(dfd);
}

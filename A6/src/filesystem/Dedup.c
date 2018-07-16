#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>

#include "../../include/filesystem/DedupLayer.h"

DedupLayer dedupLayer;

static void ProcessPath(const char * origPath, String * processedPath);

/**@brief Initializes the file system.
 */
static void * dedupInit(struct fuse_conn_info * conn,
                        struct fuse_config * cfg)
{
	// enables logging via printf() into the log file
	struct fuse_context * ctx = fuse_get_context();
	int fd = *(int *) ctx->private_data;
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	close(fd);
	setbuf(stdout, NULL);

	printf("dedupInit start\n");

	cfg->use_ino = 1;
	cfg->entry_timeout = 0;
	cfg->attr_timeout = 0;
	cfg->negative_timeout = 0;

	DedupLayer_Init(&dedupLayer);

	printf("dedupInit finished\n");

	return NULL;
}

/**@brief Used to retrieve file attributes.
 */
static int dedupGetAttr(const char * path, struct stat * stbuf,
                        struct fuse_file_info * fi)
{
	printf("dedupGetAttr path=%s\n", path);

	int rc;

	if (fi != NULL)
		rc = fstat(fi->fh, stbuf);
	else
		rc = lstat(path, stbuf);

	return (rc != 0) ? -errno : 0;
}

/**@brief Used to retrieve directory entries.
 */
static int dedupReadDir(const char * path, void * buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info * fi,
                        enum fuse_readdir_flags flags)
{
	DIR * dir;
	struct dirent * entry;

	if ((dir = opendir(path)) == NULL)
		return -errno;

	seekdir(dir, offset);
	while ((entry = readdir(dir)) != NULL)
	{
		struct stat st = {
			.st_ino = entry->d_ino,
			.st_mode = entry->d_type << 12
		};

		if (filler(buf, entry->d_name, &st, 0, 0))
			break;
	}

	String processedPath;
	ProcessPath(path, &processedPath);

	Vector * entries = DedupLayer_GetDirectoryEntries(&dedupLayer, &processedPath);

	String_Destroy(&processedPath);

	if (entries)
	{
		String * dirEntry;
		Vector_ForeachBegin(entries, dirEntry, i)
			if (filler(buf, String_CharArray(dirEntry), NULL, 0, 0))
				break;
		Vector_ForeachEnd
	}

	closedir(dir);
	return 0;
}

/**@brief Used to create directories.
 */
static int dedupMkdir(const char * path, mode_t mode)
{
	printf("dedupMkdir path=%s\n", path);

	return (mkdir(path, mode) != 0) ? -errno : 0;
}

/**@brief Used to create files.
 */
static int dedupCreate(const char * path, mode_t mode,
                       struct fuse_file_info * fi)
{
	printf("dedupCreate path=%s\n", path);

	int fd;

	fd = open(path, fi->flags, mode);
	if (fd < 0)
		return -errno;

	fi->fh = fd;
	return 0;
}

/**@brief Used to open files.
 */
static int dedupOpen(const char * path, struct fuse_file_info * fi)
{
	printf("dedupOpen path=%s\n", path);

	String processedPath;
	ProcessPath(path, &processedPath);

	String * truePath = DedupLayer_GetTrueFilePath(&dedupLayer, &processedPath);
	printf("dedupOpen truePath=%s\n", String_CharArray(truePath));
	int fd = open(String_CharArray(truePath), fi->flags);

	String_Destroy(&processedPath);

	if (fd < 0)
		return -errno;

	fi->fh = fd;

	return 0;
}

/**@brief Used to read file contents.
 */
static int dedupRead(const char * path, char * buf, size_t size, off_t offset,
                     struct fuse_file_info * fi)
{
	ssize_t result = pread(fi->fh, buf, size, offset);
	if (result < 0)
		return -errno;
	return (int) result;
}

/**@brief Used to write file contents.
 */
static int dedupWrite(const char * path, const char * buf, size_t size,
                      off_t offset, struct fuse_file_info * fi)
{
	printf("dedupWrite path=%s\n", path);

	String processedPath;
	ProcessPath(path, &processedPath);

	bool result = DedupLayer_Write(&dedupLayer, &processedPath, (int) fi->fh, buf, size, offset);

	String_Destroy(&processedPath);

	if (!result)
		return -errno;

	return (int) size;
}

static int dedupUnlink(const char * path)
{
	printf("dedupUnlink path=%s\n", path);

	String processedPath;
	ProcessPath(path, &processedPath);

	bool result = DedupLayer_Unlink(&dedupLayer, &processedPath);

	String_Destroy(&processedPath);

	if (!result)
		return -errno;

	return 0;
}

static int dedupRmdir(const char * path)
{
	printf("dedupRmdir path=%s\n", path);

	String processedPath;
	ProcessPath(path, &processedPath);

	bool result = DedupLayer_RemoveDirectory(&dedupLayer, &processedPath);

	String_Destroy(&processedPath);

	if (!result)
		return -errno;

	return 0;
}

/**@brief Maps the callback functions to FUSE operations.
 */
static const struct fuse_operations dedupOper = {
	.init           = dedupInit,
	.getattr        = dedupGetAttr,
	.readdir        = dedupReadDir,
	.mkdir          = dedupMkdir,
	.create         = dedupCreate,
	.open           = dedupOpen,
	.read           = dedupRead,
	.write          = dedupWrite,
	.unlink         = dedupUnlink,
	.rmdir          = dedupRmdir,
};

int main(int argc, char * argv[])
{
	int fd;

	// create a log file for debugging purposes
	if ((fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
	{
		perror("Logfile");
		return -1;
	}

	return fuse_main(argc, argv, &dedupOper, &fd);
}

static void ProcessPath(const char * origPath, String * processedPath)
{
	String_InitFromCharArray(processedPath, origPath);
}

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include "../../include/filesystem/DedupLayer.h"

// TODO: Implement directory to file mapping.

static void StringItemDestroyer(VectorItem item);

static bool StringItemEquals(VectorItem lhs, VectorItem rhs);

// No deep copy!
static FileHash * FindFileHash(DedupLayer * self, String * path);

static void RemoveFileHash(DedupLayer * self, String * path);

static FileHash * InsertFileHash(DedupLayer * self, String * path);

// No deep copy!
static Vector * FindFileReferences(DedupLayer * self, FileHash fileHash);

static void RemoveFileReferences(DedupLayer * self, FileHash fileHash);

static Vector * InsertFileReferences(DedupLayer * self, FileHash fileHash);

static String * GetTrueFilePath(Vector * fileReferences);

static bool ComputeFileHash(FileDescriptor fd, FileHash * fileHash);

static bool PrecomputeFileHash(FileDescriptor fd, const char * buffer, size_t size, off_t off, FileHash * fileHash);

static void CreateNewFileReference(DedupLayer * self, String * filePath, FileHash fileHash);

static bool UpdateFileReference(DedupLayer * self,
                                String * origFilePath,
                                FileHash oldFileHash,
                                FileHash newFileHash,
                                FileDescriptor fd,
                                const char * buffer,
                                size_t size,
                                off_t offset);

static bool CopyFile(String * srcPath, String * destPath);

static bool RedirectFileDescriptor(FileDescriptor fd, String * newPath);

void DedupLayer_Init(DedupLayer * self)
{
	dictInit(&self->dir2entries);
	dictInit(&self->filePath2fileHash);
	dictInit(&self->fileHash2fileReferences);
}

Vector * DedupLayer_GetDirectoryEntries(DedupLayer * self, String * dirPath)
{
	return (Vector *) dictFind(&self->dir2entries, String_CharArray(dirPath));
}

String * DedupLayer_GetTrueFilePath(DedupLayer * self, String * origFilePath)
{
	FileHash * fileHash = FindFileHash(self, origFilePath);
	if (!fileHash)
		return origFilePath;

	Vector * fileReferences = FindFileReferences(self, *fileHash);
	assert(fileReferences != NULL && !Vector_IsEmpty(fileReferences));
	return GetTrueFilePath(fileReferences);
}

bool DedupLayer_Write(DedupLayer * self,
                      String * filePath,
                      FileDescriptor fd,
                      const char * buffer,
                      size_t size,
                      off_t offset)
{
	FileHash * fileHash = FindFileHash(self, filePath);
	if (!fileHash)
	{
		if (!ComputeFileHash(fd, fileHash))
			return false;

		CreateNewFileReference(self, filePath, *fileHash);
		return true;
	}

	FileHash newFileHash;
	if (!PrecomputeFileHash(fd, buffer, size, offset, &newFileHash))
		return false;

	if (*fileHash == newFileHash)
		return true;

	// Encountered different file hashes so the file has changed.
	if (!UpdateFileReference(self, filePath, *fileHash, newFileHash, fd, buffer, size, offset))
		return false;
}

bool DedupLayer_Unlink(DedupLayer * self, String * filePath)
{
	// TODO: Implementation
}

bool DedupLayer_RemoveDirectory(DedupLayer * self, String * dirPath)
{
	// TODO: Implementation
}

static void StringItemDestroyer(VectorItem item)
{
	String_Destroy((String *) item);
}

static bool StringItemEquals(VectorItem lhs, VectorItem rhs)
{
	return String_Equals((String *) lhs, (String *) rhs);
}

static FileHash * FindFileHash(DedupLayer * self, String * path)
{
	// TODO: Implementation
}

static void RemoveFileHash(DedupLayer * self, String * path)
{
	// TODO: Implementation
}

static FileHash * InsertFileHash(DedupLayer * self, String * path)
{
	// TODO: Implementation
}

static Vector * FindFileReferences(DedupLayer * self, FileHash fileHash)
{
	// TODO: Implementation
}

static void RemoveFileReferences(DedupLayer * self, FileHash fileHash)
{
	// TODO: Implementation
}

static Vector * InsertFileReferences(DedupLayer * self, FileHash fileHash)
{
	// TODO: Implementation
}

static String * GetTrueFilePath(Vector * fileReferences)
{
	// TODO: Implementation
}

static bool ComputeFileHash(FileDescriptor fd, FileHash * fileHash)
{
	// TODO: Implementation
}

static bool PrecomputeFileHash(FileDescriptor fd, const char * buffer, size_t size, off_t off, FileHash * fileHash)
{
	// TODO: Implementation
}

static void CreateNewFileReference(DedupLayer * self, String * filePath, FileHash fileHash)
{
	// TODO: Implementation
}

static bool UpdateFileReference(DedupLayer * self,
                                String * origFilePath,
                                FileHash oldFileHash,
                                FileHash newFileHash,
                                FileDescriptor fd,
                                const char * buffer,
                                size_t size,
                                off_t offset)
{
	Vector * fileReferences = FindFileReferences(self, oldFileHash);
	assert(fileReferences != NULL);

	VectorIterator index;
	bool found = Vector_Find(fileReferences, origFilePath, StringItemEquals, &index);
	assert(found);

	//
	// Remove origFilePath's current reference.
	//
	Vector_RemoveAndDestroy(fileReferences, index);
	RemoveFileHash(self, origFilePath);

	if (Vector_IsEmpty(fileReferences))
	{
		RemoveFileReferences(self, oldFileHash);
	}
	else
	{
		// The referenced/true path is at index 0.
		// If origFilePath was the referenced path, we need to create a copy so the file can still be referenced.
		if (index == 0)
		{
			String * newTruePath = GetTrueFilePath(fileReferences);
			if (!CopyFile(origFilePath, newTruePath))
				return false;
		}
	}

	//
	// Let origFilePath reference its new content.
	//
	InsertFileHash(self, origFilePath);

	String origFilePathCopy;
	String_Copy(origFilePath, &origFilePathCopy);

	// There already exists a file with the new hash?
	fileReferences = FindFileReferences(self, newFileHash);
	if (fileReferences)
	{
		Vector_Append(fileReferences, &origFilePathCopy);

		// Since we would now have two copies of the same file,
		// we redirect the file descriptor fd to the referenced file and remove the
		// one which is pointed to by origFilePath.
		String * truePath = GetTrueFilePath(fileReferences);
		RedirectFileDescriptor(fd, truePath);
		if (unlink(String_CharArray(origFilePath)) != 0)
			return false;
	}
	else
	{
		// Create a new reference list.
		fileReferences = InsertFileReferences(self, newFileHash);
		Vector_Append(fileReferences, &origFilePathCopy);

		// Finally write the data to the file.
		if (pwrite(fd, buffer, size, offset) < 0)
			return false;
	}

	return true;
}

static bool RedirectFileDescriptor(FileDescriptor fd, String * newPath)
{
	int result = fcntl(fd, F_GETFL);
	mode_t mode = (mode_t) (result & O_ACCMODE);
	int flags = result & (~(int) O_ACCMODE);

	int newFd = open(String_CharArray(newPath), mode, flags);
	if (newFd < 0)
		return false;

	if (dup2(newFd, fd) < 0)
	{
		close(newFd);
		return false;
	}

	return true;
}

static bool CopyFile(String * srcPath, String * destPath)
{
	// TODO: Implementation
}
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include "../../include/filesystem/DedupLayer.h"

static void StringItemDestroyer(VectorItem item);

static bool StringItemEquals(VectorItem lhs, VectorItem rhs);

static FileHash * FindFileHash(DedupLayer * self, String * filePath);

static void RemoveFileHash(DedupLayer * self, String * filePath);

static FileHash * InsertFileHash(DedupLayer * self, String * filePath);

static Vector * FindFileReferences(DedupLayer * self, FileHash fileHash);

static void RemoveFileReferences(DedupLayer * self, FileHash fileHash);

static Vector * InsertFileReferences(DedupLayer * self, FileHash fileHash);

static bool DeleteFileReference(DedupLayer * self, String * origFilePath, FileHash fileHash, bool keepFile);

static bool AddFileReference(DedupLayer * self,
                             String * origFilePath,
                             FileHash newFileHash,
                             FileDescriptor fd);

static bool UpdateFileReference(DedupLayer * self,
                                String * origFilePath,
                                FileHash oldFileHash,
                                FileHash newFileHash,
                                FileDescriptor fd);

static Vector * FindDirEntries(DedupLayer * self, String * dirPath);

static void RemoveDirEntries(DedupLayer * self, String * dirPath);

static Vector * InsertDirEntries(DedupLayer * self, String * dirPath);

static void AddDirEntry(DedupLayer * self, String * filePath);

static void DeleteDirEntry(DedupLayer * self, String * filePath);

static String * GetTrueFilePath(Vector * fileReferences);

static bool ComputeFileHash(FileDescriptor fd, FileHash * fileHash);

static bool PrecomputeFileHash(FileDescriptor fd, const char * buffer, size_t size, off_t off, FileHash * fileHash);

static void CreateNewFileReference(DedupLayer * self, String * filePath, FileHash fileHash);

static bool CopyFile(String * srcPath, String * destPath);

static bool RedirectFileDescriptor(FileDescriptor fd, String * newPath);

static void MakePath(String * dirPath, struct dirent * entry, String * fullPath);

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

	bool fileAlreadyExists = FindFileReferences(self, newFileHash) != NULL;

	// Encountered different file hashes so the file has changed.
	if (!UpdateFileReference(self, filePath, *fileHash, newFileHash, fd))
		return false;

	if (!fileAlreadyExists)
		return pwrite(fd, buffer, size, offset) >= 0;

	return true;
}

bool DedupLayer_Unlink(DedupLayer * self, String * filePath)
{
	FileHash * fileHash = FindFileHash(self, filePath);
	if (fileHash)
	{
		// If the file is registered, we remove its file reference.
		if (!DeleteFileReference(self, filePath, *fileHash, false))
			return false;
	}
	else
	{
		// Otherwise we unlink the file directly.
		if (unlink(String_CharArray(filePath)) != 0)
			return false;
	}

	return true;
}

bool DedupLayer_RemoveDirectory(DedupLayer * self, String * dirPath)
{
	DIR * dir;
	struct dirent * entry;

	if ((dir = opendir(String_CharArray(dirPath))) == NULL)
		return false;

	while ((entry = readdir(dir)) != NULL)
	{
		String path;
		MakePath(dirPath, entry, &path);
		if (entry->d_type == DT_DIR)
		{
			if (!DedupLayer_RemoveDirectory(self, &path))
				return false;
		}
		else
		{
			if (!DedupLayer_Unlink(self, &path))
				return false;
		}
	}

	closedir(dir);
	return true;
}

static void StringItemDestroyer(VectorItem item)
{
	String_Destroy((String *) item);
}

static bool StringItemEquals(VectorItem lhs, VectorItem rhs)
{
	return String_Equals((String *) lhs, (String *) rhs);
}

static FileHash * FindFileHash(DedupLayer * self, String * filePath)
{
	// TODO: Implementation
}

static void RemoveFileHash(DedupLayer * self, String * filePath)
{
	// TODO: Implementation
}

static FileHash * InsertFileHash(DedupLayer * self, String * filePath)
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

static bool DeleteFileReference(DedupLayer * self, String * origFilePath, FileHash fileHash, bool keepFile)
{
	Vector * fileReferences = FindFileReferences(self, fileHash);
	if (!fileReferences)
		return true;

	VectorIterator index;
	bool found = Vector_Find(fileReferences, origFilePath, StringItemEquals, &index);
	assert(found);

	Vector_RemoveAndDestroy(fileReferences, index);
	RemoveFileHash(self, origFilePath);

	if (Vector_IsEmpty(fileReferences))
	{
		RemoveFileReferences(self, fileHash);

		if (!keepFile)
		{
			if (unlink(String_CharArray(origFilePath)) != 0)
				return false;
		}
	}
	else
	{
		String * truePath = GetTrueFilePath(fileReferences);
		if (index == 0)
		{
			// The referenced/true path is at index 0.
			// If origFilePath was the referenced path, we need to create a copy so the file can still be referenced.
			if (!CopyFile(origFilePath, truePath))
				return false;
		}
		else
		{
			// origFilePath was not the referenced/true path and so it must have been remembered as a directory entry.
			DeleteDirEntry(self, origFilePath);

			if (keepFile)
			{
				if (!CopyFile(truePath, origFilePath))
					return false;
			}
		}
	}
}

static bool AddFileReference(DedupLayer * self,
                             String * origFilePath,
                             FileHash newFileHash,
                             FileDescriptor fd)
{
	InsertFileHash(self, origFilePath);

	String origFilePathCopy;
	String_Copy(origFilePath, &origFilePathCopy);

	// There already exists a file with the new hash?
	Vector * fileReferences = FindFileReferences(self, newFileHash);
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
		// Now there's no more existing file at the origFilePath for the underlying file system.
		// However, if someone wants to list the files in origFilePath's directory, it should still be displayed.
		// So we need keep track of the origFilePath as an entry in its directory.
		AddDirEntry(self, origFilePath);
	}
	else
	{
		// Create a new reference list.
		fileReferences = InsertFileReferences(self, newFileHash);
		Vector_Append(fileReferences, &origFilePathCopy);
	}
}

static bool UpdateFileReference(DedupLayer * self,
                                String * origFilePath,
                                FileHash oldFileHash,
                                FileHash newFileHash,
                                FileDescriptor fd)
{
	if (!DeleteFileReference(self, origFilePath, oldFileHash, true))
		return false;

	if (!AddFileReference(self, origFilePath, newFileHash, fd))
		return false;

	return true;
}

static Vector * FindDirEntries(DedupLayer * self, String * dirPath)
{
	// TODO: Implementation
}

static void RemoveDirEntries(DedupLayer * self, String * dirPath)
{
	// TODO: Implementation
}

static Vector * InsertDirEntries(DedupLayer * self, String * dirPath)
{
	// TODO: Implementation
}

static void AddDirEntry(DedupLayer * self, String * filePath)
{
	// TODO: Implementation
}

static void DeleteDirEntry(DedupLayer * self, String * filePath)
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

static void MakePath(String * dirPath, struct dirent * entry, String * fullPath)
{
	// TODO: Implementation
}
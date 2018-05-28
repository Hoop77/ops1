//
// Created by philipp on 26.05.18.
//

#include <string.h>
#include "../../include/common/Path.h"
#include "../../include/common/Utils.h"

static void StringItemDestroyer(VectorItem item)
{
    String_Destroy(item);
}

void Path_Init(Path * self)
{
    Vector_Init(self, sizeof(String), StringItemDestroyer);
}

bool Path_InitFromCharArray(Path * self, const char * path)
{
    Path_Init(self);

    String strPath;
    String_InitFromCharArray(&strPath, path);
    if (String_IsEmpty(&strPath) || String_CharAt(&strPath, 0) != '/')
    {
        String_Destroy(&strPath);
        return false;
    }

    Vector split;
    String_Split(&strPath, '/', &split);
    // strPath doesn't contain any '/' chars
    if (Vector_Size(&split) == 1)
    {
        String_Destroy(&strPath);
        Vector_Destroy(&split);
        return false;
    }

    // split[0] is empty because strPath starts with '/'
    for (size_t i = 1; i < Vector_Size(&split); ++i)
    {
        String * dirName = Vector_At(&split, i);

        if (String_IsEmpty(dirName) && i != Vector_Size(&split) - 1)
        {
            String_Destroy(&strPath);
            Vector_Destroy(&split);
            return false;
        }
        else if (String_IsEmpty(dirName) && i == Vector_Size(&split) - 1)
        {
            // Permit '/' at the path end without a following directory name.
            break;
        }

        String dirNameCopy;
        String_Copy(dirName, &dirNameCopy);
        Vector_Append(self, &dirNameCopy);
    }

    String_Destroy(&strPath);
    Vector_Destroy(&split);
    return true;
}

void Path_Destroy(Path * self)
{
    Vector_Destroy(self);
}

size_t Path_Depth(Path * self)
{
    return Vector_Size(self);
}

void Path_CommonPrefix(Path * self, Path * other, Path * commonPrefix)
{
    Path_Init(commonPrefix);
    for (size_t i = 0; i < min(Path_Depth(self), Path_Depth(other)); ++i)
    {
        String * selfDirName = Vector_At(self, i);
        String * otherDirName = Vector_At(other, i);
        if (!String_Equals(selfDirName, otherDirName))
            break;
        String dirNameCopy;
        String_Copy(selfDirName, &dirNameCopy);
        Vector_Append(commonPrefix, &dirNameCopy);
    }
}

void Path_RelativePath(Path * self, Path * other, String * relativePath)
{
    String_Init(relativePath);

    Path commonPrefix;
    Path_CommonPrefix(self, other, &commonPrefix);

    size_t selfDepth = Path_Depth(self);
    size_t otherDepth = Path_Depth(other);
    size_t commonDepth = Path_Depth(&commonPrefix);

    // other is located inside self since self is a prefix of other
    if (commonDepth >= selfDepth)
    {
        String_AppendCharArray(relativePath, "./");
    }
    else // other is not located inside self since self isn't a prefix of other
    {
        size_t descent = selfDepth - commonDepth;
        for (size_t i = 0; i < descent; ++i)
            String_AppendCharArray(relativePath, "../");
    }

    for (size_t i = commonDepth; i < otherDepth; ++i)
    {
        String_Append(relativePath, Vector_At(other, i));
        String_AppendChar(relativePath, '/');
    }

    Path_Destroy(&commonPrefix);
}
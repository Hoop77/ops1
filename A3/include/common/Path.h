//
// Created by philipp on 26.05.18.
//

#ifndef REMOTESHELL_PATH_H
#define REMOTESHELL_PATH_H

#include "Vector.h"
#include "String.h"

// Vector of strings where each string represents a directory name.
// An empty vector represents the root path ('/').
typedef Vector Path;

bool Path_FromString(Path * self, String * strPath);

void Path_Root(Path * self);

void Path_Destroy(Path * self);

size_t Path_Depth(Path * self);

void Path_CommonPrefix(Path * self, Path * other, Path * commonPrefix);

void Path_RelativePath(Path * self, Path * other, String * relativePath);

#endif //REMOTESHELL_PATH_H

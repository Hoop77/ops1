//
// Created by philipp on 25.05.18.
//

#include <stdbool.h>

#ifndef REMOTE_SHELL_FILETRANSFER_H
#define REMOTE_SHELL_FILETRANSFER_H

bool FileUpload(const char * filename, int fd);

bool FileDownload(const char * filename, int fd);

#endif //REMOTE_SHELL_FILETRANSFER_H

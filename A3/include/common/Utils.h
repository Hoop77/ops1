//
// Created by philipp on 25.05.18.
//

#ifndef REMOTESHELL_UTILS_H
#define REMOTESHELL_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define terminate() { fprintf(stderr, "Terminated at file: %s, line: %s\n", __FILE__, __LINE__); \
    exit(EXIT_FAILURE); }

#endif //REMOTESHELL_UTILS_H

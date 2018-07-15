cmake_minimum_required(VERSION 3.9)
project(A6 C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        include/common/Vector.h
        include/common/Acceptor.h
        include/common/Socket.h
        include/common/Shell.h
        include/common/String.h
        include/common/Utils.h
        include/common/Path.h
        include/common/Program.h
        include/common/Pipe.h
        include/common/RemoteShell.h
        include/common/Reader.h
        include/common/Dict.h
        include/common/Array.h
        include/filesystem/Filesystem.h
        src/common/Vector.c
        src/common/Acceptor.c
        src/common/Socket.c
        src/common/Shell.c
        src/common/String.c
        src/common/Path.c
        src/common/Program.c
        src/common/Pipe.c
        src/common/RemoteShell.c
        src/common/Utils.c
        src/common/Reader.c
        src/common/Array.c
        src/common/Dict.c
        src/filesystem/Filesystem.c)

link_libraries(libfuse3.so)

add_executable(A6 ${SOURCE_FILES} src/filesystem/Dedup.c)

add_executable(FilesystemTest ${SOURCE_FILES} test/FilesystemTest.c)
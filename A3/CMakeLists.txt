cmake_minimum_required(VERSION 3.9)
project(RemoteShell C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        include/common/Vector.h
        include/common/Acceptor.h
        include/common/Socket.h
        include/common/FileTransfer.h
        include/common/Shell.h
        include/common/String.h
        include/common/Utils.h
        include/common/Path.h
        src/common/Vector.c
        src/common/Acceptor.c
        src/common/Socket.c
        src/common/FileTransfer.c
        src/common/Shell.c
        src/common/String.c
        src/common/Path.c)

add_executable(RemoteShell ${SOURCE_FILES} srv/server.h srv/server.c )

add_executable(Test ${SOURCE_FILES} test/TestFramework.h test/Test.c)
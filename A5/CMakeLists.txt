cmake_minimum_required(VERSION 3.5)
project(A5 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS -pthread)

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
        include/common/Array.h
        include/ult/Ult.h
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
        src/ult/Ult.c)

add_executable(Test ${SOURCE_FILES} test/TestFramework.h test/Test.c)

add_executable(UltTest ${SOURCE_FILES} test/UltTest.c)

add_executable(TestServer ${SOURCE_FILES} test/TestServer.c)

add_executable(Tcd ${SOURCE_FILES} src/tcd/tcd.c)
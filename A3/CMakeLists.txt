cmake_minimum_required(VERSION 3.9)
project(RemoteShell C)

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
        src/common/Reader.c)

add_executable(Server ${SOURCE_FILES} include/srv/server.h src/srv/server.c)

add_executable(Client ${SOURCE_FILES} src/cli/client.c include/cli/client.h)

add_executable(Test ${SOURCE_FILES} test/TestFramework.h test/Test.c)
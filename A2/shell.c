#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
// #include <s/wait.h>

#define FALSE 0
#define TRUE 1
#define BUFSIZE 128
#define DIRSIZE 128

// method declaration
int getCurDir(char* buf, int stringSize);
int promt(char* buf, int bufSize);
int readStdIn(char* buf, int stringSize);
int countSpaces(char* string);
int str2tok(char* string, char** token, int* flags);
int tok2exe(char** token, char*** exe, int* flags);
int scd(char** args);
int sexit();
int srun(char** args);
int swait();

// method definition
int main(void)
{
    char buf[BUFSIZE];
    char dir[DIRSIZE +2];
    int errCheck;
    while( TRUE )
    {
        if (promt(dir, DIRSIZE +2)) { exit(-1); }
        errCheck = readStdIn(buf, BUFSIZE);
        if (errCheck == 1) { fprintf(stderr, "Can't read from Std In!\n\r"); exit(-1); }
        if (errCheck == 2) { break; }
        if (errCheck == 3)
        {
            fprintf(stdout, "Your input causes a buffer overflow. Please enter less character!\n\r");
            continue;
        }
        char** token = (char**) malloc(countSpaces(buf) + 1);
        if ( ! token ) { printf("TOO LESS"); continue; }
        int flags[] = {0, 0}; // { &, | }
        errCheck = str2tok(buf, token, flags);
        if (errCheck == 1) { fprintf(stdout, "No input found!\n\r"); free(token); continue; }
        if (errCheck == 2) { fprintf(stdout, "Only spaces found!\n\r"); free(token); continue; }
        char*** executables = (char***) malloc(2*(flags[1])+2);
        if ( ! executables ) { printf("TOOO LESS"); free(token); continue; } 
        tok2exe(token, executables, flags);
        
        int toExec = 0;
        // PID = fork()
        if ( token[flags[1]-1][strlen(token[flags[1]-1])-1] != '&') { printf("TODO: WAIT till fork() returns\n"); }        // last Char is not a '&'

        if ( ! strcmp( executables[toExec][0], "cd" ) ) { scd(executables[toExec]); }
        if ( ! strcmp( executables[toExec][0], "wait" ) ) { swait(); }
        if ( ! strcmp( executables[toExec][0], "exit" ) ) { sexit(); }
        srun(executables[toExec]);


        printf("\n\r");
        free(executables);
        free(token);
    }
	return 0;
}

int swait() { printf("ITS WAIT\n"); };

int srun(char** args) { return 0; }
/*
int srun(char** args)
{
    system(args[0]);
    return 0;
}
*/

int sexit() {exit(0);};

int scd(char** args)
{
    if (args[1] == 0)
    { 
        fprintf(stdout, "No arguments given!\n\r");
        return 1;
    }
    if (args[1][0] == '|')
    { 
        fprintf(stdout, "No arguments given!\n\r");
        return 1;
    }
    if (chdir(args[1]))
    {
        fprintf(stdout, "Can't change to the given directory!\n\r");
        return 2;
    }
    return 0;
}

int tok2exe(char** token, char*** exe, int* flags)
{
    /* Returns:
     * 0 - Succes
     */
    int t = 0;
    int e = 0;
    int expression = FALSE;
    while(token[t] != 0){
        if (! expression) { exe[e++] = &token[t]; expression = TRUE; }
        if (*token[t] == '|') { exe[e++] = &token[t]; expression = FALSE; }
        t++;
    }
    exe[e] = 0;
    flags[0] = e;   // save e as length of exe      pointing on 0
    flags[1] = t;   // save t as length of token    pointing on 0
    return 0;
}

int str2tok(char* string, char** token, int* flags)
{
    /* Returns:
     * 0 - Success
     * 1 - No input
     * 2 - Spaces only
     */
    int s = 0;
    int t = 0;
    int word = FALSE;
    if (string[0] == '\0') { return 1; }
    while (string[s] != '\0')
    {
        if (string[s] == ' ') { string[s] = '\0'; word = FALSE; }
        else
        {
            if (string[s] == '&') { flags[0]++; }
            if (string[s] == '|') { flags[1]++; }
            if (! word) { token[t++] = &string[s]; word = TRUE; }
        }
        s++;
    }
    if (t == 0) { return 2; }
    token[t] = 0;
    return 0;
}

int countSpaces(char* string)
{
    /* Returns:
     * int - Success
     */
    int i = 0;
    int count = 0;
    int offset = 0;
    
    while (string[offset] == ' ') { offset++; }
    do
    {
        if (string[i + offset] == ' ') {
            string[i] = ' ';
            count++;
            while (string[i+1 + offset] == ' ') { offset++; }
            continue;
        }
        if (offset) { string[i] = string[i + offset]; }
    } while ( string[i++] != '\0' );
    if (string[strlen(string) -1] == ' ') { string[strlen(string) -1] = '\0'; count--; }
    return count;
}

int readStdIn(char* buf, int stringSize)
{
    /* Retruns
     * 0 - Success
     * 1 - Unable to open Stdin
     * 2 - No input
     * 3 - Input exceeds buffer
     */
    if (fgets(buf, stringSize, stdin) == NULL)
        // already '\0' terminated
    {
        // nothing to read (directly EOF found)
        return 1;
    }
    if (buf[strlen(buf)-1] != '\n')
    {
        int ch;
        int tooLong = FALSE;
        // flush rest of line
        while(((ch = getchar()) != '\n') && (ch != EOF)) { tooLong = TRUE; }
        if (tooLong) { return 3; } else { return 0; }
    }
    buf[strlen(buf)-1] = '\0';
    if (strlen(buf) == 0) { return 2; }
    return 0;
}

int promt(char* dir, int dirBufSize)
{
    /* Returns:
     * 0 - Success
     * 1 - To small buffer
     * 2 - Can't read direcetory information
     */
    if (dirBufSize < 3) { return 1; }
    int errCheck = getCurDir(dir, dirBufSize -2);
    if (errCheck == 1) { fprintf(stderr, "Can't get directory information!"); return 2; }
    dir[strlen(dir)+2] = '\0';
    dir[strlen(dir)+1] = ' ';
    dir[strlen(dir)] = '>';
    fprintf(stdout, "%s", dir);
    return 0;
}

int getCurDir(char* buf, int stringSize)
{
    /* Returns:
     * 0 - Success
     * 1 - Error on reading
     */
    if (getcwd(buf, stringSize) == NULL) { return 1; }
    int count = 0;
    int i = 1;
    while (buf[i] != '\0') {
        if (buf[i] == (char) 47 || buf[i] == (char) 92) { count++; }
        i++;
    }
    i = 1;
    int j = 1;
    if (count < 2) { return 0; }
    while (buf[i] != (char) 47 && buf[i] != (char) 92) { i++; j++; }
    count;
    j = j+5;
    while (count > 1)
    {
        if (buf[i] == (char) 47 || buf[i] == (char) 92) { count--; }
        i++;
    }
    if (j > i) { return 0; }
    buf[j-5] = '/';
    buf[j-4] = '.';
    buf[j-3] = '.';
    buf[j-2] = '.';
    buf[j-1] = '/';
    buf[j++] = buf[i];
    while (buf[i] != (char) 47 && buf[i] != (char) 92) { i++; }
    buf[j++] = '/';
    i++;
    while (buf[i] != '\0') { buf[j++] = buf[i++]; }
    buf[j] = '\0';
    return 0;
}


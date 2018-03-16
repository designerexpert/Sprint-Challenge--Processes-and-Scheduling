#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// My own Requirements for this - Not sure if this is allowed.
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdint.h>

#define PROMPT "lambda-shell$ "

#define MAX_TOKENS 100
#define COMMANDLINE_BUFSIZE 1024
#define DEBUG 1 // Set to 1 to turn on some debugging output, or 0 to turn off

/**
 * Parse the command line.
 *
 * YOU DON'T NEED TO MODIFY THIS!
 * (But you should study it to see how it works)
 *
 * Takes a string like "ls -la .." and breaks it down into an array of pointers
 * to strings like this:
 *
 *   args[0] ---> "ls"
 *   args[1] ---> "-la"
 *   args[2] ---> ".."
 *   args[3] ---> NULL (NULL is a pointer to address 0)
 *
 * @param str {char *} Pointer to the complete command line string.
 * @param args {char **} Pointer to an array of strings. This will hold the result.
 * @param args_count {int *} Pointer to an int that will hold the final args count.
 *
 * @returns A copy of args for convenience.
 */
char **parse_commandline(char *str, char **args, int *args_count)
{
    char *token;

    *args_count = 0;

    token = strtok(str, " \t\n\r");

    while (token != NULL && *args_count < MAX_TOKENS - 1)
    {
        args[(*args_count)++] = token;

        token = strtok(NULL, " \t\n\r");
    }

    args[*args_count] = NULL;

    return args;
}

/**
 * Main
 */
int main(void)
{
    // My own Variables for this
    DIR *dir;
    struct dirent *entry;
    char currentDir[4096] = "."; // Initialize Current Directory to the Root Directory

    // Holds the command line the user types in
    char commandline[COMMANDLINE_BUFSIZE];

    // Holds the parsed version of the command line
    char *args[MAX_TOKENS];

    // How many command line args the user typed
    int args_count;

    // Shell loops forever (until we tell it to exit)
    while (1)
    {
        // Print a prompt
        printf("%s", PROMPT);
        fflush(stdout); // Force the line above to print

        // Read input from keyboard
        fgets(commandline, sizeof commandline, stdin);

        // Exit the shell on End-Of-File (CRTL-D)
        if (feof(stdin))
        {
            break;
        }

        // Parse input into individual arguments
        parse_commandline(commandline, args, &args_count);

        if (args_count == 0)
        {
            // If the user entered no commands, do nothing
            continue;
        }

        // Exit the shell if args[0] is the built-in "exit" command
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }

        // Setting the current directory for later displaying it.
        if (args_count > 1) // Provided we get more than 1 argument
        {
            if (strcmp(args[1], "-l") == 0)
            {
                //printf("args[1] is -l and now executing insdie\n");
                // Do stuff for when -l is passed in
                if ((dir = opendir(currentDir)) == NULL)
                {
                    perror("Unable to open directory");
                }
                else
                {
                    while ((entry = readdir(dir)) != NULL)
                    /* Desired Output
                    lambda-shell$ ls -l
                    total 32
                    -rwxr-xr-x  1 beej  staff  9108 Mar 15 13:28 lssh
                    -rw-r--r--  1 beej  staff  2772 Mar 15 13:27 lssh.c
                    */
                    {
                        struct stat buf; // Contains each item's system stats.
                        char size[1024]; // Will contain File size or <DIR>.
                        struct passwd *pwd;          // holds owners name
                        struct group *grp;           // Holds group name
                        struct tm *tm;
                        char datestring[256];
                        
                        stat(entry->d_name, &buf);   // Extracts stats into buf.
                        if (entry->d_type == DT_DIR) // Checks to see if it is a directory.
                        {
                            //sprintf(type, "%s", "<DIR>"); // assigns <DIR> to the type char array.
                        }
                        else // In the case it is not a directory
                        {
                            sprintf(size, "%ld", buf.st_size); // assigns the size
                            if (entry->d_name[0] != '.')       // Check for hidden files and does not show them
                            {
                                //Handling Writing Modes
                                printf((buf.st_mode & S_IRUSR) ? "r" : "-");
                                printf((buf.st_mode & S_IRUSR) ? "r" : "-");
                                printf((buf.st_mode & S_IWUSR) ? "w" : "-");
                                printf((buf.st_mode & S_IXUSR) ? "x" : "-");
                                printf((buf.st_mode & S_IRGRP) ? "r" : "-");
                                printf((buf.st_mode & S_IWGRP) ? "w" : "-");
                                printf((buf.st_mode & S_IXGRP) ? "x" : "-");
                                printf((buf.st_mode & S_IROTH) ? "r" : "-");
                                printf((buf.st_mode & S_IWOTH) ? "w" : "-");
                                printf((buf.st_mode & S_IXOTH) ? "x" : "-");
                                //End Handle Writing Modes
                                //Handle Links
                                printf("%4d ", buf.st_nlink);
                                //Handle Owner
                                pwd = getpwuid(buf.st_uid);
                                printf("%s ", pwd->pw_name);
                                //Handle GroupName
                                grp = getgrgid(buf.st_gid);
                                printf("%s", grp->gr_name);
                                printf("%8s  %s\n", size, entry->d_name); // Prints out a line containing type/size and name of the item.
                            }
                        }
                    }
                    closedir(dir);
                }
            }
        }

#if DEBUG

            // Some debugging output

            // Print out the parsed command line in args[]
            // for (int i = 0; args[i] != NULL; i++)
            // {
            //     printf("%d: '%s'\n", i, args[i]);
            //     printf("ArgsCount: %d\n", args_count);
            // }

#endif
    }

    return 0;
}
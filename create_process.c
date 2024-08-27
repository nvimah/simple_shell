#include "shell.h"
/**
 * create_process - create a new process
 * @args: array of strings that contains the command and its flags
 * Return: 1 if sucess, 0 otherwise.
 */
int create_process(char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		if (execve(args[0], args, NULL) == -1)
		{
			perror("error in create_process: child process");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
	{
		perror("error in create_process: forking");
		return (0);
	}
	else
	{
		do
		{
	   		waitpid(pid, &status, WUNTRACED);
		}
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return (1);
}
/**
 * handle_path_commands - handles commands not given as full paths
 * @args: array of commands
 * Return: returns 1 on success 0 on failure
 */
int handle_path_commands(char **args)
{
        pid_t pid;
        int status;
        char *path;
        char **paths;
        char *path_command;

        path = get_path();
	if (path == NULL)
	{
		perror("Error: PATH environment variable not found");
		return (0);
	}
        paths = get_tokens(path, ":");
	if (paths == NULL)
	{
		perror("Error: Failed to tokenize PATH");
		free(path);
		return (0);
	}
        path_command = check_builtin(paths, args[0]);

        if (path_command != NULL)
        {
                pid = fork();
                if (pid == 0)
                {
                        if (execve(path_command, args, NULL) == -1)
                        {
                                perror("error in handle_path_commands: child process");
                                exit(EXIT_FAILURE);
                        }
                }
                else if (pid < 0)
                {
                        perror("error in create_process: forking");
			free(path_command);
			return (0);
		}
                else
                {
                        do
                        {
                        	waitpid(pid, &status, WUNTRACED);
                        }
                        while (!WIFEXITED(status) && !WIFSIGNALED(status));
                }
                return (1);
		free(path_command);
	}
        return (0);
}
/**
 * checker - checks whether a command is a full path
 * @args: the array of commands/ command tokens
 * Return: 1 on successfull execution or 0 if no execution
 *
 */
int checker(char **args)
{
	if (**args == '/')
	{
		if (create_process(args) == 0)
		{
			perror("Create process:");
		}
		return (1);
	}
	else
	{
		if (handle_path_commands(args) == 0)
		{
			perror("Create process: path: ");
		}
	}
	/* todo: handle env and exit nuiltins */
	return (0);
}
/**
 * check_builtin = checks for the command in the list of directories in PATH
 * @directories: array os strings of the directories
 * @command: command to be searched for
 * Return: returns a string of the full path
 */
char *check_builtin(char **directories, char *command)
{
	int i = 0;
	char *full_command;

	while (directories[i] != NULL)
	{
		full_command = path_concat(directories[i], command);
		if (full_command == NULL)
		{
			return (NULL);
		}
		if (access(full_command, F_OK | X_OK) == 0)
		{
			printf("full command:\n\n%s\n\n", full_command);
			return (full_command);
		}
		free(full_command);
		i++;
	}
	return (NULL);
}
/**
 * path_concat - appends the command entered to the directory string including /
 * @directory: the directory
 * @command: the command to be appended
 * Return: returns the full path string
 *
 */
char *path_concat(char *directory, char *command)
{
	int length;
	char *full_path;

	length = strlen(directory) + strlen(command) + 2;

	full_path = malloc(length * sizeof(char));
	if (full_path == NULL)
	{
		return (NULL);
	}
	strcpy(full_path, directory);
	strcat(full_path, "/");
	strcat(full_path, command);
	printf("full path:\n%s\n", full_path);
	return (full_path);
}

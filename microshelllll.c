#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int print_error(char *s)
{
    int i;

    i = 0;
    while (s[i])
        i++;
    write(2, s, i);
    return (1);
}

int fatal_error(char **ptr)
{
    if (ptr)
        free(ptr);
    exit(print_error("error: fatal\n"));
}

int cmd_size(char **argv, char *s)
{
    int i;

    if (!argv)
        return (0);
    i = 0;
    while (argv[i])
    {
        if (!strcmp(argv[i], s))
            return (i);
        i++;
    }
    return (i);
}

char **parse(char **argv, int *i)
{
    int j;
    int size;
    char **tmp;

    size = cmd_size(&argv[*i], ";");
    if (!size)
        return (NULL);
    tmp = (char **)malloc(sizeof(char *) * (size + 1));
    if (!tmp)
        fatal_error(NULL);
    j = -1;
    while (++j < size)
        tmp[j] = argv[j + *i];
    tmp[j] = NULL;
    *i += size;
    return (tmp);
}

int exec_cd(char **cmd)
{
    int i;

    i = 0;
    while (cmd[i])
        i++;
    if (i != 2)
		return (print_error("error: cd: bad arguments\n"));
    if (chdir(cmd[1]) < 0)
	{
		print_error("error: cd: cannot change directory to ");
		print_error(cmd[1]);
		print_error("\n");
	}
    return (0);
}

char **next_pipe(char **cmd)
{
    int i;

    if(!cmd)
        return (NULL);
    i = 0;
    while (cmd[i])
    {
        if (strcmp(cmd[i], "|"))
            return (cmd + i + 1);
        i++;
    }
    return (NULL);
}

int exec_cmd(char **cmd, char **env, char **tmp)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        fatal_error(tmp);
    if (pid == 0)
    {
        if(execve(cmd[0], cmd, env) < 0)
        {
            print_error("error: cannot execute ");
			print_error(cmd[0]);
			free(tmp);
			exit(print_error("\n"));
        }
    }
    waitpid(0, NULL, 0);
    return (0);
}

int exec_child(char **cmd, char **env, char **tmp, int in, int pipefd[2])
{
    if (dup2(in, 0) < 0)
        fatal_error(cmd);
    if (next_pipe(cmd) && dup2(pipefd[1], 1) < 0)
        fatal_error(cmd);
    // close(in);
    close(pipefd[0]);
    close(pipefd[1]);
    tmp[cmd_size(cmd, "|")] = NULL;
    exec_cmd(tmp, env, cmd);
    free(cmd);
    exit(0);
}

int execute(char **cmd, char **env)
{
    char **tmp = cmd;
    int in;
    int nb_wait;
    pid_t pid;
    int pipefd[2];

    if (!next_pipe(cmd))
        return (exec_cmd(cmd , env, tmp));
    if ((in = dup(0)) < 0)
        return (fatal_error(cmd));
    nb_wait = 0;
    while (tmp)
    {
        if (pipe(pipefd) < 0 || (pid = fork()) < 0)
            fatal_error(cmd);
        if (pid == 0)
            exec_child(cmd, env, tmp, in, pipefd);
        else
        {
            if(dup2(pipefd[0], in) < 0)
                fatal_error(cmd);
            close(pipefd[0]);
            close(pipefd[1]);
            tmp = next_pipe(tmp);
            nb_wait++;
        }
    }
    close(in);
    while(nb_wait-- >= 0)
        waitpid(0, NULL, 0);
    return (0);
}

int main(int argc, char **argv, char **env)
{
    int i;
    char **cmd;

    cmd = NULL;
    i = 1;
    while (i < argc)
    {
        cmd = parse(argv, &i);
        if (cmd && !strcmp(cmd[0], "cd"))
            exec_cd(cmd);
        else if (cmd)
            execute(cmd, env);
        if (cmd)
            free(cmd);
        cmd = NULL;
        i++;
    }
    return (0);
}



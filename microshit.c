#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_error(char *msg)
{
	int i;

	i = 0;
	while (msg[i])
		i++;
	write(2, msg, i);
	return (1);
}

int error_fatal(char **cmd)
{
	if (cmd)
		free(cmd);
	exit (print_error("error: fatal\n"));
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
	int size;
	char **cmd;
	int k;

	size = cmd_size(&argv[*i], ";");
	cmd = (char **)malloc(sizeof(char *) * (size + 1));
	if (!cmd)
		error_fatal(cmd);
	k = -1;
	while (++k < size)
		cmd[k] = argv[k + *i];
	cmd[k] = NULL;
	*i += size;
	return (cmd);
}

int cd_builtin(char **cmd)
{
	int i;

	i = 0;
	while (cmd[i])
		i++;
	if (i != 2)
		print_error("error: cd: bad arguments\n");
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

	if (!cmd)
		return (NULL);
	i = 0;
	while (cmd[i])
	{
		if (!strcmp(cmd[i], "|"))
			return (cmd + i + 1);
		i++;
	}
	return (NULL);
}

int exec_cmd(char **cmd, char **env, char **tmp)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		error_fatal(tmp);
	if (pid == 0)
	{
		if (execve(cmd[0], cmd, env) < 0)
		{
			print_error("error: cannot execute ");
			print_error(cmd[0]);
			print_error("\n");
		}
	}
	waitpid(0, NULL, 0);
	return (0);
}

int child_process(char **cmd, char **tmp, int pipefd[2], int in, char **env)
{
	int out;

	if (dup2(in, 0) < 0)
		error_fatal(cmd);
	if (!next_pipe(tmp))
        out = 1;
	else
        out = pipefd[1];
    if (pipefd[0] != 0)
		close(pipefd[0]);
	if (in != 0)
		close(in);
	if (dup2(out, 1) < 0)
		error_fatal(cmd);
    if (out != 1)
		close(out);
	tmp[cmd_size(tmp, "|")] = NULL;
	if (execve(tmp[0], tmp, env) < 0)
	{
		print_error("error: cannot execute ");
		print_error(tmp[0]);
		print_error("\n");
	}
	exit(0);
}

int exec(char **cmd, char **env)
{
	int in;
	int pipefd[2];
	pid_t pid;
	char **tmp = cmd;

	if (!next_pipe(cmd))
		return (exec_cmd(cmd, env, tmp));
	in = 0;
	while (tmp)
	{
		if (pipe(pipefd) < 0 || (pid = fork()) < 0)
			error_fatal(tmp);
		if (pid == 0)
			child_process(cmd, tmp, pipefd, in, env);
		else
		{
			if (in != 0)
				close(in);
			if(pipefd[1] != 1)
				close(pipefd[1]);
			tmp = next_pipe(tmp);//?free leaks
            in = pipefd[0];
		}
	}
	waitpid(-1, NULL, 0);
	return (0);
}

int main (int argc, char **argv, char **env)
{
	char **cmd;
	int i;

	cmd = NULL;
	i = 1;
	while (i < argc)
	{
		cmd = parse(argv, &i);
		if (cmd && !strcmp(cmd[0], "cd"))
			cd_builtin(cmd);
		else if (cmd)
			exec(cmd, env);
		if (cmd)
			free(cmd);
		cmd = NULL;
		i++;
	}
	return (0);
}

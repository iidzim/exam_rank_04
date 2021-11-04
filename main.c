#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int print_msg(char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	write(2, s, i);
	return (1);
}

int error_fatal(char **cmd)
{
	if (cmd)
		free(cmd);
	exit(print_msg("error: fata;\n"));
}

int cmd_size(char **cmd, char s)
{
	int i;

	if (!cmd)
		return (0);
	i = 0;
	while (cmd[i])
	{
		if (!strcmp(cmd[i], s))
			return (i);
		i++;
	}
	return (i);
}

char **parse(char **argv, int *i)
{
	int j;
	int size;
	char **cmd;

	size = cmd_size(argv, ";");
	cmd = (char **)malloc(sizeof(char *) * (size + 1));
	if (!cmd)
		error_fatal(cmd);
	j = -1;
	while (++j < size)
		cmd[j] = argv[j + *i];
	cmd[j] = NULL;
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
		print_msg("error: cd: bad arguments\n");
	if (chdir(cmd[1]) < 0)
	{
		print_msg("error: cd: cannot change directory to ");
		print_msg(cmd[1]);
		print_msg("\n");
	}
}

char **next_pipe(char **cmd)
{
	int i;

	if (!cmd)
		return (NULL);
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
			print_msg("error: cannot execute ");
			print_msg(cmd[0]);
			print_msg("\n");
		}
	}
	waitpid(0, NULL, 0);
	return (0);
}

int child_process(char **cmd, char **tmp, int pipefd[2], int in, char **env)
{
	if (dup2(in, 0) < 0)
		error_fatal(cmd);
	if (next_pipe(tmp))
		close(pipefd[0]);
	if (next_pipe(tmp) && dup2(pipefd[1], 1) < 0)	
		error_fatal(tmp);
	tmp[cmd_size(tmp, "|")] = NULL;
	if (execve(tmp[0], tmp, env) < 0)
	{
		print_msg("error: cannot execute ");
		print_msg(tmp[0]);
		print_msg("\n");
	}
	exit(0);
}

int exec(char **cmd, char **env)
{
	int in = 0;
	int wait_nbr = 0;
	char **tmp = cmd;
	pid_t pid;
	int pipefd[2];

	if (!next_pipe(cmd))
		return (exec_cmd(cmd, env, tmp));
	while (tmp)
	{
		if (pipe(pipefd) < 0 || (pid = fork()) < 0)
			error_fatal(tmp);
		if (pid == 0)
			child_process(cmd, tmp, pipefd, in, env);
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			tmp = next_pipe(tmp);
		}
		wait_nbr++;
	}
	while (wait_nbr-- >= 0)
		waitpid(0, NULL, 0);
	return (0);
}

int main (int argc, char **argv, char **env)
{
	int i;
	char **cmd;

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
	}
	return (0);
}

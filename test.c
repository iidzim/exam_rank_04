#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int		print_error(const char *s)
{
	int		i;

	i = 0;
	while (s[i])
		i++;
	write(STDERR_FILENO, s, i);
	return (1);
}

int		fatal_error(char **ptr)
{
	if (ptr)
		free(ptr);
	exit(print_error("error: fatal\n"));
}

int		cmd_size(char **av, const char *s)
{
	int		i;

	if (!av)
		return (0);
	i = 0;
	while (av[i])
	{
		if (!strcmp(av[i], s))
			return (i);
		i++;
	}
	return (i);
}

char	**add_cmd(char **av, int *i)
{
	int		j;
	int		size;
	char	**tmp;

	size = cmd_size(av + *i, ";");
	if (!size)
		return (NULL);
	if (!(tmp = (char **)malloc(sizeof(char *) * (size + 1))))
		fatal_error(NULL);
	j = 0;
	while (j < size)
	{
		tmp[j] = av[j + *i];
		j++;
	}
	tmp[j] = NULL;
	*i += size;
	return (tmp);
}

int		exec_cd(char **cmd)
{
	int		i;

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

char	**next_pipe(char **cmd)
{
	int		i;

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

int		exec_cmd(char **cmd, char **env, char **ptr)
{
	pid_t	pid;

	if ((pid = fork()) < 0)
		fatal_error(ptr);
	if (pid == 0)
	{
		if (execve(cmd[0], cmd, env) < 0)
		{
			print_error("error: cannot execute ");
			print_error(cmd[0]);
			free(ptr);
			exit(print_error("\n"));
		}
	}
	waitpid(0, NULL, 0);
	return (0);
}

int		exec_son(char **ptr, char **env, char **tmp, int in, int pipefd[2])
{
	if (dup2(in, STDIN_FILENO) < 0)
		fatal_error(ptr);
	if (next_pipe(tmp) && dup2(pipefd[1], STDOUT_FILENO) < 0)
		fatal_error(ptr);
	close(in);
	close(pipefd[0]);
	close(pipefd[1]);
	tmp[cmd_size(tmp, "|")] = NULL;
	exec_cmd(tmp, env, ptr);
	free(ptr);
	exit(0);
}

int		execute(char **cmd, char **env)
{
	int		in;
	int		pipefd[2];
	int		nb_wait;
	pid_t	pid;
	char	**tmp = cmd;

	if (!next_pipe(cmd))
		return (exec_cmd(cmd, env, tmp));
	if ((in = dup(STDIN_FILENO)) < 0)
		return (fatal_error(cmd));
	nb_wait = 0;
	while (tmp)
	{
		if (pipe(pipefd) < 0 || (pid = fork()) < 0)
			fatal_error(cmd);
		if (pid == 0)
			exec_son(cmd, env, tmp, in, pipefd);
		else
		{
			if (dup2(pipefd[0], in) < 0)
				fatal_error(cmd);
			close(pipefd[0]);
			close(pipefd[1]);
			tmp = next_pipe(tmp);
			nb_wait++;
		}
	}
	close(in);
	while (nb_wait-- >= 0)
		waitpid(0, NULL, 0);
	return (0);
}

int		main(int ac, char **av, char **env)
{
	int		i;
	char	**cmd;

	cmd = NULL;
	i = 1;
	while (i < ac)
	{
		cmd = add_cmd(av, &i);
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
#include <lucom.h>
#include <logs/deamon.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>

int log_message(char *filename, char *message) {
  FILE *logfile = NULL;
	logfile = fopen(filename, "a");
	if (!logfile) {
    fprintf(stderr, "[x] Cannot open output file\n");
    return EXIT_FAILURE;
  }
	fprintf(logfile, "%s\n", message);
	fclose(logfile);
  return EXIT_SUCCESS;
}

int create_deamon_proc(char *lockfile) {
  struct rlimit rlp;
  struct flock lock;
  int fd, lfd;
  char str[10];

	if (getppid() == 1) return EXIT_FAILURE; /* already a daemon */

  switch (fd = fork()) {
    case -1:
      fprintf(stderr, "[x] Fork call failed in logs/deamonizer.c\n");
      return EXIT_FAILURE;
    case 0:
      break;
    default: /* parent exits */
      return EXIT_SUCCESS;
  }

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */

  fd = getrlimit(RLIMIT_NOFILE, &rlp);
  if (fd == -1) {
    fprintf(stderr, "[x] getrlimit failed\n");
    return EXIT_FAILURE;
  }

	for (fd = rlp.rlim_cur; 0 <= fd; fd--) close(fd); /* close all descriptors */

  fd = open("/dev/null", O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "[x] Cannot open /dev/null\n");
    return EXIT_FAILURE;
  }

  /* handle standart I/O */
  if (fcntl(fd, F_DUPFD, 0) == -1) {
    fprintf(stderr, "[x] fcntl cannot dup fd: %d\n", fd);
    return EXIT_FAILURE;
  }

  /* handle standart I/O */
  if (fcntl(fd, F_DUPFD, 0) == -1) {
    fprintf(stderr, "[x] fcntl cannot dup fd: %d\n", fd);
    return EXIT_FAILURE;
  }

	umask(027); /* set newly created file permissions */

  lfd = open(lockfile, O_RDWR | O_CREAT, 0640);
  if (lfd < 0) {
    fprintf(stderr, "[x] Cannot open file %s\n", lockfile);
    return EXIT_FAILURE;
  }

  memset(&lock, 0, sizeof(lock));
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  lock.l_pid = 0;

  /* Place a write lock on the file. */
  if (fcntl(lfd, F_SETLKW, &lock) == -1) {
    fprintf(stderr, "[x] fcntl cannot dup fd: %d\n", fd);
    return EXIT_FAILURE;
  }

  /* first instance continues */
	snprintf(str, strlen(str), "%d\n", getpid());
	log_message(lockfile, str); /* record pid to lockfile */

  while (1) sleep(1);

  return EXIT_SUCCESS;
}

void redirect_stdout(char *filename, int *out, int *save_out) {
  *out = open(filename, O_RDWR|O_CREAT|O_APPEND, 0600);
  if (*out == -1) { fprintf(stderr, "[x] failed to open %s\n", filename); return; }

  *save_out = fcntl(fileno(stdout), F_DUPFD, 0);
  if (*save_out == -1) { fprintf(stderr, "[x] Cannot open output file\n"); return; }

  if (dup2(*out, fileno(stdout)) == -1) {
    fprintf(stderr, "[x] Cannot redirect stdout\n");
    return;
  }
}

void reset_stdout(int *out, int *save_out) {
  fflush(stdout);
  close(*out);
  if (dup2(*save_out, fileno(stdout)) == -1) {
    fprintf(stderr, "[x] Cannot redirect stdout\n");
    return;
  }
  close(*save_out);
}

int deamonize(char *lockfile) {
	if (!create_deamon_proc(lockfile))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

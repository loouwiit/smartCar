#include <errno.h>
#include <sys/types.h>

pid_t _getpid(void) {
    return 1;
}

int _kill(pid_t pid, int sig) {
    errno = ENOSYS;
    return -1;
}

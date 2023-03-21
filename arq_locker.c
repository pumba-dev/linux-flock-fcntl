#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/wait.h>

int fileOpen() {
    int fd = open("arq_lock_file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        printf("Error - File Open\n");
    }
    char *content = "DORIME AMENO AMENO LATRIME NO DORIME";
    if (write(fd, content, strlen(content)) == -1) {
        printf("Error - File Write\n");
    }
    return fd;
}

void fileClose(int fd) {
    if (close(fd) == -1) {
        printf("Error - File Close\n");
    }

}

void flockShared(int fd) {
    if (flock(fd, LOCK_SH) == -1) {
        printf("Error - Flock Shared\n");
    } else {
        printf("Sucess - Flock Shared\n");
    }
}

void flockExclusive(int fd) {
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        printf("Error - Flock Exclusive\n");
    } else {
        printf("Sucess - Flock Exclusive\n");
    }
}

void flockUnlock(int fd) {
    if (flock(fd, LOCK_UN) == -1) {
        printf("Error - Flock Unlock\n");
    } else {
        printf("Sucess - Flock Unlock\n");
    }
}

void fcntlExclusive(int fd, off_t offset, off_t len) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = offset;
    fl.l_len = len;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printf("Error - Fcntl Exclusive\n");
    } else {
        printf("Sucess - Fcntl Exclusive\n");
    }
}

void fcntlShared(int fd, off_t offset, off_t len) {
    struct flock fl;
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = offset;
    fl.l_len = len;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printf("Error - Fcntl Shared\n");
    } else {
        printf("Sucess - Fcntl Shared\n");
    }
}

void fcntlUnlock(int fd) {
    if (fcntl(fd, 0, lseek(fd, 0, SEEK_END)) == -1) {
        printf("Error - Fcntl Unlock\n");
    } else {
        printf("Sucess - Fcntl Unlock\n");
    }
}

int main(int argc, char *argv[]) {
    off_t offset = 0;
    int fd = fileOpen();
    off_t len = lseek(fd, 0, SEEK_END) / 2;
    pid_t pid = fork();
    fileClose(fd);

    if (pid == 0) {
        sleep(1);
        printf("Child Process START\n");

        int fd = fileOpen();
        flockExclusive(fd);
        // flockShared(fd);
        // fcntlExclusive(fd, offset, len);
        // fcntlShared(fd, offset, len);
        fileClose(fd);

        printf("Child Process END\n");
        exit(0);
    } else {
        int fd = fileOpen();
        printf("Father Process START\n");

        flockExclusive(fd);
        // flockShared(fd);
        // fcntlExclusive(fd, offset, len);
        // fcntlShared(fd, offset, len);
        wait(0);
        printf("Father CONTINUE\n");
        flockUnlock(fd);
        // fcntlUnlock(fd);
        fileClose(fd);

        printf("Father Process END\n");
    }

    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
    int fd, bytes_read;
    char buff;

    fd = open("sample-files/sample2.mp3", O_RDONLY, 0);

    if(fd == -1) {
        printf("File not available\n");
        exit(1);
    }

    for(int i=0;i<4;i++) {
        bytes_read = read(fd, &buff, 1);
        printf("Bytes read:%d\n",bytes_read);
        printf("%c:%d\n",buff,buff);
    }


    close(fd);

    return 0;
}

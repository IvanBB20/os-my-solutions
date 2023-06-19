
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) { 
    if(argc != 3){
        err(3,"Error not correct number of arguments");
    }

    int fd1 = open(argv[1],O_RDONLY);

    if(fd1 == -1){
        err(1,"Cannot open first file");
    }

    int fd2 = open(argv[2] ,O_RDONLY);

    if(fd2 == -1){
        err(2,"Cannot open second file");
    }

    int fd = open("output.bin",O_CREAT | O_TRUNC , O_WRONLY , S_IRUSR | S_IWUSR);

    if(fd == -1){
        err(3,"Cannot create and open output file");
    }

    uint8_t buf;
    uint16_t counter = 0;
    uint16_t sdl_buf;
    while(read(fd1, &buf,sizeof(uint8_t)) == sizeof(uint8_t)){
                for(int i = 7; i>=0;i--){
                    if(buf & (1 << i) == 1){
                            if(lseek(fd2,sizeof(uint16_t) *counter,SEEK_SET) == -1){
                                err(2,"Cannot lseek");
                            }
                            if(read(fd2,&sdl_buf,sizeof(uint16_t)) == -1){
                                err(3,"Cannot read");
                            }
                            if(write(fd,&sdl_buf,sizeof(uint16_t)) == -1){
                                err(3,"Cannot write");
                            }
                    }
                }
                counter++;
    }

    close(fd1);
    close(fd2);
    close(fd);
    return 0; 
}

#include <err.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


struct data{
	uint16_t offset;
	uint8_t originalByte;
	uint8_t newByte;
}__attribute__((packed));



int main(int argc,char* argv[]){
	if(argc!=4){
		err(1,"Error wrong number of arguments");
	}	

	int fd1 = open(argv[1] , O_RDONLY);

	if(fd1 == -1){
		err(2,"Cannot open first file");
	}

	int fd2 = open(argv[2] , O_RDONLY);

	if(fd2 == -1){
		err(3,"Cannot open second file");
	}

	struct stat s1;

	if(stat(argv[2] , &s1 ) == -1){
		err(3,"Cannot stat second file");
	}

	int fd3 = open(argv[3] , O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);

	if(fd3 == -1){
		err(3,"Cannot open third file");
	}

	int res;

	struct data d;
	int  i =0;
	while( (res = read(fd1,&d,sizeof(d))) == sizeof(d) ){
		  
		int size = s1.st_size;
		int offset = d.offset;

		if(offset > size-1){
			err(2,"Offset too far away");
		}	

		while(i<offset){
			if(lseek(fd2,i*sizeof(uint8_t), SEEK_SET) == -1){
				err(3,"lseek error");
			}

			uint8_t buf;

			if(read(fd2,&buf,sizeof(buf)) == -1){
				err(4,"Cannot read");
			}

			if(write(fd3,&buf,sizeof(buf)) == -1){
				err(5,"Cannot write to fd3");
			}
			i=i+1;
		}

		if(lseek(fd2,offset*sizeof(uint8_t) , SEEK_SET) == -1){
			err(6,"lseek error");
		}

		uint8_t r;

		if(read(fd2,&r,sizeof(r)) == -1){
			err(8,"Read error");
		}

		if(r != d.originalByte){
			err(9,"Original byte is different");
		}

		if(write(fd3,&d.newByte,sizeof(uint8_t)) == -1){
			err(10,"Cannot write to file");
		}
			
	}

	if(res<0){
		err(2,"Error reading");
	}
	
	close(fd1);
	close(fd2);
	close(fd3);
}

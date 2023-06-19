#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct dataHeader{
	uint32_t magic;
	uint32_t count;
}__attribute__((packed));

struct commHeader{
	uint32_t magic1;
	uint16_t magic2;
	uint16_t reserved;
	uint64_t count;
}__attribute__((packed));

struct commSec{
	uint16_t type;
	uint16_t res1;
	uint16_t res2;
	uint16_t res3;
	uint32_t offset1;
	uint32_t offset2;
}__attribute__((packed));






int main(int argc,char* argv[]){
	if( argc != 3){
		err(2,"Wrong number of args");
	}

	int fd1 = open(argv[1] , O_RDWR);

	if(fd1 == -1){
		err(3,"Cannot open first file");
	}

	int fd2 = open(argv[2] , O_RDONLY);

	if(fd2 == -1){
		err(4,"Cannot open second file");
	}

	struct dataHeader dh;
	struct commHeader ch;
	struct commSec cs;

	if(read(fd1,&dh,sizeof(dh)) == -1 ) {
		err(4,"Cannot read data header");
	}


	if(read(fd2,&ch,sizeof(ch)) == -1){
		err(4,"Cannot read commparotor header");
	}
	
	if(dh.magic != 0x21796F4A){
		err(5,"Error with magic");
	}

	struct stat s1;
	
	if(stat(argv[1] , &s1) == -1){
		err(6,"Cannot stat first file");
	}

	if(dh.count != (s1.st_size - sizeof(dh)) / sizeof(uint64_t) ) {
		err(7,"Counts differ");
	}

	if(ch.magic1 != 0xAFBC7A37 || ch.magic2 != 0x1C27){
		err(8,"Comparotor magic is incorrect");
	}

	struct stat s2;

	if(stat(argv[2],&s2) == -1){
		err(9,"Cannot stat second file");
	}	

	if(ch.count != ((s2.st_size - sizeof(ch)) / sizeof(cs) ) ) {
		err(10,"Counts not equal at second file");
	}
	
	int buf;

	while( (buf = read(fd2,&cs,sizeof(cs))) == sizeof(cs) ){
		if(cs.type != 0 && cs.type != 1){
			err(12,"Type is not 0 or 1");
		}	
		if(cs.res1 != 0 || cs.res2 != 0 || cs.res3 != 0){
			err(16,"Not all reserved are 0");
		}

		if(cs.offset1 > dh.count){
			err(13,"Cannot go to offset1");
		}		

		if(cs.offset2 > dh.count){
			err(14,"Cannot go to offset2");
		}

		if(lseek(fd1,cs.offset1*sizeof(uint64_t) + sizeof(dh) , SEEK_SET ) == -1){
			err(14,"Lseek error with offset1");
		}

		uint64_t x,y;

		if(read(fd1,&x,sizeof(uint64_t)) == -1){
			err(13,"Cannot read element at offset1");
		}

		if(lseek(fd1,cs.offset2 * sizeof(uint64_t) + sizeof(dh)  , SEEK_SET) == -1){
			err(14,"Lseek error with offset2");
		}

		if(read(fd1,&y,sizeof(uint64_t)) == -1){
			err(15,"Cannot read element at offset2");
		}

	
		if(cs.type == 0){
			if(x > y){ // if 0 x should be smaller than y , but if it's not then swap
	
				if(lseek(fd1,cs.offset1*sizeof(uint64_t) + sizeof(dh) , SEEK_SET) == -1){		
					err(16,"lseek error at offset1 when swaping");
				}				

				if(write(fd1,&y,sizeof(uint64_t)) == -1){
					err(16,"Cannot write at fd1");
				}

				
				if(lseek(fd1,cs.offset2*sizeof(uint64_t) + sizeof(dh) , SEEK_SET) == -1){
					err(17,"Cannot lseek fd1");
				}

				if(write(fd1,&x,sizeof(uint64_t)) == -1){
					err(18,"Cannot write to fd1");
				}
			}
		
		} else{ // else type is 1 and x should be greater than y
		
			if(x < y){
		
				if(lseek(fd1,cs.offset1*sizeof(uint64_t) + sizeof(dh) , SEEK_SET) == -1){
					err(12,"Cannot lseek to offset 1 at fd1");
				}

				if(write(fd1,&y,sizeof(uint64_t)) == -1){
					err(13,"Cannot write to fd1");
				}
				
				if(lseek(fd1,cs.offset2*sizeof(uint64_t) + sizeof(dh) , SEEK_SET ) == -1){
					err(14,"Cannot lseek at offset2");
				}

				if(write(fd1,&x,sizeof(uint64_t)) == -1){
					err(15,"Cannot write at fd1");
				}
		
			}	
		}

	}

	if(buf < 0){
		err(11,"Error reading comparator section");
	} 	

	
	close(fd1);
	close(fd2);
}

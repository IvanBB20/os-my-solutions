#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct data{
	uint16_t magic;
	uint16_t filetype;
	uint32_t count;
};

int main(int argc,char * argv[]){

	if(argc!=4){
		err(1,"Wrong number of args");
	}

	int fd1 = open(argv[1] , O_RDONLY);

	if(fd1 == -1){
		err(2,"Cannot open first file");
	}	

	struct stat s1;

	if(stat(argv[1] , &s1) == -1){
		err(3,"Cannot stat first  file");
	}

	struct data d1;
	struct data dummy;
	if(read(fd1 , &d1 , sizeof(dummy)) == -1){
		err(3,"Read error");
	}

	if(d1.magic != 0x5A4D){
		err(1,"Magic not correct");
	}

	if(d1.filetype != 1){
		err(2,"filetype error");
	}
	if(d1.count != (s1.st_size - sizeof(dummy))/sizeof(uint16_t)){
		err(2,"Count not equal");
	}

	

	int fd2 = open(argv[2] , O_RDONLY);

	if(fd2 == -1){
		err(3,"Cannot open second file");
	}


	struct stat s2;

	if(stat(argv[2] , &s2) == -1){
		err(3,"Cannot stat second file");
	}
	
	struct data d2;

	if(read(fd2,&d2,sizeof(dummy)) == -1) {
		err(4,"Cannot read second file");
	}

	if(d2.magic != 0x5A4D){
		err(5 , "Magic error on second file");
	}

	if(d2.filetype != 2){
		err(6,"Filetype error");
	}

	if(d2.count != (s2.st_size - sizeof(dummy) ) / sizeof(uint32_t) ){
		err(6,"Counts don't match");
	}

	

	int fd3 = open(argv[3] , O_WRONLY | O_TRUNC | O_CREAT , S_IRUSR | S_IWUSR);

	if(fd3 == -1) {
		err(3,"Error with third file");
	}

	
	int i = 0;

	uint16_t element;

	int res1;
	int count = -1;

	while( (res1 = read(fd1,&element,sizeof(uint16_t))) == sizeof(uint16_t) ){

		if(i > (int)d2.count - 1 ){
			err(3,"Error wrong position");
		}	
	
		if(lseek(fd2,i*sizeof(uint32_t) + sizeof(dummy),SEEK_SET) == -1){
			err(4,"lseek error");
		}

		uint32_t buf1;

		if(read(fd2,&buf1,sizeof(uint32_t)) == -1){
			err(5,"Read error");
		}

		if((int)buf1 > count){
			count = buf1;
		}		

		uint64_t buf2=buf1;

		if(lseek(fd3,element*sizeof(uint64_t) + sizeof(dummy) , SEEK_SET) == -1){
			err(4,"lseek error at fd3");
		}

		if(write(fd3,&buf2,sizeof(buf2)) == -1){
			err(4,"write error at fd3");
		}

	}

	if(res1 < 0){
		err(2,"Error reading");
	}	

	struct data fd3Header;
	fd3Header.filetype=3;
	fd3Header.magic=0x5A4D;
	fd3Header.count=count;

	if(lseek(fd3,0,SEEK_SET) == -1 ) {
		err(3,"Error lseek fd3 0");
	}

	if(write(fd3,&fd3Header,sizeof(dummy)) == -1){
		err(4,"error writing fd3Header");
	}
	close(fd1);
	close(fd2);
	close(fd3);
}

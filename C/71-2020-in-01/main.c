#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct header {
        uint32_t magic;
        uint8_t headerVer;
        uint8_t dataVer;
        uint16_t count;
        uint32_t res1;
        uint32_t res2;
} __attribute__((packed));

struct dataO {
        uint16_t off;
        uint8_t originalB;
        uint8_t newB;
} __attribute__((packed));

struct dataT {
        uint32_t offset;
        uint16_t originalW;
        uint16_t newW;
} __attribute__((packed));

int main(int argc, char *argv[]) {
        if (argc != 4) {
                err(1, "Not correct amount of args ");
        }

        int fd1 = open(argv[1], O_RDONLY);

        if (fd1 == -1) {
                err(2, "Cannot open first file");
        }

        struct stat s1;

        if (stat(argv[1], &s1) == -1) {
                err(3, "Cannot stat first file");
        }

        int fd2 = open(argv[2], O_RDONLY);

        if (fd2 == -1) {
                err(4, "Cannot open second file");
        }

        int fd3 =
            open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        if (fd3 == -1) {
                err(5, "Cannot open third file");
        }

        struct header h;

        if (read(fd1, &h, sizeof(h)) == -1) {
                err(1, "Cannot read from header from first file");
        }

        if (h.magic != 0xEFBEADDE) {
                err(3, "Magic not correct");
        }

        if (h.headerVer != 0x01) {
                err(4, "Header version not 0x01");
        }

        if (h.dataVer != 0x00 || h.dataVer != 0x01) {
                err(5, "Data version is not correct");
        }

        if (h.dataVer == 0x00) {

                int res;
                uint8_t buf;
                while ((res = read(fd2, &buf, sizeof(buf))) == sizeof(buf)) {
                        if (write(fd3, &buf, sizeof(buf)) == -1) {
                                err(3, "Cannot write to fd3");
                        }
                }
                if (res < 0) {
                        err(12, "Cannot read");
                }

                struct dataO d1;

                int res2;

                while ((res2 = read(fd1, &d1, sizeof(d1))) == sizeof(d1)) {
                        int offset = d1.off;
                        if (s1.st_size < offset) {
                                err(2, "Offset too far away");
                        }

                        if (lseek(fd2, offset * sizeof(uint8_t), SEEK_SET) ==
                            -1) {
                                err(3, "Cannot lseek at fd2");
                        }

                        uint8_t buffer;

                        if (read(fd2, &buffer, sizeof(buffer)) ==
                            sizeof(buffer)) {
                                err(4, "Cannot read at fd2");
                        }

                        if (d1.originalB != buffer) {
                                err(5,
                                    "original byte and buffer are different");
                        }

                        if (lseek(fd3, offset * sizeof(uint8_t), SEEK_SET) ==
                            -1) {
                                err(6, "Cannot lseek at fd3");
                        }

                        if (write(fd3, &d1.newB, sizeof(buffer)) == -1) {
                                err(7, "Cannot write at fd3");
                        }
                }

                if (res2 < 0) {
                        err(2, "Cannot read from fd1");
                }
        } else {

                int res;
                uint16_t buf;
                while ((res = read(fd2, &buf, sizeof(buf))) == sizeof(buf)) {
                        if (write(fd3, &buf, sizeof(buf)) == -1) {
                                err(3, "Cannot write to fd3");
                        }
                }
                if (res < 0) {
                        err(12, "Cannot read");
                }

                struct dataT d1;

                int res2;

                while ((res2 = read(fd1, &d1, sizeof(d1))) == sizeof(d1)) {
                        int offset = d1.offset;
                        if (s1.st_size < offset) {
                                err(2, "Offset too far away");
                        }

                        if (lseek(fd2, offset * sizeof(uint16_t), SEEK_SET) ==
                            -1) {
                                err(3, "Cannot lseek at fd2");
                        }

                        uint16_t buffer;

                        if (read(fd2, &buffer, sizeof(buffer)) ==
                            sizeof(buffer)) {
                                err(4, "Cannot read at fd2");
                        }

                        if (d1.originalW != buffer) {
                                err(5,
                                    "original byte and buffer are different");
                        }

                        if (lseek(fd3, offset * sizeof(uint16_t), SEEK_SET) ==
                            -1) {
                                err(6, "Cannot lseek at fd3");
                        }

                        if (write(fd3, &d1.newW, sizeof(buffer)) == -1) {
                                err(7, "Cannot write at fd3");
                        }
                }

                if (res2 < 0) {
                        err(2, "Cannot read from fd1");
                }
        }
        close(fd1);
        close(fd2);
        close(fd3);
}

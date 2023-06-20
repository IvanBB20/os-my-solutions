#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct header {
        uint32_t buf1;  // 4
        uint16_t count; // 2
        uint64_t buf2;  // 8
        uint16_t buf3;  // 2

} __attribute__((packed));

struct interval {
        uint16_t start;
        uint16_t n;
} __attribute__((packed));

struct complect {
        struct interval po;
        struct interval pr;
        struct interval in;
        struct interval su;
} __attribute__((packed));

int main(int argc, char *argv[]) {
        if (argc != 7) {
                err(1, "Wrong number of args");
        }

        int affix = open(argv[1], O_RDONLY);

        if (affix == -1) {
                err(2, "Cannot open affix");
        }

        struct stat aff;

        if (stat(argv[1], &aff) == -1) {
                err(3, "Cannot stat file");
        }

        struct header h1;

        if (read(affix, &h1, sizeof(h1)) == -1) {
                err(4, "Cannot read header from affix");
        }

        struct complect cc;
        if (h1.count != ((aff.st_size - sizeof(h1)) / (sizeof(cc) * 2))) {
                err(3, "Wrong number of count in affix");
        }

        int postfix = open(argv[2], O_RDONLY);

        if (postfix == -1) {
                err(4, "Cannot open postfix");
        }

        struct stat pos;

        if (stat(argv[2], &pos) == -1) {
                err(5, "Cannot stat postfix");
        }

        struct header h2;

        if (read(postfix, &h2, sizeof(h2)) == -1) {
                err(6, "Cannot read header from postfix");
        }

        if (h2.count != ((pos.st_size - sizeof(h2)) / sizeof(uint32_t))) {
                err(7, "Count not equal in postfix");
        }

        int prefix = open(argv[3], O_RDONLY);

        if (prefix == -1) {
                err(8, "Cannot open prefix");
        }

        struct stat pre;

        if (stat(argv[3], &pre) == -1) {
                err(9, "Cannot stat preffix");
        }

        struct header h3;

        if (read(prefix, &h3, sizeof(h3)) == -1) {
                err(10, "Cannot read header from prefix");
        }

        if (h3.count != ((pre.st_size - sizeof(h3)) / sizeof(uint8_t))) {
                err(11, "Count not correct at prefix");
        }

        int infix = open(argv[4], O_RDONLY);

        if (infix == -1) {
                err(12, "Cannot open infix");
        }

        struct stat inf;

        if (stat(argv[4], &inf) == -1) {
                err(13, "Cannot stat infix");
        }

        struct header h4;

        if (read(infix, &h4, sizeof(h4)) == -1) {
                err(14, "Cannot read infix header");
        }

        if (h4.count != ((inf.st_size - sizeof(h4)) / (sizeof(uint16_t)))) {
                err(15, "Not equal counts in infix");
        }

        int suffix = open(argv[5], O_RDONLY);

        if (suffix == -1) {
                err(16, "Cannot open suffix");
        }

        struct stat suf;

        if (stat(argv[5], &suf) == -1) {
                err(17, "Cannot stat suffix");
        }

        struct header h5;

        if (read(suffix, &h5, sizeof(h5)) == -1) {
                err(18, "Cannot read head suffux");
        }

        if (h5.count != ((suf.st_size - sizeof(h5)) / sizeof(uint64_t))) {
                err(19, "Counts in suffix not equal");
        }

        int crucifixus =
            open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        if (crucifixus == -1) {
                err(20, "Cannot open crucifixus");
        }

        struct complect c;
        int res;
        while ((res = read(affix, &c, sizeof(c))) == sizeof(c)) {
                struct interval p1 = c.po;
                struct interval p2 = c.pr;
                struct interval p3 = c.in;
                struct interval p4 = c.su;

                if (lseek(postfix, p1.start * sizeof(uint32_t) + sizeof(h1),
                          SEEK_SET) == -1) {
                        err(21, "Error lseek postfix");
                }

                uint8_t b;
                int n = p1.n;
                uint32_t b1;
                for (int j = 0; j < n; j++) {
                        if (read(postfix, &b1, sizeof(b1)) == -1) {
                                err(22, "Error");
                        }
                        b = b1;
                        if (write(crucifixus, &b, sizeof(b)) == -1) {
                                err(23, "Error write");
                        }
                }

                if (lseek(prefix, p2.start * sizeof(uint8_t) + sizeof(h1),
                          SEEK_SET) == -1) {
                        err(1, "Error");
                }

                n = p2.n;
                uint8_t buffPre;
                for (int j = 0; j < n; j++) {
                        if (read(prefix, &buffPre, sizeof(buffPre)) == -1) {
                                err(2, "Error");
                        }
                        b = buffPre;
                        if (write(crucifixus, &b, sizeof(b)) == sizeof(b)) {
                                err(3, "Cannot write");
                        }
                }

                if (lseek(infix, p3.start * sizeof(uint16_t) + sizeof(h1),
                          SEEK_SET) == -1) {
                        err(1, "Error");
                }

                n = p3.n;
                uint16_t buffInf;
                for (int j = 0; j < n; j++) {
                        if (read(infix, &buffInf, sizeof(buffInf)) == -1) {
                                err(2, "Error");
                        }

                        b = buffInf;

                        if (write(crucifixus, &b, sizeof(b)) == -1) {
                                err(4, "Error");
                        }
                }

                if (lseek(suffix, p4.start * sizeof(uint64_t) + sizeof(h1),
                          SEEK_SET) == -1) {
                        err(4, "Error");
                }

                n = p4.n;
                uint64_t buffSuf;
                for (int j = 0; j < n; j++) {
                        if (read(suffix, &buffSuf, sizeof(buffSuf)) == -1) {
                                err(5, "Error");
                        }

                        b = buffSuf;

                        if (write(crucifixus, &b, sizeof(b)) == -1) {
                                err(12, "ERROR");
                        }
                }
        }

        if (res < 0) {
                err(2, "Cannot read from affix");
        }
        close(affix);
        close(postfix);
        close(prefix);
        close(infix);
        close(suffix);
        close(crucifixus);
        return 0;
}

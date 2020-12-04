#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *wavdata;
long wavdatalen;
char *p;

long getrestdatalen()
{
    return wavdatalen - (p - wavdata);
}

long getfsize(FILE *fp)
{
    long curpos;
    long fsize;
    char errmsg[80];

    if ((curpos = ftell(fp)) < 0) {
        errmsg[sizeof(errmsg) - 2] = '\0';
        strerror_r(errno, errmsg, sizeof(errmsg) - 1);
        fprintf(stderr, "Error: ftell(), errno=%d(%s)\n", errno, errmsg);
        return -1;
    } 
    if (fseek(fp, 0, SEEK_END) != 0) {
        errmsg[sizeof(errmsg) - 2] = '\0';
        strerror_r(errno, errmsg, sizeof(errmsg) - 1);
        fprintf(stderr, "Error: fseek(%d, %d), errno=%d(%s)\n", 0, SEEK_END, errno, errmsg);
        return -2;
    }
    if ((fsize = ftell(fp)) < 0) {
        errmsg[sizeof(errmsg) - 2] = '\0';
        strerror_r(errno, errmsg, sizeof(errmsg) - 1);
        fprintf(stderr, "Error: ftell(), errno=%d(%s)\n", errno, errmsg);
        return -3;
    }
    if (fseek(fp, curpos, SEEK_SET) != 0) {
        errmsg[sizeof(errmsg) - 2] = '\0';
        strerror_r(errno, errmsg, sizeof(errmsg) - 1);
        fprintf(stderr, "Error: fseek(%ld, %d), errno=%d(%s)\n", curpos, SEEK_END, errno, errmsg);
        return -2;
    }

    return fsize;
}

int readwavdata(const char fname[])
{
    FILE *fp;
    long fsize;
    char errmsg[80];

    if ((fp = fopen(fname, "r")) == NULL) {
        errmsg[sizeof(errmsg) - 2] = '\0';
        strerror_r(errno, errmsg, sizeof(errmsg) - 1);
        fprintf(stderr, "Error: fopen(%s), errno=%d(%s)\n", fname, errno, errmsg);
        return 1;
    }

    if ((fsize = getfsize(fp)) < 0) {
        fclose(fp);
        return 2;
    }
    if ((p = wavdata = malloc(fsize)) == NULL) {
        fprintf(stderr, "Error: malloc(%ld), errno=%d(%s)\n", fsize, errno, errmsg);
        fclose(fp);
        return 3;
    }

    wavdatalen = fread(wavdata, 1, fsize, fp);
    if (wavdatalen < fsize && !feof(fp)) {
        fprintf(stderr, "Error: fread(%ld), errno=%d(%s)\n", fsize, errno, errmsg);
        fclose(fp);
        return 4;
    }
    wavdatalen = fsize;

    fclose(fp);

    return 0;
}

void newwavdata()
{
    FILE *fp;

    if ((fp = fopen("newwav.wav", "w")) == NULL) {
        perror("fopen");
        return;
    }

    if (fwrite(wavdata, wavdatalen, 1, fp) != 1) {
        perror("fwrite");
    }

    fclose(fp);
}

char *gettag()
{
    static char tag[4];
    int i;
    
    if (getrestdatalen() < 4) {
        return NULL;
    }
    for (i = 0; i < 4; i++) {
        tag[i] = *p++;
    }
    return tag;
}

int getlength(unsigned long *length)
{
    unsigned long len;

    if (getrestdatalen() < 4) {
        return -1;
    }
    len =              ((unsigned char)*(p + 3)) & 0xff;
    len = (len << 8) | (((unsigned char)*(p + 2)) & 0xff);
    len = (len << 8) | (((unsigned char)*(p + 1)) & 0xff);
    len = (len << 8) | (((unsigned char)*(p)) & 0xff);

    *length = len;
    p += 4;

    return 0;
}

void resetlength(unsigned long length)
{
    p -= 4;
    *p++ = (length & 0xff);
    length = length >> 8;
    *p++ = (length & 0xff);
    length = length >> 8;
    *p++ = (length & 0xff);
    length = length >> 8;
    *p++ = (length & 0xff);
    length = length >> 8;
}

typedef enum {
    TAG_RIFF = 1, TAG_WAVE, TAG_JUNK, TAG_fmt, TAG_LIST, TAG_END = -1
} tag_t;

tag_t gettagid()
{
    return TAG_END;
}

int main(int argc, char *argv[])
{
//    FILE *fp;
    int rc;
    int isfix;

    if (readwavdata(argv[1]) != 0) {
        return 1;
    }

    rc = 0;
    isfix = 0;
    while (getrestdatalen() > 0) {
        char *tag;
        unsigned long size;

        if ((tag = gettag()) == NULL) {
            rc = 1;
            break;
        }
        if (strncmp(tag, "WAVE", 4) == 0) {
            printf("TAG: %.4s\n", tag);
        } else {
            if (getlength(&size)) {
                rc = 1;
                break;
            }
            if (strncmp(tag, "RIFF", 4) == 0) {
                if (size == getrestdatalen()) {
                    printf("TAG: %.4s(%lu)\n", tag, size);
                }
                else {
                    printf("TAG: %.4s(%lu)<== Should be %lu\n", tag, size, getrestdatalen());
		    resetlength(getrestdatalen());
                    isfix = 1;
                }
                if (strncmp(tag, "RIFF", 4) == 0) {
                    p += 0;
                } else if (strncmp(tag, "data", 4) == 0) {
                    p += getrestdatalen();
                }
            } else {
                if (size <= getrestdatalen()) {
                    printf("TAG: %.4s(%lu)\n", tag, size);
                } else {
                    printf("TAG: %.4s(%lu)<== less than %lu\n", tag, size, getrestdatalen());
                    size = getrestdatalen();
		    resetlength(size);
                    isfix = 1;
                }
                p += size;
            }
        }
    }

    if (isfix) {
        newwavdata();
    }

    return rc;
}

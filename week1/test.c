#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *file = fopen("/dev/opdr_3_4", "r+");
    char buf[256];
    fgets(buf, 10, file);

    fputs("hello there", file);

    fclose(file);

    return 0;
}

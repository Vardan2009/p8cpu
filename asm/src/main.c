#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "p8asm: usage: %s <.asm path>\n", argv[0]);
        return 1;
    }

    FILE *fptr;
    char buffer[512];

    fptr = fopen(argv[1], "r");

    if (fptr == NULL) {
        perror("p8asm");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) printf("%s", buffer);

    fclose(fptr);

    return 0;
}

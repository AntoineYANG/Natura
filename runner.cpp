#include <stdlib.h>
#include <stdio.h>

int main() {
    printf("running cat with system.\n");
    system("g++ hw.cpp");
    system("hw.exe");
    printf("-----------\nDone.\n");

    return 0;
}


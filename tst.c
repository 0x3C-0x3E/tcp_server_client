#include <stdio.h>
#include <time.h>

int main() {
    time_t timer;
    time(&timer);
    printf("TIME NOW: %ld\n", timer);
    return 0;
}

#include <cstdio>
#include <cstdlib>
#include "RingBufCPP.h"


int main() {
    RingBufCPP<int, 5> q;
    int tmp = -1;

    for (uint16_t i = 0; i < 100; i++) {
        tmp = rand(); // NOLINT

        if (q.add(tmp)) {
            printf("Added %d\n", tmp);
        }
        else {
            q.pull(&tmp);
            printf("Buffer is full. Pulled %d\n", tmp);
            break;
        }
    }

    while (!q.isEmpty()) {
        int pulled;
        q.pull(&pulled);
        printf("Got %d\n", pulled);
    }
}

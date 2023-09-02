#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"

int main(void)
{
    // mmap() 전에 freemem() 테스트
    printf(1, "Free memory before mmap: %d\n", freemem());

    int mmap_length = 4096;
    uint mapped_memory = mmap(0, mmap_length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);

    // mmap()이 제대로 됐는지 체크
    if (mapped_memory == 0)
    {
        printf(1, "mmap failed\n");
        exit();
    }

    // mmap() 후에 Freemem() 테스트
    printf(1, "Free memory after mmap: %d\n", freemem());

    // mapped memory로부터 읽고 쓸 수 있는지 테스트
    int *data = (int *)mapped_memory;
    *data = 42;
    printf(1, "Value at mapped memory: %d\n", *data);

    // munmap() 테스트
    if (munmap((uint)mapped_memory) < 0)
    {
        printf(1, "munmap failed\n");
        exit();
    }

    // munmap() 후에 freemem() 테스트
    printf(1, "Free memory after munmap: %d\n", freemem());

    exit();
}

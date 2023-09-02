#include "types.h"
#include "stat.h"
#include "user.h"

#define NUMCASES 6
int score = 0;

void test_p1()
{
	int pid;

	printf(1, "case 1. get nice value of init process: ");
	if (getnice(1) == 20) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");

	printf(1, "case 2. get nice value of this process: ");
	pid = getpid();
	if (getnice(pid) == 20) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");

	printf(1, "case 3. get nice value of non-existing process: ");
	if (getnice(100) == -1 && getnice(-1) == -1) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");

	printf(1, "case 4. set nice value of current process: ");
	pid = getpid();
	setnice(pid, 10);
	if (getnice(pid) == 10) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");

	printf(1, "case 5. set nice value of non-existing process: ");
	if (setnice(100, 10) == -1) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");

	printf(1, "case 6. set wrong nice value of current process: ");
	if (setnice(pid, -1) == -1 && setnice(pid, 100) == -1) {
		printf(1, "OK\n");
		score++;
	}
	else
		printf(1, "WRONG\n");



}

int main(int argc, char **argv)
{
	printf(1, "=== TEST START ===\n");
	test_p1();

	printf(1, "total score: %d/%d\n", score, NUMCASES);
	printf(1, "case 7. ps(pid) test\n");
	ps(2);
	printf(1, "case 8. ps all test\n");
	ps(0);
	printf(1, "=== TEST   END ===\n");
	exit();
}

#include "types.h"
#include "user.h"
#include "stat.h"



int main(int argc, char *argv[])
{
	printf(1,"===========test 0========\n");
	ps(-1);
	printf(1,"setnice value : %d \n",setnice(0,21));
	printf(1,"getnice value : %d \n",getnice(0));

	printf(1,"=========test right=======\n");
	ps(0);
	ps(1);
	ps(2);
	ps(3);
	printf(1,"answer 20 getnice : %d \n",getnice(2));
	printf(1,"answer -1 getnice : %d \n",getnice(25));
	
	printf(1,"==========test wrong========\n");
	ps(25);
	printf(1,"answer -1 setnice : %d\n",setnice(2,39));
	printf(1,"answer -1 setnice : %d\n",setnice(2,-1));
	printf(1,"answer -1 getnice : %d\n",getnice(-1));	
	
    exit();
}

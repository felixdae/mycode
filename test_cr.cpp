#include <stdio.h>
#include <unistd.h>

int main()
{
    for (char c='a';c<='z';c++)
    {   
        for (int i=0; i<10; ++i)
        {   
            printf("%c", c); 
            fflush(stdout);
            usleep(100*1000);
        }   
        printf("\r");
    }   
    return 0;
}






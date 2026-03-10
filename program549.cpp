#include<stdio.h>
#include<stdlib.h>

//Conditional Preprocessing
int main()
{
    #ifdef _WIN32               //for windows
        system("dir");
    #else
        system("ls");           //for linux based OS
    #endif
    
    return 0;
}
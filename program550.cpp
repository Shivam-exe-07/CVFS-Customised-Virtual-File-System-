#include<stdio.h>
#include<stdlib.h>

//Conditional Preprocessing
int main()
{
    #ifdef _WIN32               //for windows
        system("cls");
    #else
        system("clear");           //for linux based OS
    #endif
    
    return 0;
}
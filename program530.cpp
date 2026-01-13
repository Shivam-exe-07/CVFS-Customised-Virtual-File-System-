/////////////////////////////////////////////////////////////////////
//
//  Header File Inclusion
//
/////////////////////////////////////////////////////////////////////

#include<stdio.h>       
#include<stdlib.h>      
#include<fcntl.h>
#include<unistd.h>  
#include<stdbool.h>     
#include<string.h>      

/////////////////////////////////////////////////////////////////////
//
//  User Defined Macros
//
/////////////////////////////////////////////////////////////////////

//Maximum file size that we allow in the project
#define MAXFILESIZE 50          //File size is 50 we can increase as required

#define MAXOPENFILES 20         //Total Open Files : 20

#define MAXINODE 5              //we are going to create 5 inode linked list

//Both Read and Write is needed in this (3) and execute is not needed in this project but we take permission as 0777
#define READ 1
#define WRITE 2
#define EXECUTE 4

#define START 0
#define CURRENT 1
#define END 2

#define EXECUTE_SUCCESS 0

/////////////////////////////////////////////////////////////////////
//
//  User Defined Structures
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :    BootBlock 
//  Description    :    Holds the information to boot the OS
//
/////////////////////////////////////////////////////////////////////

struct BootBlock
{
    char Information[100];
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name  :   SuperBlock 
//  Description     :   Holds the Information about the file system    
//
/////////////////////////////////////////////////////////////////////

struct SuperBlock
{
   int TotalInodes;
   int FreeInodes;
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name  :   Inode 
//  Description     :   Holds the Information about the file    
//
/////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct Inode
{
    char FileName[20];
    int InodeNumber;
    int FileSize;
    int ActualFileSize;
    int ReferenceCount;
    int Permission;             //1,2,4,6,7 //technically we give 3(read + write) but now we will give 7 all permission included
    char *Buffer;
    struct Inode *next;         //self referencial structure
};

typedef struct Inode INODE;
typedef struct Inode * PINODE;
typedef struct Inode ** PPINODE;

/////////////////////////////////////////////////////////////////////
//
//  Structure Name  :   FileTable 
//  Description     :   Holds the Information about the opened file    
//
/////////////////////////////////////////////////////////////////////

struct FileTable
{
    int ReadOffset;
    int WriteOffset;
    int Mode;
    PINODE ptrinode;
};

typedef FileTable FILETABLE;
typedef FileTable * PFILETABLE;

/////////////////////////////////////////////////////////////////////
//
//  Structure Name  :   UAREA 
//  Description     :   Holds the Information about the Process   
//
/////////////////////////////////////////////////////////////////////

struct UAREA
{
    char ProcessName[20];
    PFILETABLE UFDT[MAXOPENFILES];
};

/////////////////////////////////////////////////////////////////////
//
//  Global variables or objects used in the project  
//
/////////////////////////////////////////////////////////////////////

BootBlock bootobj;
SuperBlock superobj;
UAREA uareaobj;

PINODE head = NULL;

/////////////////////////////////////////////////////////////////////
//
//  Entry Point Function of the project
//
/////////////////////////////////////////////////////////////////////

int main()
{


    return 0;
}
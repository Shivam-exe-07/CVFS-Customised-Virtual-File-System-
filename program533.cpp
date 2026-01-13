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
//  Function Name   :   InitialiseUAREA
//  Description     :   It is used to initialise UAREA members
//  Author          :   Shivam Santosh Thakur
//  Date            :   13/01/2026
//
/////////////////////////////////////////////////////////////////////

void InitialiseUAREA()
{
    strcpy(uareaobj.ProcessName,"Myexe");

    int i = 0;

    for(i = 0; i<MAXOPENFILES; i++)
    {
        uareaobj.UFDT[i] = NULL;
    }
    
    printf("Marvellous CVFS : UAREA gets initialized succesfully\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   :   InitialiseSuperBlock
//  Description     :   It is used to initialise Super Block members
//  Author          :   Shivam Santosh Thakur
//  Date            :   13/01/2026
//
/////////////////////////////////////////////////////////////////////

void InitialiseSuperBlock()
{
    superobj.TotalInodes = MAXINODE;
    superobj.FreeInodes = MAXINODE;

    printf("Marvellous CVFS : Super Block gets initialized succesfully\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   :   CreateDILB
//  Description     :   It is used to create linked list of inode
//  Author          :   Shivam Santosh Thakur
//  Date            :   13/01/2026
//
/////////////////////////////////////////////////////////////////////

void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    for(i = 1; i <= MAXINODE; i++)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        strcpy(newn->FileName,"\0");
        newn->InodeNumber = i;
        newn->FileSize = 0;
        newn->ActualFileSize = 0;
        newn->ReferenceCount = 0;
        newn->Permission = 0;
        newn->Buffer = NULL;
        newn->next = NULL;

        if(temp == NULL)        //LL is Empty
        {
            head = newn;
            temp = head;
        }
        else                    //LL contains atleast 1 node
        {
            temp->next = newn;
            temp = temp->next;
        }
    }

    printf("Marvellous CVFS : DILB created succesfully\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   :   StartAuxillaryDataInitialisation
//  Description     :   It is used to call all such functions which are used to initialise auxillary data
//  Author          :   Shivam Santosh Thakur
//  Date            :   13/01/2026
//
/////////////////////////////////////////////////////////////////////

void StartAuxillaryDataInitialisation()
{
    strcpy(bootobj.Information,"Booting process of Marvellous CVFS is done");

    printf("%s\n",bootobj.Information);

    InitialiseSuperBlock();

    CreateDILB();

    InitialiseUAREA();

    printf("Marvellous CVFS : Auxillary data initialized succesfully\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Entry Point Function of the project
//
/////////////////////////////////////////////////////////////////////

int main()
{
    StartAuxillaryDataInitialisation();

    return 0;
}
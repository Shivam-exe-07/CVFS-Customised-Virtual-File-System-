/////////////////////////////////////////////////////////////////////
//
//  Header File Inclusion
//  Description : Includes all standard libraries required for
//                input/output operations, memory management,
//                file handling, string manipulation, and
//                boolean operations.
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

// Defines the maximum allowed size of a file (in bytes)
#define MAXFILESIZE 50          

// Defines the maximum number of files that can be opened simultaneously
#define MAXOPENFILES 20         

// Defines the maximum number of inodes available in the file system
#define MAXINODE 5              

// File permission macros
// READ  -> Read permission
// WRITE -> Write permission
// EXECUTE -> Execute permission (not used but kept for completeness)
#define READ 1
#define WRITE 2
#define EXECUTE 4

// File offset macros used for file pointer positioning
#define START 0
#define CURRENT 1
#define END 2

// Macro to indicate successful execution of a function
#define EXECUTE_SUCCESS 0


/////////////////////////////////////////////////////////////////////
//
//  User Defined Structures
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
//  Structure Name : BootBlock
//  Description    : Stores boot-related information of the
//                   Custom Virtual File System.
//                   This structure simulates the boot sector
//                   information in an actual operating system.
//                   (Helps to boot the OS).
//
/////////////////////////////////////////////////////////////////////

struct BootBlock
{
    char Information[100];
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name : SuperBlock
//  Description    : Stores metadata about the file system.
//                   It keeps track of total inodes and
//                   available free inodes.
//
/////////////////////////////////////////////////////////////////////

struct SuperBlock
{
   int TotalInodes;
   int FreeInodes;
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name : Inode
//  Description    : Represents a file in the Custom Virtual
//                   File System.
//                   It stores file metadata such as file name,
//                   size, permissions, reference count,
//                   and a pointer to the file data buffer.
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
//  Structure Name : FileTable
//  Description    : Stores information about an opened file.
//                   It maintains read and write offsets,
//                   access mode, and a pointer to the
//                   corresponding inode.
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
//  Structure Name : UAREA
//  Description    : Represents the User Area of a process.
//                   It contains the process name and an
//                   array of file descriptors (UFDT)
//                   that point to opened files.
//
/////////////////////////////////////////////////////////////////////

struct UAREA
{
    char ProcessName[20];
    PFILETABLE UFDT[MAXOPENFILES];
};

/////////////////////////////////////////////////////////////////////
//
//  Global Variables
//  Description : Global objects used across the CVFS project
//                to maintain system-wide information.
//
/////////////////////////////////////////////////////////////////////

BootBlock bootobj;        // Stores boot information
SuperBlock superobj;      // Stores file system metadata
UAREA uareaobj;           // Stores process-related information

PINODE head = NULL;       // Head pointer of the inode linked list

/////////////////////////////////////////////////////////////////////
//
//  Function Name   : InitialiseUAREA
//  Description     : Initializes the UAREA (User Area) structure.
//                    It sets the process name and initializes all
//                    User File Descriptor Table (UFDT) entries to NULL,
//                    indicating that no files are currently opened
//                    by the process.
//  Input           : None
//  Output          : None
//  Return Value    : None
//  Author          : Shivam Santosh Thakur
//  Date            : 13/01/2026
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
//  Function Name   : InitialiseSuperBlock
//  Description     : Initializes the SuperBlock of the file system.
//                    It sets the total number of inodes and marks
//                    all inodes as free at the start of the system.
//                    SuperBlock maintains overall file system metadata.
//  Input           : None
//  Output          : None
//  Return Value    : None
//  Author          : Shivam Santosh Thakur
//  Date            : 13/01/2026
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
//  Function Name   : CreateDILB
//  Description     : Creates the Disk Inode List Block (DILB).
//                    It dynamically allocates memory for a linked
//                    list of inodes. Each inode represents a file
//                    and is initialized with default values.
//                    The linked list size is determined by MAXINODE.
//  Input           : None
//  Output          : None
//  Return Value    : None
//  Author          : Shivam Santosh Thakur
//  Date            : 13/01/2026
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
//  Function Name   : StartAuxillaryDataInitialisation
//  Description     : Initializes all auxiliary data structures
//                    required for the Custom Virtual File System.
//                    This includes BootBlock, SuperBlock,
//                    Disk Inode List Block (DILB), and UAREA.
//                    This function acts as a startup routine
//                    for the CVFS.
//  Input           : None
//  Output          : None
//  Return Value    : None
//  Author          : Shivam Santosh Thakur
//  Date            : 13/01/2026
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
//  Function Name   : DisplayHelp
//  Description     : It is used to display the help page
//  Author          : Shivam Santosh Thakur
//  Date            : 14/01/2026
//
/////////////////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("---------------------------------------------------------------\n");
    printf("-------------------Marvellous CVFS Help Page-------------------\n");
    printf("---------------------------------------------------------------\n");

    printf("man     : it is used to display the manual page\n");
    printf("clear   : it is used to clear the terminal\n");
    printf("create  : it is used to create new file\n");
    printf("write   : it is used to write the data into file\n");
    printf("read    : it is used to read the data from the file\n");
    printf("stat    : it is used to display the statistical information\n");
    printf("unlink  : it is used to delete the file\n");
    printf("exit    : it is used to terminate Marvellous CVFS\n");

    printf("---------------------------------------------------------------\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Entry Point Function of the project
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
//  Function Name   : main
//  Description     : Entry point of the Custom Virtual File System.
//                    It initializes all required file system
//                    components and starts the command-line
//                    interface for user interaction.
//                    The program runs in an infinite loop,
//                    waiting for user commands.
//  Input           : None
//  Output          : None
//  Return Value    : Integer (Execution status)
//  Author          : Shivam Santosh Thakur
//  Date            : 13/01/2026
//
/////////////////////////////////////////////////////////////////////


int main()
{
    char str[80] = {'\0'};
    char Command[5][20] = {{'\0'}};
    int iCount = 0;

    StartAuxillaryDataInitialisation();

    printf("---------------------------------------------------------------\n");
    printf("--------------Marvellous CVFS started succesfully--------------\n");
    printf("---------------------------------------------------------------\n");

    //Infine Listening Shell
    while(1)                        // Infinite loop to keep the CVFS running continuously
    {
        fflush(stdin);              // Clears the input buffer to remove any leftover characters
                                    // from previous user input before accepting new command

        strcpy(str,"");             // Clears the command string to avoid using old input data

        printf("Marvellous CVFS : > ");  
                                    // Displays the command prompt for user interaction

        fgets(str, sizeof(str), stdin);
                                    // Reads the complete command entered by the user
                                    // including spaces, until a newline is encountered
                                    
        iCount = sscanf(str,"%s %s %s %s %s", Command[0],Command[1], Command[2], Command[3], Command[4]);

        fflush(stdin);

        if(iCount == 1)
        {
            if(strcmp("exit",Command[0]) == 0)
            {
                printf("Thank you for using Marvellous CVFS\n");
                printf("Deallocating all the allocated resources");

                break;
            }
            else if (strcmp("ls",Command[0]) == 0)
            {
                printf("Inside ls\n");
            }
            else if(strcmp("help",Command[0]) == 0)
            {
                DisplayHelp();
            }
            
        }   //End of else if 1
        else if(iCount == 2)
        {

        }   //End of else if 2
        else if(iCount == 3)
        {

        }   //End of else if 3
        else if(iCount == 4)
        {

        }   //End of else if 4
        else
        {
            printf("Command not found\n");
            printf("Please refer help option to get more information\n");
        }   //End of else
    }   //End of while


    return 0;
}
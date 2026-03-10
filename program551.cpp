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

#define REGULARFILE 1
#define SPECIALFILE 2

/////////////////////////////////////////////////////////////////////
//
//  User Defined Macros for Error handling
//
/////////////////////////////////////////////////////////////////////

#define ERR_INVALID_PARAMETER -1

#define ERR_NO_INODES -2

#define ERR_FILE_ALREADY_EXIST -3
#define ERR_FILE_NOT_EXIST -4

#define ERR_PERMISSION_DENIED -5

#define ERR_INSUFFICIENT_SPACE -6
#define ERR_INSUFFICIENT_DATA -7

#define ERR_MAX_FILES_OPEN -8

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
    int FileType;
    int ActualFileSize;
    int ReferenceCount;
    int Permission;             
    // 1 = READ, 2 = WRITE, 4 = EXECUTE
    // Combined permissions: 3 (R+W), 5 (R+X), 6 (W+X), 7 (R+W+X)
    char *Buffer;
    struct Inode *next;         //self referencial structure(Pointer to next inode in Disk Inode List Block (DILB))
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
        newn->FileType = 0;
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
    // Store booting message in boot block
    strcpy(bootobj.Information,"Booting process of Marvellous CVFS is done");

    // Display booting message on screen
    printf("%s\n",bootobj.Information);

    // Initialize super block (total and free inodes)
    InitialiseSuperBlock();

    // Create inode linked list (DILB)
    CreateDILB();

    // Initialize user area and file descriptor table
    InitialiseUAREA();

    // Final message after successful initialization
    printf("Marvellous CVFS : Auxillary data initialized succesfully\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   : DisplayHelp
//  Description     : Displays the help page containing
//                    a list of supported commands and
//                    their basic usage information.
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
//  Function Name   : ManPageDisplay
//  Description     : Displays the manual page for the
//                    specified command, providing
//                    detailed usage and description.
//  Author          : Shivam Santosh Thakur
//  Date            : 14/01/2026
//
/////////////////////////////////////////////////////////////////////


void ManPageDisplay(char Name[])
{
    if(strcmp("ls",Name) == 0)
    {
        printf("About : It is used to list the names of all files\n");
        printf("Usage : ls\n");
    }
    else if(strcmp("man",Name) == 0)
    {
        printf("About : It is used to display manual page\n");
        printf("Usage : man command_name\n");
        printf("command_name : It is the name of command\n");
    }
    else if(strcmp("exit",Name) == 0)
    {
        printf("About : It is used to terminate the shell\n");
        printf("Usage : exit\n");
    }
    else if(strcmp("clear",Name) == 0)
    {
        printf("About : It is used to clear the shell\n");
        printf("Usage : clear\n");
    }
    else
    {
        printf("No manual entry for %s\n",Name);
    }
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   : IsFileExist
//  Description     : It is used to check whether file is exist or not.
//  Input           : It accepts file name 
//  Output          : It returns the boolean(T/F)
//  Author          : Shivam Santosh Thakur
//  Date            : 16/01/2026
//
/////////////////////////////////////////////////////////////////////

bool IsFileExist(
                    char *name      //File name
                )
{
    PINODE temp = head;
    bool bFlag = false;

    while(temp != NULL)
    {
        if((strcmp(name,temp -> FileName) == 0) && (temp->FileType == REGULARFILE))
        {
            bFlag = true;
            break;
        }
        temp = temp->next;
    }

    return bFlag;
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name   : CreateFile
//  Description     : It is used to create new regular file.
//  Input           : It accepts file name and permissions
//  Output          : It returns the file descriptors
//  Author          : Shivam Santosh Thakur
//  Date            : 16/01/2026
//
/////////////////////////////////////////////////////////////////////

int CreateFile(
                    char *name,             //  Name of new file
                    int permission          //  Permission for that file
                )
{
    PINODE temp = head;
    int i = 0;

    printf("Total number of inodes remaining : %d\n",superobj.FreeInodes);

    // if name is missing
    if(name == NULL)
    {
        return ERR_INVALID_PARAMETER;
    }

    // if permission value is wrong
    // permission : 1 -> (Read)
    // permission : 2 -> (Write)
    // permission : 3 -> (Read + Write)
    if(permission < 1 || permission > 3)
    {
        return ERR_INVALID_PARAMETER;
    }

    // if the inodes are full
    if(superobj.FreeInodes == 0)
    {
        return ERR_NO_INODES;
    }

    // if file is already present
    if(IsFileExist(name) == true)
    {
        return ERR_FILE_ALREADY_EXIST;
    }

    //Search empty Inode
    while(temp != NULL)
    {
        if(temp -> FileType == 0)
        {
            break;
        }
        temp = temp->next;
    }

    if(temp == NULL)
    {
        printf("There is no Inode\n");
        return ERR_NO_INODES;
    }

    // Search for empty UFDT entry
    // Note : 0,1,2 are reserved(0:stdin, 1:stdout, 2:stderr)
    for (i = 3; i < MAXOPENFILES; i++)  
    {
        if(uareaobj.UFDT[i]==NULL)
        {
            break;
        }
    }

    // UFDT is full
    if(i == MAXOPENFILES)
    {
        return ERR_MAX_FILES_OPEN;
    }
    
    // ALlocate memory for file table
    uareaobj.UFDT[i] = (PFILETABLE)malloc(sizeof(FILETABLE));

    // Initialize file table
    uareaobj.UFDT[i]->ReadOffset = 0;
    uareaobj.UFDT[i]->WriteOffset = 0;
    uareaobj.UFDT[i]->Mode = permission;

    //Connect File Table with Inode
    uareaobj.UFDT[i]->ptrinode = temp;

    //Initialize elements of Inode
    strcpy(uareaobj.UFDT[i]->ptrinode->FileName,name);
    uareaobj.UFDT[i]->ptrinode->FileSize = MAXFILESIZE;
    uareaobj.UFDT[i]->ptrinode->ActualFileSize = 0;
    uareaobj.UFDT[i]->ptrinode->FileType = REGULARFILE;
    uareaobj.UFDT[i]->ptrinode->ReferenceCount = 1;
    uareaobj.UFDT[i]->ptrinode->Permission = permission;

    // Allocate memory for file data
    uareaobj.UFDT[i]->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);

    superobj.FreeInodes--;

    return i;       //File Descriptor
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
    int iRet = 0;

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

        printf("\nMarvellous CVFS : > ");  
                                    // Displays the command prompt for user interaction

        fgets(str, sizeof(str), stdin);
                                    // Reads the complete command entered by the user
                                    // including spaces, until a newline is encountered
                                    
        iCount = sscanf(str,"%s %s %s %s %s", Command[0],Command[1], Command[2], Command[3], Command[4]);

        fflush(stdin);

        if(iCount == 1)
        {
            //Marvellous CVFS : > exit
            if(strcmp("exit",Command[0]) == 0)
            {
                printf("Thank you for using Marvellous CVFS\n");
                printf("Deallocating all the allocated resources");

                break;
            }
            //Marvellous CVFS : > ls
            else if (strcmp("ls",Command[0]) == 0)
            {
                printf("Inside ls\n");
            }
            //Marvellous CVFS : > help
            else if(strcmp("help",Command[0]) == 0)
            {
                DisplayHelp();
            }
            //Marvellous CVFS : > clear
            else if(strcmp("clear",Command[0]) == 0)
            {
                #ifdef _WIN32                   //for windows
                    system("cls");
                #else
                    system("clear");           //for linux based OS
                #endif
            }
            
        }   //End of else if 1
        else if(iCount == 2)
        {
            //Marvellous CVFS : > man ls
            if(strcmp("man",Command[0]) == 0)
            {
                ManPageDisplay(Command[1]);
            }
        }   //End of else if 2
        else if(iCount == 3)
        {
            //Marvellous CVFS : > creat Ganesh.txt 3
            if(strcmp("creat",Command[0]) == 0)
            {
                iRet = CreateFile(Command[1], atoi(Command[2]));

                if(iRet == ERR_INVALID_PARAMETER)
                {
                    printf("Error : Unable to create file as parameters are invalid\n");
                    printf("Please refer man page\n");
                }

                if(iRet == ERR_NO_INODES)
                {
                    printf("Error : Unable to create file as there is NO inode\n");
                }

                if(iRet == ERR_FILE_ALREADY_EXIST)
                {
                    printf("Error : Unable to create the file because the file is already present\n");
                }

                if(iRet == ERR_MAX_FILES_OPEN)
                {
                    printf("Error : Unable to create file\n");
                    printf("Max Opened files limit reached\n");
                }

                printf("File gets succesfully created with FD %d\n",iRet);
            }
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
}   // End of main
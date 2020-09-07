/*
 *  Virtual File System
 *  Author: benjaSantana
 *  Date: 28/05/2020
 *
 */


#include "managerStrings.h"

#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAXSIZEOFCOMMAND 50
#define VOLUMESIZE 512
#define BLOCKSIZE 4096
#define MILLION 1000000
#define MAXNAMELEN 20
#define TABLE_SIZE 125000

typedef struct FileControlBlock{

    char date[25];
    size_t size;
    size_t firstBlock;

}FCB;

typedef struct DirectoryTuple{

    char *fileNameInDir;
    size_t posFCB;


}DirTuple;

typedef struct MemBlock{

    char *name;
    int size;
    int numBlocks;
    char *ActualDisk;

    int dirBlock;

    DirTuple dirTuples[128];
    int currDirTuples;

}MemBlock;

void createCommand(char*, MemBlock*);
void unmountFileSystem(char*, MemBlock*);
void mountFileSystem(char*, MemBlock*);
void loadFile(char*, MemBlock*);
void downloadFile(char*, MemBlock*);
char* makeDirTuplesStrings(DirTuple);
char* luToString(size_t);
char* castFCBtoString(FCB);
void stringToDirTuples(char*, DirTuple*);
FCB stringToFCB(char*);
void printDetails(char*, MemBlock*);
void listFiles(MemBlock*);
void infoSystem(MemBlock*);
unsigned long hash(const char*);


int main() {

    int menu = 1;

    MemBlock *memBlock = malloc(sizeof(MemBlock));
    char *input;


    while(menu == 1) {


        printf("\nfoo@bar~:$ ");

        input = malloc(MAXSIZEOFCOMMAND);
        fgets(input, MAXSIZEOFCOMMAND, stdin);

        size_t leny = strlen(input);

        stripNewLine(input, leny);


        char *command = getCommand(input, leny);

        menu = 0;


        if (strcmp(command, "create") == 0) {


            if (strlen(input) > 12) {

                createCommand(input, memBlock);

            } else {

                printf("\n\tLacking arguments"
                       "\n\tTry: 'create name 512 4'\n");

            }


            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "unmount") == 0) {


            if (strlen(input) > 8) {

                unmountFileSystem(input, memBlock);

            } else {

                printf("\n\tLacking arguments"
                       "\n\tTry: 'unmount name'\n");

            }

            memBlock->numBlocks = 0;
            memBlock->name[0] = '\0';
            memBlock->size = 0;
            memBlock->ActualDisk = NULL;

            free(memBlock);

            free(input);
            input = NULL;
            free(command);
            command = NULL;

            memBlock = malloc(sizeof(MemBlock));

            menu = 1;

        } else if (strcmp(command, "mount") == 0) {

            if (strlen(input) > 6) {

                mountFileSystem(input, memBlock);

            } else {

                printf("\n\tWrong input on command"
                       "\n\tTry: 'mount name'\n");

            }


            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "upload") == 0) {

            loadFile(input, memBlock);

            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "download") == 0 ) {

            downloadFile(input, memBlock);

            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "details") == 0) {

            printDetails(input, memBlock);
            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;


        } else if (strcmp(command, "ls") == 0) {


            listFiles(memBlock);
            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "info") == 0) {

            infoSystem(memBlock);
            free(input);
            input = NULL;
            free(command);
            command = NULL;

            menu = 1;

        } else if (strcmp(command, "exit") == 0) {

            return 0;

        } else {

            printf("\n\tCommand not Found!\n");
            free(input);
            input = NULL;
            free(command);
            command = NULL;
            menu = 1;

        }


    }
        free(memBlock);
        return 0;



}

void createCommand(char* input, MemBlock* memBlock){


    int i;

    //-------READ COMMAND------------------------------------------------------

    int inputLen = strlen(input);

    char *inputs[3];


    /*
     * First we have to decompose the input into parts,
     * input[0] name of disk
     * input[1] size
     * input[2] size of partitions
     *
     */
    int j = 0;
    int lastIndex = 6;

    //We start in 6 because we skip 'create'
    for(i=lastIndex; i<inputLen; i++){

        if(input[i] == ' ' && i != 6){ //Every time there's an ' ' we store the new word.

            inputs[j] = malloc(MAXNAMELEN*sizeof(char));
            memcpy(inputs[j], &input[lastIndex+1], i-lastIndex-1);
            j++;
            lastIndex = i;
        }

    }

    inputs[2] = malloc(5*sizeof(char ));
    memcpy(inputs[2], &input[lastIndex+1], inputLen-lastIndex);


    memBlock->name = malloc(sizeof(inputs[0]));
    strcpy(memBlock->name, inputs[0]);
    stripNewLine(memBlock->name,strlen(memBlock->name));

    char *trash;

    memBlock->size = strtol(inputs[1],&trash, 10);
    memBlock->numBlocks = strtol(inputs[2],&trash, 10);


    //-----------------------------------------------------------------------

    // The size is fixed, so we don't care what the user inputs
    memBlock->size = VOLUMESIZE;
    memBlock->numBlocks = BLOCKSIZE;

    //Allocate memory for the disk and fill it with -1s
    memBlock->ActualDisk = malloc(VOLUMESIZE*sizeof(char)*MILLION);
    memset(memBlock->ActualDisk,-1,VOLUMESIZE*sizeof(char)*MILLION);

    //INODE
    char inode[] = "FileSystemValid.";
    size_t lenInode = strlen(inode);
    memcpy(memBlock->ActualDisk, &inode[0], lenInode);

    //SUPERBLOCK

    char *metadata = malloc(memBlock->numBlocks*sizeof(char));
    char numSize[8];
    char numBlocks[8];

    strcat(metadata, "dirb:2."); //Save the Block of the Directory

    strcat(metadata, memBlock->name); //Save the name to the SuperBlock
    strcat(metadata, ".");

    sprintf(numSize,"%d",memBlock->size); //Save the Size
    strcat(metadata, numSize);
    strcat(metadata, ".");

    sprintf(numBlocks,"%d",memBlock->numBlocks); //Save the Size of Blocks
    strcat(metadata, numBlocks);
    strcat(metadata, ".");

    memBlock->currDirTuples = 0;



    int lenMetadata = strlen(metadata);
    memcpy(&memBlock->ActualDisk[lenInode], &metadata[0], lenMetadata);



    printf("\n\tVirtual System Created Successfully!\n");
    printf("\tThe size of both partitions and volumes are prefixed\n");
    printf("\t512MB and 4K respectively.\n");
    printf("\tNOTE: Be sure to use the command 'unmount' to save it\n\n");

    free(*inputs);

}

void unmountFileSystem(char* input,MemBlock* memBlock) {

    FILE *pFile;

    char *fileName = malloc(20 * sizeof(char));
    size_t lenInput = strlen(input);
    memcpy(fileName, &input[8], lenInput - 7);

    if (strcmp(fileName, memBlock->name) != 0) {

        printf("\n\tNo such File System Mounted!\n");
        return;

    }


    strcat(fileName, ".fs");

    pFile = fopen(fileName, "wb+");

    if(pFile==NULL){

        printf("\n\tFile System ERRROR :(\n");
        return;
    }

    fwrite(memBlock->ActualDisk, strlen(memBlock->ActualDisk),1, pFile);

    free(fileName);
    fileName = NULL;
    input = NULL;

    fclose(pFile);


    printf("\n\tThe File System '%s' has been successfully saved", memBlock->name);
    printf("\n\tTo open it use the 'mount' command, followed by the File System name.");
    printf("\n\tNOTE: Don't use any file extension\n\n");

}

void mountFileSystem(char* input, MemBlock* memBlock) {

    int i;

    char *fileName = malloc(MAXNAMELEN * sizeof(char));
    int lenInput = strlen(input);
    memcpy(fileName, &input[6], lenInput - 6);

    memBlock->name = malloc(MAXNAMELEN*sizeof(char));
    stripNewLine(memBlock->name,MAXNAMELEN);
    strcpy(memBlock->name, fileName);

    strcat(fileName, ".fs");

    FILE *pFile;
    pFile = fopen(fileName, "rb");
    rewind(pFile);
    if(pFile==NULL){

        printf("\n\tFile System not found :(\n");
        return;
    }

    char inode[16]; //16 is the num of characters on the INODE
    char c;
    for(i=0; i<16;i++){

        c = fgetc(pFile);
        inode[i] = c;

    }

    inode[i] = '\0';

    if(strcmp(inode, "FileSystemValid.")!=0){

        printf("\tNot a valid File System!!\n");

    }else{

        printf("\n\tFile System Found\n");

    }


    memBlock->size = VOLUMESIZE;
    memBlock->numBlocks = BLOCKSIZE;
    memBlock->ActualDisk = malloc(VOLUMESIZE*sizeof(char)*MILLION);


    rewind(pFile);

    fread(memBlock->ActualDisk, VOLUMESIZE*sizeof(char)*MILLION, 1, pFile);

    //Get the Directory Block
    memBlock->dirBlock = memBlock->ActualDisk[21] - '0';

    //Get how many files are in the disk
    char *temp;
    temp = calloc(10, sizeof(char));
    memcpy(temp, &memBlock->ActualDisk[memBlock->dirBlock*BLOCKSIZE], 6);

    char *buffer = stringUntil(temp, strlen(temp),'.');
    temp=NULL;
    memBlock->currDirTuples= (int)strtol(buffer,&temp, 10);

    //Get the Directory tuples
    int k = 1;
    for(i=0; i<memBlock->currDirTuples; i++) {
        char *buffDir = calloc(33, sizeof(char ));

        int index = (memBlock->dirBlock * BLOCKSIZE) + (32 * k);
        memcpy(buffDir,
                &memBlock->ActualDisk[index],
                31);

        memBlock->dirTuples[i].fileNameInDir = malloc(24*sizeof(char ));
        stringToDirTuples(buffDir, &memBlock->dirTuples[i]);

        memBlock->dirTuples[i].posFCB = hash(memBlock->dirTuples[i].fileNameInDir)*BLOCKSIZE;

        k++;
        free(buffDir);
        buffDir = NULL;
    }


    printf("\n\t----------------------------------------\n");
    printf("\tFile System Name: %s\n", memBlock->name);
    printf("\tSize: %lu\n", strlen(memBlock->ActualDisk));
    printf("\tBlock Size: %d\n", memBlock->numBlocks);
    printf("\tDir is in block: %d\n", memBlock->dirBlock);
    printf("\tFiles in System: %d\n", memBlock->currDirTuples);
    printf("\t----------------------------------------\n");

    fclose(pFile);
    free(fileName);
    fileName = NULL;

}

void loadFile(char* input, MemBlock* memBlock){

    int i = 0;

    char *fileName = malloc(20*sizeof(char));
    size_t lenInput = strlen(input);
    memcpy(fileName, &input[7],lenInput-6);


    FILE *pFile;

    pFile = fopen(fileName, "rb");

    if(pFile==NULL){

        printf("\n\tFile not found\n");
        return;
    }


    //--------------------METADATA-----------------------------------------------------------------------

    //Search an Empty tuple in the Directory

    for (i = 0; i < memBlock->currDirTuples; i++) {

        if (memBlock->dirTuples[i].fileNameInDir[0] == '\0') {

            break;

        }

    }
    memBlock->dirTuples[i].fileNameInDir = malloc(MAXNAMELEN*sizeof(char));
    strcpy(memBlock->dirTuples[i].fileNameInDir, fileName);
    memBlock->dirTuples[i].posFCB = hash(fileName)*BLOCKSIZE;

    //Cast Tuple to String in order to store it in the ActualDisk
    char dirTuplesStr[33];

    strcpy(dirTuplesStr, makeDirTuplesStrings(memBlock->dirTuples[i]));


    dirTuplesStr[31] = '\0';

    //Block of the Dir + 32*(How many tuples already are + the 32 chars where we store how many files we've got)
    size_t dirIndex = memBlock->dirBlock*BLOCKSIZE+32*(memBlock->currDirTuples+1);

    //Allocate the Directory in the Disk
    memcpy(&memBlock->ActualDisk[dirIndex], //Block Position
           dirTuplesStr,
            strlen(dirTuplesStr)); //AKA 32

    printf("\n\tDir Index: %lu", dirIndex);

    //Make the FCB for the File
    FCB thisFileFCB;
    time_t now;
    time(&now);

    size_t fileSize = fsize(fileName);


    strcpy(thisFileFCB.date, ctime(&now));
    stripNewLine(thisFileFCB.date, strlen(thisFileFCB.date));
    thisFileFCB.size = fileSize;

    thisFileFCB.firstBlock = hash(luToString(memBlock->dirTuples[i].posFCB))*BLOCKSIZE;


    char *FCBString = castFCBtoString(thisFileFCB);

    //Allocate the FCB in the Disk
    memcpy(&memBlock->ActualDisk[memBlock->dirTuples[i].posFCB], //Position given by hash
          FCBString,
            strlen(FCBString));


    //Add one to the directory tuples
    memBlock->currDirTuples += 1;
    char* numOfDirTuples = luToString(memBlock->currDirTuples); //MAYBE add '\0' at the end
    strcat(numOfDirTuples,".");
    //Update the number of files in the dir
    memcpy(&memBlock->ActualDisk[memBlock->dirBlock*BLOCKSIZE], //Block Position 4096*2
           numOfDirTuples, //How many tuples are in the directory
           strlen(numOfDirTuples));


    printf("\n\tFile Name: %s", fileName);
    printf("\n\tDate: %s", thisFileFCB.date);
    printf("\n\tDisk Size: %lu", strlen(memBlock->ActualDisk));
    printf("\n\tFile Size: %lu", fileSize);
    printf("\n\tFiles in Dir: %s\n", numOfDirTuples);
    printf("\n\tPosition First Block: %lu\n", thisFileFCB.firstBlock);

    //----------------------------------------------------------------------------------------------

    //Store file into a char*
    char *actualFile = malloc(fileSize +1);
    fread(actualFile, fileSize, 1, pFile);

    int blocksNeeded = (int)fileSize/BLOCKSIZE;

    if(blocksNeeded <= 1){

        size_t fcbposition = memBlock->dirTuples[i].posFCB;

        size_t blockPosition = thisFileFCB.firstBlock;
        memcpy(&memBlock->ActualDisk[blockPosition],&actualFile[0], fileSize);

        memBlock->ActualDisk[blockPosition+fileSize+1] = '.';

    }else{

        int w;

        size_t fcbposition = memBlock->dirTuples[i].posFCB;

        size_t blockPosition = thisFileFCB.firstBlock;

        for(w=0;w<blocksNeeded; w++){

            memcpy(&memBlock->ActualDisk[blockPosition],&actualFile[w*BLOCKSIZE], BLOCKSIZE);
            blockPosition = hash(luToString(blockPosition))*BLOCKSIZE;

        }


    }

    fclose(pFile);

}

void downloadFile(char* input, MemBlock* memBlock){

    int i = 0;

    char *fileName = malloc(30*sizeof(char));
    size_t lenInput = strlen(input);
    memcpy(fileName, &input[9],lenInput-8);

    printf("\n\tLooking for: %s\n", fileName);

    printf("\t\t");
    bool founded = false;
    for(i=0; i<memBlock->currDirTuples; i++){

        if(memBlock->dirTuples[i].fileNameInDir[0] != '\0') {
            if (strcmp(memBlock->dirTuples[i].fileNameInDir, fileName) == 0) {

                printf("\n\tFile Found!");
                founded = true;
                break;

            }
        }
        printf(".");
    }

    if(!founded){

        printf("\n\tFile not found! Check the name for typos :(\n");
        return;

    }

    FCB fcbFile;
    size_t fcbPosition;


    fcbPosition = memBlock->dirTuples[i].posFCB;
    char buffer[50];

    memcpy(buffer, &memBlock->ActualDisk[fcbPosition], 45);

    size_t lenBuffer = strlen(buffer);

    buffer[lenBuffer] = '\0';

    fcbFile = stringToFCB(buffer);


    fcbFile.firstBlock = hash(luToString(fcbPosition))*BLOCKSIZE;

    printf("\n\tUploaded on: %s", fcbFile.date);
    printf("\n\tSize: %lu bytes", fcbFile.size);
    printf("\n\tFirst block: %lu", fcbFile.firstBlock);
    printf("\n\tDisk Length: %lu\n", strlen(memBlock->ActualDisk));


    //Create new File
    FILE *pFile;
    pFile = fopen(fileName, "wb+");

    //Store file into a char*
    char *actualFile = malloc(fcbFile.size +1);
    fread(actualFile, fcbFile.size, 1, pFile);

    int blocksNeeded = (int)fcbFile.size/BLOCKSIZE;

    int remainginPart;
    if(blocksNeeded>0) {
        remainginPart = (int) fcbFile.size % blocksNeeded; //Get the part that's on the other block but not complete
    }else{

        remainginPart = (int) fcbFile.size;
    }


    int k=0;
    size_t fcbFirstPos = fcbFile.firstBlock;

    for(k=0; k<blocksNeeded; k++){


        memcpy(&actualFile[k*BLOCKSIZE], &memBlock->ActualDisk[fcbFirstPos], BLOCKSIZE);
        fcbFirstPos = hash(luToString(fcbFirstPos))*BLOCKSIZE;

    }

    if(remainginPart>0){

        memcpy(&actualFile[k*BLOCKSIZE], &memBlock->ActualDisk[fcbFirstPos], remainginPart);

    }

    fwrite(actualFile, fcbFile.size, 1, pFile);

    fclose(pFile);

}

char* makeDirTuplesStrings(DirTuple dirTuple){

    char *buffer;
    buffer = malloc(32*sizeof(char));
    strcpy(buffer, dirTuple.fileNameInDir);

    char *longNumber = luToString(dirTuple.posFCB);
    int i;
    for(i=(int)strlen(dirTuple.fileNameInDir); i<31-(int)strlen(longNumber); i++){

        buffer[i] = '-';

    }

    buffer[i] = '\0';
    strcat(buffer, longNumber);

    return buffer;


}

char* castFCBtoString(FCB fileControl){

    char *buffer;
    buffer = malloc(300*sizeof(char));

    strcpy(buffer, fileControl.date);

    char *fileSize = luToString(fileControl.size);

    strcat(buffer, ".");

    strcat(buffer, fileSize);

    strcat(buffer, ".");

    char *firstBlock = luToString(fileControl.firstBlock);

    strcat(buffer, firstBlock);

    return buffer;



}

char* luToString(size_t longNumber){

    char blu[21];
    sprintf(blu,"%lu", longNumber);
    char *bufferstr = calloc(strlen(blu), sizeof(char));
    strcpy(bufferstr, blu);

    return bufferstr;

}

void stringToDirTuples(char* tuple, DirTuple *dirTuple){

    int i;
    //DirTuple dirTuple;

    char *bufferDate = stringUntil(tuple, 26, '-');

    strcpy(dirTuple->fileNameInDir,bufferDate);

    char *bufferPos = malloc(12*sizeof(char ));
    int j= 0;
    for(i=30; i>22; i--){

        bufferPos[j] = tuple[i];
        j++;
    }

    char *temporal = stringUntil(bufferPos, 9, '-' );

    char *numberFinal = malloc(strlen(temporal)*sizeof(char ));
    j=0;
    for(i=(int)strlen(temporal)-1; i>-1; i--){

        numberFinal[j] = temporal[i];
        j++;
    }
    bufferPos = NULL;
    size_t temp = strtol(numberFinal,&bufferPos, 10);
    dirTuple->posFCB = temp;




}

FCB stringToFCB(char* tuple) {

    int i;
    FCB fileControl;

    size_t lenTuple = strlen(tuple);

    strcpy(fileControl.date, stringUntil(tuple, lenTuple, '.'));

    fileControl.firstBlock = 10;

    char bufSize[12];
    int j=0;
    for(i=25; i<36;i++){

        bufSize[j] = tuple[i];
        j++;
    }

    char *temp = stringUntil(bufSize,10, '.');

    char *trash;

    fileControl.size = strtol(temp, &trash, 10);

    return fileControl;
}

void printDetails(char* input, MemBlock* memBlock){

    int i = 0;
    if(memBlock->size == 0){

        printf("\n\tNO DISK MOUNTED\n");
        return;
    }

    char *fileName = malloc(30*sizeof(char));
    size_t lenInput = strlen(input);
    memcpy(fileName, &input[8],lenInput-7);

    printf("\n\tLooking for: %s\n", fileName);

    printf("\t\t");
    bool founded = false;
    for(i=0; i<memBlock->currDirTuples; i++){

        if(memBlock->dirTuples[i].fileNameInDir[0] != '\0') { /*BE CAREFUL WHEN WE REMOVE FILES*/
            if (strcmp(memBlock->dirTuples[i].fileNameInDir, fileName) == 0) {

                printf("\n\tFile Found!");
                founded = true;
                break;

            }
        }
        printf(".");
    }

    if(!founded){

        printf("\n\tFile not found! Check the name for typos :(\n");
        return;

    }

    FCB fcbFile;
    size_t fcbPosition;


    fcbPosition = memBlock->dirTuples[i].posFCB;
    char buffer[50];

    memcpy(buffer, &memBlock->ActualDisk[fcbPosition], 45);

    size_t lenBuffer = strlen(buffer);

    buffer[lenBuffer] = '\0';

    fcbFile = stringToFCB(buffer);


    fcbFile.firstBlock = hash(luToString(fcbPosition))*BLOCKSIZE;

    printf("\n\tUploaded on: %s", fcbFile.date);
    printf("\n\tSize: %lu bytes", fcbFile.size);
    printf("\n\tFirst block: %lu", fcbFile.firstBlock);
    printf("\n\tDisk Size: %lu\n", strlen(memBlock->ActualDisk));

    printf("\n\tList of blocks:\n");


    int blocksNeeded = (int)fcbFile.size/BLOCKSIZE;

    int remainginPart;
    if(blocksNeeded>0) {
        remainginPart = (int) fcbFile.size % blocksNeeded; //Get the part that's on the other block but not complete
    }else{

        remainginPart = (int) fcbFile.size;
    }


    int k=0;
    int j=0;

    int hashIndex;

    size_t fcbFirstPos = fcbFile.firstBlock;

    for(k=0; k<blocksNeeded; k++){

        printf("\n\tBlock Number: %d, Position: %lu", k, fcbFirstPos);
        fcbFirstPos = hash(luToString(fcbFirstPos))*BLOCKSIZE;
    }

    if(remainginPart>0){

        printf("\n\tBlock Number: %d, Position: %lu", k, fcbFirstPos);

    }

    printf("\n");

}

void listFiles(MemBlock* memBlock){
    int i;
    if(memBlock->size == 0){

        printf("\n\tNO DISK MOUNTED\n");
        return;
    }
    printf("\n");
    for(i=0; i<memBlock->currDirTuples; i++){

        printf("\t%s\n", memBlock->dirTuples[i].fileNameInDir);

    }

    printf("\n");
}

void infoSystem(MemBlock* memBlock){


    if(memBlock->size == 0){

        printf("\n\tNO DISK MOUNTED\n");
        return;
    }
    size_t usedSize=0;


    FCB fcbFile;
    size_t fcbPosition;




    int i;
    for(i = 0; i <memBlock->currDirTuples; i++) {


        fcbPosition = memBlock->dirTuples[i].posFCB;
        char buffer[50];

        memcpy(buffer, &memBlock->ActualDisk[fcbPosition], 45);

        size_t lenBuffer = strlen(buffer);

        buffer[lenBuffer] = '\0';

        fcbFile = stringToFCB(buffer);

        usedSize += fcbFile.size;

    }

    printf("\n\tFile System Name: %s", memBlock->name);
    printf("\n\tVolume size: %d", memBlock->size*MILLION);
    printf("\n\tUsed Size: %lu", usedSize);
    printf("\n\tRemaining Size: %lu\n", strlen(memBlock->ActualDisk)-usedSize);

}


unsigned long hash(const char *str ){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = hash * 33 + c;

    return hash%TABLE_SIZE;
}

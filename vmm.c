#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#define TLB_SIZE 16
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE PAGE_SIZE*FRAME_SIZE
int logicalAddress = 0;
int offsetNumber = 0;
int pageNumber = 0;
int physicalAddress = 0;
int Frame = 0;
int Value = 0;
int Hit = 0;
int tlbIndex = 0;
13
int tlbSize = 0;
unsigned pageNumberMask = 65280; //1111111100000000
unsigned offsetMask = 255; //11111111
int tlbHitCount = 0;
float tlbHitRate = 0;
int addressCount = 0;
int pageFaultCount = 0;
float pageFaultRate = 0;
struct tlbTable {
 unsigned int pageNum;
 unsigned int frameNum;
};
int main (int argc, char *argv[])
{
 //Open addresses.txt, BACKING_STORE.bin, and
 //Create Output.txt to store program results
 FILE *addresses = fopen("addresses.txt","r");
 FILE *BACKINGSTORE = fopen("BACKING_STORE.bin", "rb");
 FILE *Output = fopen("addressOutput.txt", "w");
 int physicalMemory[PHYSICAL_MEMORY_SIZE];
 char Buffer[256];
 int Index;
 //Declare and initialize pageTable[] array to -1
 int pageTable[PAGE_SIZE];
 memset(pageTable, -1, 256*sizeof(int));
 //Declare and initialize tlb[] structure to -1
 struct tlbTable tlb[TLB_SIZE];
 memset (pageTable, -1, 16*sizeof(char));
 //Read each address from addresses.txt
 while(fscanf(addresses, "%d", &logicalAddress) == 1)
 {
 addressCount++;
 //set the page number and offset for each logical address
 pageNumber = logicalAddress & pageNumberMask;
 pageNumber = pageNumber >> 8;
 offsetNumber = logicalAddress & offsetMask;
 Hit = -1;
 //Check to see if the page number is already in the tlb
14
 //If it is in tlb, then it is tlb hit
 for(Index = 0; Index < tlbSize; Index++)
 {
 if(tlb[Index].pageNum == pageNumber)
 {
 Hit = tlb[Index].frameNum;
 physicalAddress = Hit*256 + offsetNumber;
 }
 }
 if(!(Hit == -1))
 {
 tlbHitCount++;
 }
 //This "else if" loop is the tlb miss
 //Gets the physical page number from page table
 else if(pageTable[pageNumber] == -1)
 {
 fseek(BACKINGSTORE, pageNumber*256, SEEK_SET);
 fread(Buffer, sizeof(char), 256, BACKINGSTORE);
 pageTable[pageNumber] = Frame;
 for(Index = 0; Index < 256; Index++)
 {
 physicalMemory[Frame*256 + Index] = Buffer[Index];
 }
 pageFaultCount++;
 Frame++;
 //FIFO algorithm for the tlb
 if(tlbSize == 16)
 tlbSize--;
 for(tlbIndex = tlbSize; tlbIndex > 0; tlbIndex--)
 {
 tlb[tlbIndex].pageNum = tlb[tlbIndex-1].pageNum;
 tlb[tlbIndex].frameNum = tlb[tlbIndex-1].frameNum;
 }
 if (tlbSize <= 15)
 tlbSize++;
 tlb[0].pageNum = pageNumber;
 tlb[0].frameNum = pageTable[pageNumber];
 physicalAddress = pageTable[pageNumber]*256 + offsetNumber;
 }
 else
 {
 physicalAddress = pageTable[pageNumber]*256 + offsetNumber;
15
 }
 //Gets the value from the bin file provided
 Value = physicalMemory[physicalAddress];
 //print the addresses and value to Output.txt
 fprintf(Output, "Virtual Address: %d Physical Address: %d Value: %d \n", logicalAddress, 
physicalAddress, Value);
 }
 //The statistics of the program
 pageFaultRate = pageFaultCount*1.0f / addressCount;
 tlbHitRate = tlbHitCount*1.0f / addressCount;
//Close files provided for the project
fclose(addresses);
fclose(BACKINGSTORE);
//Print the statistics of the program to Output.txt
fprintf(Output, "Number of Addresses: %d\n", addressCount);
fprintf(Output, "Number of Page Faults: %d\n", pageFaultCount);
fprintf(Output, "Page Fault Rate: %f\n", pageFaultRate);
fprintf(Output, "TLB Hits: %d\n", tlbHitCount);
fprintf(Output, "TLB Hit Rate %f\n", tlbHitRate);
//Close Output.txt
fclose(Output);
return 0;
}

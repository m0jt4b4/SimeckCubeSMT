#include "stdafx.h"
#include "Helper.h"
#include <assert.h>
#include <strsafe.h>

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = (char **) malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
void ErrorHandler(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
void array_rnd_fill(int *CUBE,int SIZE,int start,int end)
   {  /* very lazy array filler */
      int i,j;
      for(i = 0; i < SIZE; i++) {
         do {
            CUBE[i]=rand_int(start,end);
            for(j = 0; j < i; j++) {
               if(CUBE[i] == CUBE[j]) break;
            }
         } while(i != j);
      }
   }
void index_rnd_fill(int *CUBE,int index,int SIZE,int start,int end)
   {  /* very lazy array filler */
      int j;
         if(index < SIZE) do {
            CUBE[index]=rand_int(start,end);
            for(j = 0; j < index; j++) {
               if(CUBE[index] == CUBE[j]) break;
            }
         } while(index != j);
         else printf("index exeeded");
   }
void long_to_bits64(uint64_t x)
{
	int i;
	for(i = 63; i>=0; i--)
		printf("%d",(int)((x>>i) & 1ull));
		printf("\n");
}
void long_to_bits16(uint16_t x)
{
	int i;
	for(i = 15; i>=0; i--)
		printf("%d",(int)((x>>i) & 1ull));
		printf("\n");
}
void long_to_bits(uint64_t x)
{
	int i;
	for(i = 0; i<64; i++)
		printf("%d",(int)((x>>i) & 1ull));
		printf("\n");
}
  uint64_t rand64(){
  uint64_t r0 = (uint64_t) (rand()) << 48;
  uint64_t r1 = (uint64_t) (rand()) << 32;
  uint64_t r2 = (uint64_t) (rand()) << 16;
  uint64_t r3 = (uint64_t) (rand());
  return r0 ^ r1 ^r2 ^ r3;
  } // end of rand64
  uint32_t rand32(){
  uint32_t r0 = (uint32_t) (rand()) << 24;
  uint32_t r1 = (uint32_t) (rand()) << 16;
  uint32_t r2 = (uint32_t) (rand()) << 8;
  uint32_t r3 = (uint32_t) (rand());
  return r0 ^ r1 ^r2 ^ r3;
  } // end of rand32
  uint16_t rand16(){
  uint16_t r0 = (uint16_t) (rand()) << 12;
  uint16_t r1 = (uint16_t) (rand()) << 8;
  uint16_t r2 = (uint16_t) (rand()) << 4;
  uint16_t r3 = (uint16_t) (rand());
  return r0 ^ r1 ^r2 ^ r3;
  }
  int rand_int(int a, int b)
  {
	  return a+((int)(rand64()%(b-a+1)));
  }

  int createLogFile(const char* path)
{

    FILE *file;
    int file_exists;

    file=fopen(path,"r");
    if (file==NULL) file_exists=0;
    else {file_exists=1; fclose(file);}
    
    if (file_exists==0)
	{
		file=fopen(path,"w+b");
		fclose(file);
	}

    return 0;
}
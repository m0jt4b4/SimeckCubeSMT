#include "stdafx.h"
#include "offline_full_search_32.h"
#include "simeck32.h"
#include "Helper.h"
#include <time.h>
#include <sstream>
#include <fstream>

using namespace std;

int cube_size = 1;
#define rnds 10

#define threads 4
#define number_of_tests 1024
#define per_thread number_of_tests/threads
int word_size = 16;
#define START 0
#define END 2*word_size-1
#define ciphervariant "32-64"
#define _lin_max_vars 100
#define _cube_max_size 30
int c_indx = 1;
int cnt = 0;
int best_cube = 0;
string s;

#define SSTR( x ) dynamic_cast< std::ostringstream & > ( ( std::ostringstream() << std::dec << x ) ).str()
const char * const_output_file_name;
const char * const_out_file_name;

uint64_t rand64();
uint32_t rand32();
uint16_t rand16();
void statistics(char *file_name);
void write(char *file_name,int *cube,int dim,int round);
void check_and_write(char *file_name,int *cube,int cube_dim,int round);
void write_statistics(char * file_name,int array[11][26],int cnt,double pnt);
void read(char *file_name);
int rand_int(int a, int b);
void array_rnd_fill(int *CUBE,int SIZE,int start,int end);
void r_index_rnd_fill(int *CUBE,int r_index,int SIZE,int start,int end);
void read(char *input_file);
int * write_long(uint64_t x);
uint64_t read_long(int *buf);
char** str_split(char* a_str, const char a_delim);
void write_maxterm(int size,int out,int rounds,bool is_const);
void write_to_file(int cntt);

DWORD WINAPI cthread_func(LPVOID lpParameter);
int c_degree = 0;
int c_result = 0;
typedef struct thread_data
{
	int c_result;
	int outbit;
	int rounds;
	int c_degree;
};

int constCube[32];

// TEXT AND KEY
uint16_t ctext32[2];
uint16_t ckey64[4];

void offline_full_search(int cs)
{
	cube_size = cs;
	string a = "simeck" ciphervariant "_offline_full_search_cubeSize_" + SSTR(cube_size) + "___c_result.txt";
	const_output_file_name = a.c_str();
	string b = "simeck" ciphervariant "_offline_full_search_cubeSize_" + SSTR(cube_size) + "___Maxterms.txt";
	const_out_file_name = b.c_str();

	srand(time(NULL));

	while(true)
	{
		s = "";
		cnt = 0;
		array_rnd_fill(constCube,cube_size,START,END);

		for(int i = 0; i < cube_size; i++) printf("%d ",constCube[i]);
		printf("\n");

		for(int rounds = 1; rounds <= rnds; rounds++)
		{
			printf("\tround %d:\n",rounds);

			for(int outbit = 0; outbit < 32; outbit++)
			{
				clock_t begin = clock();
				printf("\t\tout bit %d : ",outbit);
				HANDLE  thr[threads];
				thread_data thr_data[threads];
				
				int trial = 0;
				
				if(trial == 0)
				{
					c_degree = 0;

					for(int i = 0; i < threads; i++)
					{
						thr_data[i].c_degree = c_degree;
						thr_data[i].c_result = 0;
						thr_data[i].rounds = rounds;
						thr_data[i].outbit = outbit;
					}
					// start the threads
					for(int i = 0; i < threads; i++)
					{
						thr[i] = CreateThread(NULL, 0, cthread_func, &thr_data[i], 0, 0);
						
						if (thr[i] == NULL)
						{
							ErrorHandler(TEXT("CreateThreadError"));
							ExitProcess(3);
						}
					}
					
					// wait for all threads to finish
					WaitForMultipleObjects(threads, thr, TRUE, INFINITE);
					
					c_result = 0;
					
					for(int i = 0 ; i<threads; i++)
					{
						CloseHandle(thr[i]);
						c_result += thr_data[i].c_result;
					}
					if((c_result != 0 ) && (c_result != number_of_tests))
						trial = 1;
				}
				if(trial == 1)
				{
					c_degree = 1;
					for(int i = 0; i < threads; i++)
					{
						thr_data[i].c_degree = c_degree;
						thr_data[i].c_result = 0;
						thr_data[i].rounds = rounds;
						thr_data[i].outbit = outbit;
					}
					// start the threads
					for(int i = 0; i < threads; i++)
					{
						thr[i] = CreateThread(
							NULL,                   // default security attributes
							0,                      // use default stack size  
							cthread_func,       // thread function name
							&thr_data[i],          // argument to thread function 
							0,                      // use default creation flags 
							0);   // returns the thread identifier 
						
						if (thr[i] == NULL)
						{
							ErrorHandler(TEXT("CreateThreadError"));
							ExitProcess(3);
						}
					}
					// wait for all threads to finish
					WaitForMultipleObjects(threads, thr, TRUE, INFINITE);
					c_result = 0;
					for(int i = 0 ; i<threads; i++)
					{
						CloseHandle(thr[i]);
						c_result += thr_data[i].c_result;
					}
					if(c_result != 0)
						trial = 2;
				}
				if(trial == 2)
					c_degree = -1;
				
				clock_t end = clock();
				double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
				// c_degree output :: 0 : 1 : -1  ************************************************************
				
				if(c_degree == 0)
				{
					if(c_result == 0)
						printf("zero in %.1f s\n",elapsed_secs);
					else
						printf("one in %.1f s\n",elapsed_secs);
					write_maxterm(cube_size,outbit,rounds,true);
					cnt++;
				}
				else if(c_degree == 1)
				{
					printf("linear equation in %.1f s\n",elapsed_secs);
					write_maxterm(cube_size,outbit,rounds,false);
					cnt++;
				}else
					printf("higher c_degree in %.1f s\n",elapsed_secs);

				
			}
		}
		if(cnt > best_cube)
		{
			best_cube = cnt;
			write_to_file(best_cube);
		}
	}
} // end of main

DWORD WINAPI cthread_func(LPVOID lpParameter)
{
	thread_data *data = (thread_data*)lpParameter;
	
	// TEXT AND KEY
	uint16_t tmpctext32[2];
	uint16_t tmpckey64[4];
	
	// temp key 
	uint16_t k[2][4];

	uint64_t test_counter,sum,test_sum;
	sum = 0ull;
	test_sum = 0ull;
	data->c_result = 0;
	
	for(int loop = 0 ; loop < per_thread; loop++)
	{
		
		// create random key
		for(int l = 0; l < 2; l++)
			for(int y = 0; y < 4; y++)
				k[l][y] = rand16();
		
		test_sum = 0ull;
		// do test of c_degree k
		if(data->c_degree == 0)
		{
			memset(tmpckey64,0ull,sizeof tmpckey64);
			for(int y = 0; y < 4; y++)
				tmpckey64[y] = k[0][y];
			sum = 0ull;
			for(uint64_t i = 0; i < (1 << cube_size); i++)
			{
				memset(tmpctext32,0ull,sizeof tmpctext32);
				for(int j = 0; j < cube_size; j++)
				{
					((uint32_t *)tmpctext32)[0] ^= ((i>>j) & 1ull) << (constCube[j]);
				}
				
								
				simeck_32_64(tmpckey64, tmpctext32, tmpctext32,data->rounds);
				if (((uint64_t*)tmpctext32)[0] & (1ull << data->outbit))
					sum ^= 1ull;
			}
			if(sum)
				data->c_result++;
		}
		else if (data->c_degree == 1)
		{
			for(test_counter = 0ull; test_counter < (1ull << 2); test_counter++)
			{
				memset(tmpckey64,0ull,sizeof tmpckey64);
				for(int j = 0; j < 2; j++)
				{
					if((test_counter>>j) & 1ull)
						for(int y = 0; y < 4; y++)
							tmpckey64[y] ^= k[j][y];
				}
				// calculate one test instance ( entire cube )
				// set cube variables to all possible values, calculate grain-128a and sum
				sum = 0ull;
				for(uint64_t i = 0; i < (1 << cube_size); i++)
				{
					memset(tmpctext32,0ull,sizeof(tmpctext32));
					for(int j = 0; j < cube_size; j++)
					{
						((uint32_t *)tmpctext32)[0] ^= ((i>>j) & 1ull) << (constCube[j]);
					}
					

					simeck_32_64(tmpckey64, tmpctext32, tmpctext32,data->rounds);
					
					if (((uint64_t*)tmpctext32)[0] & (1ull << data->outbit))
						sum ^= 1ull;
				}
				test_sum ^= sum;
			}
			if(test_sum)
				data->c_result++;
			}
	}
	return 0;
  } // end of *thr_func

void write_maxterm(int size,int out,int rounds,bool is_const)
{
	int counter = 0;
	uint64_t sum,sum_const,test_sum;
	
	sum_const = 0ull;
	
	if(is_const)
	{
		for(uint64_t i = 0; i < (1<<size); i++)
		{
		memset(ckey64,0ull,sizeof ckey64);
		memset(ctext32,0ull,sizeof(ctext32));
		for(int j = 0; j < size; j++)
		{
			((uint32_t *)ctext32)[0] ^= ((i>>j) & 1ull) << (constCube[j]);
		}
		 
		simeck_32_64(ckey64, ctext32, ctext32,rounds);
		
		if (((uint64_t*)ctext32)[0] & (1ull << out))
			sum_const^= 1ull;
		}
		if(sum_const)
		{
			if(out < 16)
				s += "\ns.assert((left[" + SSTR(rounds) + "] & " + SSTR((int)(1 << out)) + ") == " + SSTR((int)(1 << out)) + " )";
			else
				s += "\ns.assert((right[" + SSTR(rounds) + "] & " + SSTR((int)(1 << (out-16))) + ") == " + SSTR((int)(1 << (out-16))) + " )";
			counter++;
		}else
		{
			if(out < 16)
				s += "\ns.assert((left[" + SSTR(rounds) + "] & " + SSTR((int)(1 << out)) + ") == " + SSTR(0) + " )";
			else
				s += "\ns.assert((right[" + SSTR(rounds) + "] & " + SSTR((int)(1 << (out-16))) + ") == " + SSTR(0) + " )";
			counter++;
		}
	}
	else
	{
		if(out < 16)
			s += "\ns.assert((left[" + SSTR(rounds) + "] & " + SSTR((int)(1 << out)) + ") == (";
		else
			s += "\ns.assert((right[" + SSTR(rounds) + "] & " + SSTR((int)(1 << (out-16))) + ") == (";

		for(uint64_t i = 0; i < (1<<size); i++)
		{
			memset(ckey64,0ull,sizeof ckey64);
			memset(ctext32,0ull,sizeof(ctext32));
			for(int j = 0; j < size; j++)
			{
				((uint32_t *)ctext32)[0] ^= ((i>>j) & 1ull) << (constCube[j]);
			}
		 
			simeck_32_64(ckey64, ctext32, ctext32,rounds);
		
			if (((uint64_t*)ctext32)[0] & (1ull << out))
						sum_const^= 1ull;
		}
		if(sum_const)
		{
			if(out < 16)
				s += SSTR((int)(1 << out));
			else
				s += SSTR((int)(1 << (out-16)));
			counter++;
		}else
		{
			s += "0";
			counter++;
		}

		int k_r_index;
		for(k_r_index = 0; k_r_index<64; k_r_index++)
		{
			sum = 0ull;
			memset(ckey64,0ull,sizeof ckey64);
		
			((uint64_t *)ckey64)[0] = 1ull << k_r_index;

			for(uint64_t i = 0; i < (1<<size); i++)
			{
				memset(ctext32,0ull,sizeof(ctext32));
				for(int j = 0; j < size; j++)
				{
					((uint32_t *)ctext32)[0] ^= ((i>>j) & 1ull) << (constCube[j]);
				}
			 
				simeck_32_64(ckey64, ctext32, ctext32,rounds);
			
					if (((uint64_t*)ctext32)[0] & (1ull << out))
						sum ^= 1ull;
				}
				if(sum ^ sum_const)
				{
					if(k_r_index < 16)
						s += " ^ (key[" + SSTR(0) + "] & " + SSTR((1 << k_r_index)) + ")";
					else if(k_r_index < 32)
						s += " ^ (key[" + SSTR(1) + "] & " + SSTR((1 << (k_r_index-16))) + ")";
					else if(k_r_index < 48)
						s += " ^ (key[" + SSTR(2) + "] & " + SSTR((1 << (k_r_index-32))) + ")";
					else
						s += " ^ (key[" + SSTR(3) + "] & " + SSTR((1 << (k_r_index-48))) + ")";
					counter++;
				}
			}
		s += ")";
	}
  } 

void write_to_file(int cntt)
{
	s += "\n";
	ofstream myfile;
	string a = "simeck" ciphervariant "_full_cube_search___Size_" + SSTR(cube_size) + "_SMT.txt";
	const char * filename = a.c_str();
	myfile.open(filename,std::ios_base::trunc);
	myfile << "index : " << c_indx << "\tcount : " << cntt << "\tDim : " << cube_size << "\tCube indexs : ";
	for(int i = 0; i < cube_size; i++) myfile << constCube[i] << ",";
	myfile << "\n" << s;
	myfile.close();
	c_indx++;
}

void statistics(char *file_name)
{
	FILE *file;
	file = fopen(file_name,"a+");

	if ( file != NULL )
   {
      char line [ 512 ]; /* or other suitable maximum line size */
      while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
      {
         fputs ( line, stdout ); /* write the line */
      }
      fclose ( file );
   }
   else
   {
      perror ( file_name ); /* why didn't the file open? */
   }
}
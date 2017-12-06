#include "stdafx.h"
#include "offline_search_32.h"
#include "Helper.h"
#include "simeck32.h"
#include <time.h>
#include <sstream>
#include <fstream>

using namespace std;

#define threads 8
#define number_of_tests 128
#define per_thread number_of_tests/threads
int w_size = 16;
int minCubeSizeLIMIT;
#define START 0
#define END 2*w_size-1
int cubeSizeLIMIT;
#define ciphervariant "32-64"
#define degLIMIT 1
#define searchLIMIT 15
#define SL "15"
#define _lin_max_vars 64
#define _cube_max_size 31
int indx = 1;
int out = 0;
uint64_t outbit = 1ull << out;

#define SSTR( x ) dynamic_cast< std::ostringstream & > ( ( std::ostringstream() << std::dec << x ) ).str()
const char * output_file_name;
const char * out_file_name;

void write(const char *file_name,int *cube,int dim,int round);
void check_and_write( const char *file_name,int *cube,int cube_dim,int round);
int * write_long(uint64_t x);
bool write_maxterm(int size,int out, int rnds);

DWORD WINAPI thread_func(LPVOID lpParameter);

int result = 0;
int degree = 0;
int dimension = 0;

typedef struct thread_data
{
	int res;
	int rnds;
};

int cube[32];

// TEXT AND KEY
uint16_t _text32[2];
uint16_t _key64[4];

void offline_search(int rnds)
{
	if(rnds <= 7)
	{
		minCubeSizeLIMIT = 1;
		cubeSizeLIMIT = 15;
	}
	else if(rnds == 8)
	{
		minCubeSizeLIMIT = 7;
		cubeSizeLIMIT = 18;
	}
	else if(rnds == 9)
	{
		minCubeSizeLIMIT = 10;
		cubeSizeLIMIT = 20;
	}
	else if(rnds == 10)
	{
		minCubeSizeLIMIT = 13;
		cubeSizeLIMIT = 27;
	}
	else
	{
		minCubeSizeLIMIT = 25;
		cubeSizeLIMIT = 31;
	}

	string a = "Simeck" ciphervariant "_offline_search_minCubeSize_" + SSTR(minCubeSizeLIMIT) + "__cubeSizeLIMIT_" + SSTR(cubeSizeLIMIT) + "__searchLIMIT_" SL "_inRound_" + SSTR(rnds) +  "__result.txt";
	output_file_name = a.c_str();
	string b = "Simeck" ciphervariant "_offline_search_minCubeSize_" + SSTR(minCubeSizeLIMIT) + "__cubeSizeLIMIT_" + SSTR(cubeSizeLIMIT) + "__searchLIMIT_" SL "_inRound_" + SSTR(rnds) + "__Maxterms.txt";
	out_file_name = b.c_str();

	createLogFile(output_file_name);
	createLogFile(out_file_name);

	srand(time(NULL));

	HANDLE  thr[threads];
	thread_data thr_data[threads];

	int lim = 1;
	int ii = rand_int(minCubeSizeLIMIT,cubeSizeLIMIT); //startCubeSize
	out = rand_int(START,END);
	outbit = 1ull<<out;
	array_rnd_fill(cube,ii,START,END); //initialize the cube
	printf("\nEvaluating cube with outbit %d: ",out);
	for(int i = 0; i < ii; i++) printf("%d ",cube[i]);

	int trial;

	while(true)
	{
		
		// test the constantness
		trial = 0;
		dimension = ii;
		degree = 0;

		for(int i = 0; i < threads; i++)
		{
			thr_data[i].res = 0;
			thr_data[i].rnds = rnds;
		}
			
		// start the threads
		for(int i = 0; i < threads; i++)
		{
			thr[i] = CreateThread(NULL, 0, thread_func, &thr_data[i], 0, 0);

			if (thr[i] == NULL)
			{
				ErrorHandler(TEXT("CreateThreadError"));
				ExitProcess(3);
			}
		}
			
		// wait for all threads to finish
		WaitForMultipleObjects(threads, thr, TRUE, INFINITE);

		result = 0;

		for(int i = 0 ; i<threads; i++)
		{
			CloseHandle(thr[i]);
				
			result += thr_data[i].res;
		}

		if((result != 0 ) && (result != number_of_tests)) // the cube is not constant
			trial = 1;
		
		// test the linearinty
		if(trial == 1)
		{
			dimension = ii;
			degree = 1;
				
			for(int i = 0; i < threads; i++)
			{
				thr_data[i].res = 0;
				thr_data[i].rnds = rnds;
			}

			// start the threads
			for(int i = 0; i < threads; i++)
			{
				thr[i] = CreateThread( NULL, 0, thread_func, &thr_data[i], 0, 0);

				if (thr[i] == NULL)
				{
					ErrorHandler(TEXT("CreateThreadError"));
					ExitProcess(3);
				}
			}
			
			// wait for all threads to finish
			WaitForMultipleObjects(threads, thr, TRUE, INFINITE);

			result = 0;

			for(int i = 0 ; i<threads; i++)
			{
				CloseHandle(thr[i]);
				
				result += thr_data[i].res;
			}

			if(result != 0) // the cube is not linear
				trial = 2;
		}

		if(trial == 2)
			degree = -1;

		// decide based on the test outputs :: 0 : 1 : -1
		if((degree == -1) && (ii < cubeSizeLIMIT))
		{
			//add one cube bit
			index_rnd_fill(cube,ii,cubeSizeLIMIT,START,END);
			ii++;
			printf("\nEvaluating cube with outbit %d: ",out);
			for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
		}
		else if((degree == -1) && (ii == cubeSizeLIMIT))
		{
			//reset
			lim = 1;
			ii =  rand_int(minCubeSizeLIMIT,cubeSizeLIMIT); //startCubeSize;
			array_rnd_fill(cube,ii,START,END);
			out = rand_int(START,END);
			outbit = 1ull<<out;
			printf("\nbig polynomial !\n\n");
			printf("\nEvaluating cube with outbit %d: ",out);
			for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
		}
		else if((degree == 0) && (lim <= searchLIMIT))
		{
			printf("\nconstant cube with size %d\n",ii);

			if (ii == minCubeSizeLIMIT) //reset
			{
				out = rand_int(START,END);
				outbit = 1ull<<out;
				array_rnd_fill(cube,ii,START,END);
				printf("\n\nEvaluating cube with outbit %d: ",out);
				for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
			}
			else if (ii > minCubeSizeLIMIT) //remove one cube bit
			{
				ii--;
				//index_rnd_fill(cube,ii,cubeSizeLIMIT,START,END);
				//ii++;
				printf("\nEvaluating cube with outbit %d: ",out);
				for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
			}
			else
				printf("erRRRRRror, ii is : %d",ii);
			lim++;
		}
		else if((degree == 0) && (lim > searchLIMIT))
		{
			printf("\nconstant cube with size %d\n",ii);

			//reset cube
			lim = 1;
			ii =  rand_int(minCubeSizeLIMIT,cubeSizeLIMIT); //startCubeSize;
			array_rnd_fill(cube,ii,START,END);
			out = rand_int(START,END);
			outbit = 1ull<<out;
			printf("\nsearch limit exceeded !\n\n");
			printf("\nEvaluating cube with outbit %d: ",out);
			for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
		}
		else if((degree == 1) && (result == 0))
		{
			//success
			printf("\ncube found with size %d\n",ii);
			check_and_write(output_file_name,cube,ii,rnds);

			//restart by a new random cube
			lim = 1;
			ii =  rand_int(minCubeSizeLIMIT,cubeSizeLIMIT); //startCubeSize;
			array_rnd_fill(cube,ii,START,END);
			out = rand_int(START,END);
			outbit = 1ull<<out;
			printf("\n\nEvaluating cube with outbit %d: ",out);
			for(int i = 0; i < ii; i++) printf("%d ",cube[i]);
		}
		else
			printf(":8:");
		//-------------------------------
	}
} // end of main

DWORD WINAPI thread_func(LPVOID lpParameter)
  {
	  thread_data *data = (thread_data*)lpParameter;

	  // TEXT AND KEY
	  uint16_t tmp_text32[2];
	  uint16_t tmp_key64[4];

	// cube test initialization
	uint16_t k[degLIMIT+1][4];
	uint64_t test_counter,sum,test_sum;
	sum = 0ull;
	test_sum = 0ull;
	memset(k,0ull,sizeof k);
	memset(tmp_key64,0ull,sizeof tmp_key64);
	
	data->res = 0;

	for(int loop = 0 ; loop < per_thread; loop++)
	{

		if((degree == 1) && (data->res != 0))
			break;

		// create random key
		for(int l = 0; l < (degLIMIT+1); l++)
			  for(int y = 0; y < 4; y++)
				k[l][y] = rand16();

        test_sum = 0ull;
		

		// do test of degree k
		if(degree == 0)
		{
			memset(tmp_key64,0ull,sizeof tmp_key64);

			for(int y = 0; y < 4; y++)
				tmp_key64[y] = k[0][y];
			
			sum = 0ull;
			
			for(int i = 0; i < (1 << dimension); i++)
			{
				memset(tmp_text32,0ull,sizeof tmp_text32);

				for(int j = 0; j < dimension; j++)
				{
					((uint32_t *)tmp_text32)[0] ^= ((i>>j) & 1ull) << (cube[j]);
				}
				
				simeck_32_64(tmp_key64, tmp_text32, tmp_text32, data->rnds);

				if (((uint64_t*)tmp_text32)[0] & outbit)
					sum ^= 1ull;
			}
			if(sum)
				data->res++;
		}
		else if (degree == 1)
		{
			for(test_counter = 0ull; test_counter < (1ull << (degree+1)); test_counter++)
			{
				memset(tmp_key64,0ull,sizeof tmp_key64);
				for(int j = 0; j < (degree+1); j++)
				{
					if((test_counter>>j) & 1ull)
                        for(int y = 0; y < 4; y++)
							tmp_key64[y] ^= k[j][y];
				}

				// calculate one test instance ( entire cube )
				// set cube variables to all possible values, calculate grain-128a and sum
				sum = 0ull;
				for(int i = 0; i < (1 << dimension); i++)
				{
					memset(tmp_text32,0ull,sizeof(tmp_text32));
					for(int j = 0; j < dimension; j++)
					{
						((uint32_t *)tmp_text32)[0] ^= ((i>>j) & 1ull) << (cube[j]);
					}

					simeck_32_64(tmp_key64, tmp_text32, tmp_text32,data->rnds);
					
					if (((uint64_t*)tmp_text32)[0] & outbit)
						sum ^= 1ull;
				}
				test_sum ^= sum;
			}
			if(test_sum)
				data->res++;
			}
	}
	return 0;
  } // end of *thr_func

bool write_maxterm(int size,int out, int rnds)
{
	string s;
	int counter = 0;
	uint64_t sum,sum_const;//,test_sum;
	
	sum_const = 0ull;
	
	for(int i = 0; i < (1<<size); i++)
	{
		memset(_key64,0ull,sizeof _key64);
		memset(_text32,0ull,sizeof(_text32));
		for(int j = 0; j < size; j++)
		{
			((uint32_t *)_text32)[0] ^= ((i>>j) & 1ull) << (cube[j]);
		}
		simeck_32_64(_key64, _text32, _text32,rnds);
		
		if (((uint64_t*)_text32)[0] & outbit)
					sum_const^= 1ull;
	}
	if(sum_const)
	{
		s += "1 +";
	}

	int k_index;
	for(k_index = 0; k_index<64; k_index++)
	{
		sum = 0ull;
		memset(_key64,0ull,sizeof _key64);
		
		((uint64_t *)_key64)[0] = 1ull << k_index;

		for(int i = 0; i < (1<<size); i++)
		{
			memset(_text32,0ull,sizeof(_text32));
			for(int j = 0; j < size; j++)
			{
				((uint32_t *)_text32)[0] ^= ((i>>j) & 1ull) << (cube[j]);
			}

			simeck_32_64(_key64, _text32, _text32,rnds);
			
			if (((uint64_t*)_text32)[0] & outbit)
					sum ^= 1ull;
			}

			if(sum ^ sum_const)
			{
				s += " X" + SSTR(k_index) + " +";
				counter++;
			}
		}
	
	if((counter <= _lin_max_vars) && (counter > 0) && (size <= _cube_max_size))
	{
		s += "\n";
		s = "index : " + SSTR(indx) + "\tcube size : " + SSTR(size) + "\toutbit : " + SSTR(out) + "\tthe maxterm : " + "key_var's count : " + SSTR(counter) + "\t" + s; 
		ofstream myfile;
		myfile.open(out_file_name,std::ios_base::app);
		myfile << s;
		myfile.close();
		return true;
	}
	return false;
  }

void write(const char *file_name,int *cube,int dim,int round)
{
	if(write_maxterm(dim,out, round))
	{
		int i;
		FILE *file;
		file = fopen(file_name,"a+");
		
		fprintf(file,"index :  %d",indx);
		fprintf(file,"\toutbit :  %d",out);
		fprintf(file,"\tNumber of Rounds :  %d",round);
		fprintf(file,"\tDim :  %d",dim);
		fprintf(file,"\tCube Indexs : ");
		for(i = 0; i < dim; i++) fprintf(file,"%d,",cube[i]);
		fprintf(file,"\n");
		fclose(file);
		
		indx++;
	}
}

void check_and_write(const char *file_name,int *cube,int cube_dim,int round)
{
	FILE *file;
	file = fopen(file_name,"r");
	
	char * scantext;
	char cubescanresult[100] = " ";
	
	int ind,ot,rrnds,cubeee_size,tmp_cube[31];

	bool flag_cube;
	bool flag_2;
	flag_cube = true;
	flag_2 = false;
	if ( (file != NULL) )
	{
		char line [ 1024 ]; /* or other suitable maximum line size */
		while ( (fgets ( line, sizeof line, file ) != NULL) && flag_cube ) /* read a line */
		{
			scantext = "index :  %d\toutbit :  %d\tNumber of Rounds :  %d\tDim :  %d\tCube Indexs : %s\n";
			
			sscanf(line,scantext,&ind,&ot,&rrnds,&cubeee_size,cubescanresult);
			
			char** tokens;
			
			tokens = str_split(cubescanresult, ',');
			
			//size = 0;
			//\n
			flag_cube = false;
			if((cube_dim == cubeee_size) && (out == ot))
			{
				if (tokens)
				{
					for (int i = 0; *(tokens + i); i++)
					{
						//size++;
						tmp_cube[i] = atoi(*(tokens + i));
						free(*(tokens + i));
					}
					free(tokens);
				}
				
				for(int k = 0; k < cube_dim; k++)
					if(cube[k] != tmp_cube[k])
					{
						flag_cube = true;
						break;
					}
				if(flag_cube)
				{
					flag_2 = false;
					for(int k = 0; k < cube_dim; k++)
					{
						for(int z = 0; z < cube_dim; z++)
						{
							if(cube[k] == tmp_cube[z])
							{
								flag_2 = true;
								break;
							}
						}
						if(!flag_2)
							break;
						else
							flag_2 = false;
					}
				}
	}
	else
	{
		flag_cube = true;
		flag_2 = false;
	}
		}
		fclose ( file);
	}
	else
	{
		perror ( file_name ); /* why didn't the file open? */
   }
   if(flag_cube && (!flag_2))
	   write(file_name,cube,cube_dim,round);
}

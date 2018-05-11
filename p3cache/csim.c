#include "cachelab.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
//function to read/process file
//structure of cache
//function to deal with LRU and distinguish LRU line
//function to initialize cache
//function to destroy or free space in cache
//function to take the processed events and put them through the cache

typedef struct{
	char type;
	unsigned long long int address;
	int size;
}event; 

typedef struct{
	event* events;
	int length;
}trace;

typedef struct{
	unsigned long long int data;
	unsigned long long int tag;
	int valid;
	long int time_stamp;
}block;

typedef struct{
	unsigned long long int tag;
	int set;
}block_info;

typedef struct{
	int sets;
	int block_bits;
	int lines;
	char* trace_file;
}cache_info;

typedef struct{
	int misses;
	int hits;
	int evictions;
}results;

int number_lines(char* t_file){
	printf("\n the file is %s\n",t_file);
	FILE *fp = fopen(t_file,"r");

	if(!fp){
		printf("\nCannot open file\n");
		exit(1);
	}
	int lines = 0;
	char ch;
	while(!feof(fp)){
		ch = fgetc(fp);
		if(ch == '\n') lines++;
	}
	fclose(fp);
	return lines;
}

trace process_trace(char* t_file){

	int num_lines = number_lines(t_file);
	printf("\n num lines %d\n",num_lines);
	event* events = malloc(sizeof(event)*num_lines);
	FILE *fp = fopen(t_file,"r");
		if(!fp){
		printf("\nCannot open file again\n");
		exit(1);
	}
	unsigned long long tmp_address;
	char tmp_type;
	int tmp_size;
	event tmp_event;
	int counter = 0;
	while(fscanf(fp," %c %llx,%d", &tmp_type,&tmp_address,&tmp_size)==3){
//		printf("\n type = %c",tmp_type);
		if(tmp_type != 'I'){
			tmp_event.type = tmp_type;
			tmp_event.address = tmp_address;
			tmp_event.size = tmp_size;
			events[counter] = tmp_event;
//			printf("\n type = %c address = %llu\n",tmp_type,tmp_address);
			counter++;
		}
	}
//	printf("\n counter = %d \n",counter);
	fclose(fp);
	trace ret_trace;
	ret_trace.length = counter;
	ret_trace.events = events;
	return ret_trace;
}

cache_info parse_cmd(int argc, char *args[]){
	int sets = 0;
	int lines = 0;
	int blocks = 0;
	char* t_file = NULL;
	for(int i = 1; i<argc; i++){
		char* arg = args[i];
		if(strcmp(arg,"-s") == 0) sets = atoi(args[i+1]);
		else if(strcmp(arg,"-E")==0) lines = atoi(args[i+1]);
		else if(strcmp(arg,"-b")==0) blocks = atoi(args[i+1]);
		else if(strcmp(arg,"-t")==0) t_file = args[i+1];
	}
	sets = pow(2,sets);
	cache_info cache;
	cache.sets = sets;
	cache.block_bits = blocks;
	cache.lines = lines;
	cache.trace_file = t_file;
//	printf("\n sets: %d, blocks: %d, lines: %d, file: %s\n",sets,blocks,lines,t_file);
	return cache;
}

block** create_cache(int sets, int lines){
	block** cache = (block**)malloc(sizeof(block*)*sets);
	for(int i = 0; i < sets; i++){
		cache[i] = (block*)malloc(sizeof(block)*lines);
		for(int j = 0; j < lines; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].time_stamp = 0;
			cache[i][j].data = 0;
		}
	}
	return cache;
}

block_info which_block(event trace, cache_info info){
	unsigned long long int address = trace.address;
	int s_bits = log2(info.sets);
	int b_bits = info.block_bits;
	int t_bits = (64 - (b_bits + s_bits));
//	printf("s_bits %d b_bits %d t_bits %d\n",s_bits,b_bits,t_bits);
	unsigned long long int tag = address >> (s_bits + b_bits);
//	printf("\n tag = %llu ",tag);
 	int set = (address << t_bits) >> (t_bits + b_bits);
 	block_info binfo;
 	binfo.tag = tag;
 	binfo.set = set;
 	return binfo;
	 
}

int find_LRU(block* set,int lines){
	int lru_index = 0;
	long int lru_time = set[0].time_stamp;
	for(int i = 0; i < lines; i++) 
		if(set[i].time_stamp < lru_time){
			lru_index = i;
			lru_time = set[i].time_stamp;
		}
	return lru_index;
}

int invalid_line(block* set, int lines){
	for(int i = 0; i < lines; i++) if(!(set[i].valid)) return i;
	return -1;
}

int in_cache(block* set, int lines, unsigned long long int tag){
	for(int i = 0; i < lines; i++){
		if(set[i].valid&&(set[i].tag == tag)) return i;
	}
	return -1;
}

void destroy_cache(int sets, int lines, block** cache){
	for(int i = 0; i < sets; i++){
		block* line = cache[i];
		if(line != NULL) free(line);
	}
	if(cache != NULL) free(cache);
	return;
}

//void print_event(event event){
//	printf("\n event type = %c, event address = %llu, event size = %d\n",event.type,event.address,event.size);
//	return;
//}

results simulate_cache(block** cache, trace trace, cache_info info){
	int misses = 0;
	int hits = 0;
	int evictions = 0;
	event* events = trace.events;
	int num_events = trace.length;
//	printf("\n num events %d\n", num_events);
	int lines = info.lines;
	for(int i = 0; i < num_events; i++){ //iterate through each of the traces 
		event current_event = events[i];
		//print_event(current_event);
		char type = current_event.type;
		block_info binfo = which_block(current_event, info);
		int set_index = binfo.set;
		unsigned long long int tag = binfo.tag;
//		printf("\n set index %d, tag %llu\n", set_index,tag);
		int incache = in_cache(cache[set_index],lines,tag);
		if(incache>=0){ //cache hit
			if(type == 'M'){
				hits += 2;
			}
			else hits++;
//			printf("hit 1\n");
			cache[set_index][incache].time_stamp = i;
		}
		else{
			misses++; 
//			printf("miss\n");
			int invalidline = invalid_line(cache[set_index],lines);
//			printf("\n invalidline %d",invalidline);
			if(invalidline >= 0){ // there is an available line
				if(type == 'M'){
					hits++;
//					printf("hit 2\n");
				}
				cache[set_index][invalidline].data = current_event.address;
				cache[set_index][invalidline].time_stamp = i;
				cache[set_index][invalidline].tag = tag;
				cache[set_index][invalidline].valid = 1;	

			}
			else{ /// cache set is full, find lru and evict line
				evictions++;
//				printf("evict\n");
				int lru = find_LRU(cache[set_index],lines);
				if(type == 'M'){
					hits++;
//					printf("hit 3\n");
				}
				cache[set_index][lru].data = current_event.address;
				cache[set_index][lru].time_stamp = i;
				cache[set_index][lru].tag = tag;
				cache[set_index][lru].valid = 1;
			}
		}
	}
	results results;
	results.hits = hits;
	results.misses = misses;
	results.evictions = evictions;
	return results;
}

int main(int argc, char* args[])
{

	//int test = number_lines("traces/dave.trace");
	//printf("\n this should equal 5, it equals %d\n",test);
	cache_info info = parse_cmd(argc,args);
	block** cache = create_cache(info.sets,info.lines);
	trace trace = process_trace(info.trace_file);
	results results = simulate_cache(cache,trace,info);
	destroy_cache(info.sets,info.lines,cache);
    printSummary(results.hits, results.misses, results.evictions);
    return 0;
}

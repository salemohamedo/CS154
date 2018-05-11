/*
 * mm.c
 *
 * This is the only file you should modify.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* word size (bytes) */  
#define DSIZE       8       /* doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)  /* initial heap size (bytes) */
#define OVERHEAD    8       /* overhead of header and footer (bytes) */
#define MIN_BLK_SIZE 24
#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack  a size and allocated bit into a word */
#define PACK(size, alloc)  (((size) | (alloc)))

/* Read and write a word at address p */
/* NB: this code calls a 32-bit quantity a word */
#define GET(p)       (*(unsigned  int *)(p))
#define PUT(p, val)  (*(unsigned  int *)(p) = (val))  

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)  
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
/* $end mallocmacros */
#define PREV_P(bp)     ((char *)(bp) + WSIZE)
#define NEXT_P(bp)     ((char *)(bp))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((p) + (ALIGNMENT-1)) & ~0x7)
#define GET_SEG_PT(i)  ((i)? heap_listp + i : NULL)
#define GET_SEG_I(bp)   ((bp)? (char*)(bp) - heap_listp : 0)
//#define CHAR_PT(bp)  ((char*)bp)
//#define NEXT_CHAR_PT(bp) ((char*)bp + DSIZE)

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p) {
    //printf("\n in inheap\n");
    return p < mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
//static int aligned(const void *p) {
    // printf("\n in alligned\n");
//    return (size_t)ALIGN(p) == (size_t)p;
//}


/* Global variables */
static char *heap_listp = NULL;  /* pointer to first block */  
//#ifdef NEXT_FIT
//static char *rover;       /* next fit rover */
//#endif
//static char *free_listp = NULL;
static char *seg_listp = NULL;
static char *max_segmentp = NULL;
//void *end;
/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);

int segregate_index(size_t size){
  if(size <= 32) return 0;
  else if(size <= 64) return 1;
  else if(size <= 128) return 2;
  else if(size <= 256) return 3;
  else if(size <= 512) return 4;
  else if(size <= 2048) return 5;
  else if(size <= 4096) return 6;
  else return 7;

}
static void LIFO_INSERT(char* bp){
  printf("in LIFO_INSERT\n");
  size_t bp_size = GET_SIZE(bp);
  char* seg_list = seg_listp + (segregate_index(bp_size)*WSIZE);
  int seg_list_pt = GET(seg_list);
  if(!seg_list_pt) PUT(seg_list,GET_SEG_I(bp));
  else{
    PUT(seg_list,GET_SEG_I(bp));
    PUT(NEXT_P(bp),seg_list_pt);
    PUT(PREV_P(GET_SEG_PT(seg_list_pt)),GET_SEG_I(bp));
  }
 // PUT(seg_list,bp);
 // PUT(seg_list_prev,0);
}

static void REMOVE_BLOCK(char *bp){
  printf("in REMOVE_BLOCK\n");

  size_t bp_size = GET_SIZE(bp);
  char* seg_list = seg_listp + (segregate_index(bp_size)*WSIZE);  
  //char* seg_list = segregate_index(bp_size);

  char* get_prev_ptr = GET_SEG_PT(GET(PREV_P(bp)));
  char* get_next_ptr = GET_SEG_PT(GET(NEXT_P(bp)));

  if(get_next_ptr){
    PUT(NEXT_P(get_prev_ptr),GET_SEG_I(get_next_ptr));
    if(get_next_ptr)
      PUT(PREV_P(get_next_ptr),GET_SEG_I(get_prev_ptr));
    PUT(PREV_P(bp),0);
    PUT(NEXT_P(bp),0);   

  }
  else{
    PUT(seg_list,GET_SEG_I(get_next_ptr));
    if(get_next_ptr)
      PUT(PREV_P(get_next_ptr),0);
    PUT(PREV_P(bp),0);
    PUT(NEXT_P(bp),0);     
  }


}

/* 
 * mm_init - Initialize the memory manager 
 */
/* $begin mminit */
int mm_init(void) 
{
  /* create the initial empty heap */
    printf("\n in init\n");

  if ((heap_listp = mem_sbrk(6*DSIZE)) == NULL)
    return -1;
  PUT(heap_listp,0);
  PUT(heap_listp + WSIZE,0);
  PUT(heap_listp + (WSIZE*2),0);
  PUT(heap_listp + (WSIZE*3),0);
  PUT(heap_listp + (WSIZE*4),0);
  PUT(heap_listp + (WSIZE*5),0);
  PUT(heap_listp + (WSIZE*6),0);
  PUT(heap_listp + (WSIZE*7),0);
  PUT(heap_listp + (WSIZE*8), 0);                        /* alignment padding */
  PUT(heap_listp+(WSIZE*9), PACK(DSIZE, 1));  /* prologue header */ 
  PUT(heap_listp+(WSIZE*10), PACK(DSIZE, 1));  /* prologue footer */ 
  PUT(heap_listp+(WSIZE*11), PACK(0, 1));   /* epilogue header */
  seg_listp = heap_listp;
  max_segmentp = heap_listp + (8*WSIZE);
  heap_listp += 10*WSIZE;

  /* Extend the empty heap with a free block of CHUNKSIZE bytes */
  if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
    return -1;
  return 0;
}
/* $end mminit */

/*
 * malloc - Allocate a block with at least size bytes of payload 
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size)
{
      printf("\n in malloc\n");

  size_t asize;      /* adjusted block size */
  size_t extendsize; /* amount to extend heap if no fit */
  char *bp;      
 if (heap_listp == 0){
    mm_init();
  }

  /* Ignore spurious requests */
  if (size <= 0)
    return NULL;

  /* Adjust block size to include overhead and alignment reqs. */
  if (size <= OVERHEAD)
    asize = 2*OVERHEAD;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / (DSIZE));


  /* Search the free list for a fit */
  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }
  //printf("\n didn't find fit \n");
  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize,CHUNKSIZE);
  if ((bp = extend_heap(extendsize/DSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  return bp;
} 
/* $end mmmalloc */

/* 
 * free - Free a block 
 */
/* $begin mmfree */
void mm_free(void *bp)
{
     printf("\n in free\n");

  if(bp == 0) return;
//siz
  size_t size = GET_SIZE(HDRP(bp));
  if (heap_listp == 0){
   mm_init();
 }


  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  ///set in header of next block set alloc for bp to 0
  //PUT(HDRP(NEXT_BLKP(bp)),GET(HDRP(NEXT_BLKP(bp)))&~2);
  PUT(NEXT_P(bp),0);
  PUT(PREV_P(bp),0);
  coalesce(bp);
 // mm_checkheap(0);
}

/* $end mmfree */

/*
 * realloc - naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{

    printf("in realloc\n");

  size_t oldsize;
  void *newptr;

  /* If size == 0 then this is just free, and we return NULL. */
  if(size == 0) {
    mm_free(ptr);
    return 0;
  }

  /* If oldptr is NULL, then this is just malloc. */
  if(ptr == NULL) {
    return mm_malloc(size);
  }

  newptr = mm_malloc(size);

  /* If realloc() fails the original block is left untouched  */
  if(!newptr) {
    return 0;
  }

  /* Copy the old data. */
  oldsize = GET_SIZE(HDRP(ptr));
  if(size < oldsize) oldsize = size;
  memcpy(newptr, ptr, oldsize);

  /* Free the old block. */
  mm_free(ptr);

  return newptr;
}




/* 
 * checkheap - Minimal check of the heap for consistency 
 */
void mm_checkheap(int verbose)
{
  char *bp = heap_listp;
    printf("\n in checkheap\n");

  if (verbose)
    printf("Heap (%p):\n", heap_listp);

  if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
    printf("Bad prologue header\n");
  checkblock(heap_listp);

  for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (verbose) 
      printblock(bp);
    checkblock(bp);
  }

  if (verbose)
    printblock(bp);
  if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
    printf("Bad epilogue header\n");
}

/* The remaining routines are internal helper routines */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words) 
{

      printf("\n in extendheap\n");

  char *bp;
  //siz
  size_t size;
  void *return_ptr;



  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words+1) * DSIZE : words * DSIZE;
  if ((long)(bp = mem_sbrk(size)) < 0) 
    return NULL;
 //int iprev_alloc = (GET(HDRP(bp)) & 2)|0;

  /* Initialize free block header/footer and the epilogue header */
  PUT(HDRP(bp), PACK(size, 0));         /* free block header */
  PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
  //PUT(NEXT_P(bp),NULL);
  //PUT(PREV_P(bp),NULL);
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

  PUT(NEXT_P(bp),0);
  PUT(PREV_P(bp),0);

  /* Coalesce if the previous block was free */
  //end = (unsigned int*)HDRP(NEXT_BLKP(bp));
  //LIFO_INSERT(bp);
  return_ptr = coalesce(bp);
  //mm_checkheap(0);
  return return_ptr;
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
/* $begin mmplace */
/* $begin mmplace-proto */
static void place(void *bp, size_t asize)
  /* $end mmplace-proto */
{

      printf("\n in place\n");

  size_t csize = GET_SIZE(HDRP(bp));

  REMOVE_BLOCK(bp);

  if ((csize - asize) >= 2*DSIZE) { 

    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(csize-asize, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(csize-asize, 0));
    PUT(NEXT_P(NEXT_BLKP(bp)),0);
    PUT(PREV_P(NEXT_BLKP(bp)),0);
    coalesce(NEXT_BLKP(bp));
  }
  else { 
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit(size_t asize)
{
  printf("in find_fit\n");

  char* seg_list = seg_listp + (segregate_index(asize)*WSIZE);
 // printf("\nmmeeh\n");
  //printf("\n hii bp = %p \n",GET_SIZE(HDRP(bp)));
 // printf("\n hi2 bp = %p\n",HDRP(bp));
 // printf("\n hi2 free = %p\n",free_listp);
 // printf("\n hi2 bp next = %p\n",(void*)NEXT_FR_BP(bp));
while(seg_list!=max_segmentp){
    void* bp = GET_SEG_PT(GET(seg_list));
  while(bp && GET_SIZE(HDRP(bp)) > 0){
    //printf("\nstuck here\n");
    if(GET_SIZE(HDRP(bp)) >= asize && !GET_ALLOC(HDRP(bp))){
      return bp;
    }
    bp = GET_SEG_PT(GET(NEXT_P(bp)));
  }
  seg_list += WSIZE;
 // bp = GET(seg_list);
}
  return NULL;
}

/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) 
{

      printf("\n in ffcoalesce\n");

  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));


  if (prev_alloc && next_alloc) {  
    printf("\nwent here 1\n");          /* Case 1 */
    LIFO_INSERT(bp);
    return bp;
  }

    if(prev_alloc && !next_alloc) {      /* Case 2 */
      printf("\nwent here 2\n");          /* Case 1 */

    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    REMOVE_BLOCK(NEXT_BLKP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size,0));

   // LIFO_INSERT(bp);
   // return bp;
  }

  else if (!prev_alloc && next_alloc) {      /* Case 3 */
      printf("\nwent here 3\n");          /* Case 1 */

    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    REMOVE_BLOCK(PREV_BLKP(bp));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
    //LIFO_INSERT(PREV_BLKP(bp));
     bp = PREV_BLKP(bp);
  }
  

  else if(!prev_alloc && !next_alloc) {  
    printf("\nwent here 4\n");          /* Case 1 */
                                   /* Case 4 */
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));  
    REMOVE_BLOCK(NEXT_BLKP(bp));
    REMOVE_BLOCK(PREV_BLKP(bp));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    
    
    //LIFO_INSERT(PREV_BLKP(bp));
    bp = PREV_BLKP(bp);

  }
  LIFO_INSERT(bp);
  return bp;

}


static void printblock(void *bp) 
{
     // printf("\n in printblock\n");

  size_t hsize;//, halloc, fsize, falloc;

  hsize = GET_SIZE(HDRP(bp));
  //halloc = GET_ALLOC(HDRP(bp));  
  //fsize = GET_SIZE(FTRP(bp));
  //falloc = GET_ALLOC(FTRP(bp));  

  if (hsize == 0) {
    printf("%p: EOL\n", bp);
    return;
  }

  /*  printf("%p: header: [%p:%c] footer: [%p:%c]\n", bp, 
      hsize, (halloc ? 'a' : 'f'), 
      fsize, (falloc ? 'a' : 'f')); */
}

static void checkblock(void *bp) 
{
     // printf("\n in checkblock\n");

  if ((size_t)bp % 8)
    printf("Error: %p is not doubleword aligned\n", bp);
  if (GET(HDRP(bp)) != GET(FTRP(bp)))
    printf("Error: header does not match footer\n");
}

// void *mm_calloc (size_t nmemb, size_t size)
// {

//      // printf("\n in calloc\n");

//   void *ptr;
//   if (heap_listp == 0){
//     mm_init();
//   }

//   ptr = mm_malloc(nmemb*size);
//   bzero(ptr, nmemb*size);


//   return ptr;
// }

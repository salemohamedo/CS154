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
#define OVERHEAD    16       /* overhead of header and footer (bytes) */
#define MIN_BLK_SIZE 24
#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack  a size and allocated bit into a word */
#define PACK(size, alloc)  ((unsigned int)((size) | (alloc)))

/* Read and write a word at address p */
/* NB: this code calls a 32-bit quantity a word */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))  

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
#define NEXT_P(bp)     ((char *)(bp))
//#define NEXT_FR_BP(bp)  (CHAR_PT(GET_PT(CHAR_PT(bp))))
#define NEXT_FR_BP(bp)  (NEXT_P(bp))
#define PREV_FR_BP(bp)  (PREV_P(bp))
#define PREV_P(bp)     ((char *)(bp) + DSIZE)
//#define PREV_FR_BP(bp)  (CHAR_PT(GET_PT(CHAR_PT(bp)) + DSIZE))
#define GET_PT(bp)     (*(unsigned long int *)(bp))
#define PUT_PT(bp,val) (*(unsigned long int *)(bp) = (unsigned long int)(val))
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)
#define CHAR_PT(bp)  ((char*)bp)
#define NEXT_CHAR_PT(bp) ((char*)bp + DSIZE)

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p) {
    printf("\n in inheap\n");
    return p < mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void *p) {
    printf("\n in alligned\n");
    return (size_t)ALIGN(p) == (size_t)p;
}


/* Global variables */
void *heap_listp = 0;  /* pointer to first block */  
//#ifdef NEXT_FIT
//static char *rover;       /* next fit rover */
//#endif
void *free_listp = 0;
void *end;
/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);


static void LIFO_INSERT(void* bp){
  if(!(void *)GET_PT(CHAR_PT(free_listp))){
    //free_listp =  bp;
    PUT_PT(free_listp,bp);
    PUT_PT(PREV_P(bp),NULL);
    PUT_PT(NEXT_P(bp),NULL);
    PUT_PT(NEXT_CHAR_PT(free_listp),bp);  
  }
  else{
  //void* root = (void*)free_listp;
  //NEXT_P(bp) = root;
//  PUT_PT(NEXT_P(PREV_P(bp)),bp);
//  PUT_PT(PREV_P(bp),PREV)
    PUT_PT(NEXT_P((void*)GET_PT(NEXT_CHAR_PT(free_listp))),bp);
    PUT_PT(PREV_P(bp),(void*)GET_PT(NEXT_CHAR_PT(free_listp)));
    PUT_PT(NEXT_P(bp),NULL);
    PUT_PT(NEXT_CHAR_PT(free_listp),bp); 
  //PREV_P(root) = bp;
  //free_listp = bp;
}
}

static void REMOVE_BLOCK(void *bp){
  void* next_ptr = (void *)NEXT_FR_BP(bp);
  void* prev_ptr = (void *)PREV_FR_BP(bp);
  
  if(!prev_ptr){
    printf("\n this 1\n");
    PUT_PT(CHAR_PT(free_listp),NEXT_FR_BP(bp));
    PUT_PT(PREV_P((void*)GET_PT(CHAR_PT(free_listp))),NULL);
  }
  else if(!next_ptr){
    printf("\n this 2 \n");
  if(!prev_ptr){
    PUT_PT(CHAR_PT(free_listp),NULL);
    PUT_PT(NEXT_CHAR_PT(free_listp),NULL);
    return;
  }
    printf("\n give me bp = %p \n",bp);
    printf("\n give me the previhous %p \n",PREV_P(bp));
    PUT_PT(NEXT_P(PREV_FR_BP(bp)),NULL);
    PUT_PT(NEXT_CHAR_PT(free_listp),PREV_FR_BP(bp));
  }
  else{
    printf("\nthis three\n");
    PUT_PT(PREV_P(NEXT_FR_BP(bp)),PREV_FR_BP(bp));
    PUT_PT(NEXT_P(PREV_FR_BP(bp)),NEXT_FR_BP(bp));
    return;
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
    printf("\n sizeof size_t * = %d\n",sizeof(size_t));
    printf("\n sizeof void * = %d\n",sizeof(void*));

    printf("\n initial heap list val = %p \n",heap_listp);
    printf("\n initial free list val = %p \n",free_listp);
  if ((heap_listp = mem_sbrk(4*DSIZE)) == NULL)
    return -1;
  PUT_PT(heap_listp,NULL);
  PUT_PT(heap_listp+DSIZE,NULL);
  free_listp = heap_listp;
  printf("\n 1 heap list val = %p \n",heap_listp);
  printf("\n 1 free list val = %p \n",free_listp);
  heap_listp += 2*DSIZE;
  PUT(heap_listp, 0);                        /* alignment padding */
  PUT(heap_listp+WSIZE, PACK(DSIZE, 1));  /* prologue header */ 
  //PUT(heap_listp+DSIZE,0);
  //PUT(heap_listp+DSIZE+DSIZE,0);
  PUT(heap_listp+DSIZE, PACK(DSIZE, 1));  /* prologue footer */ 
  PUT(heap_listp+DSIZE+WSIZE, PACK(0, 1));   /* epilogue header */
  //heap_listp += DSIZE;
/*  PUT(heap_listp+DSIZE,0);
  PUT(heap_listp+DSIZE+DSIZE,0);*/
  end = (unsigned int*)(heap_listp+DSIZE+WSIZE);
  heap_listp += DSIZE;

//#ifdef NEXT_FIT
//  rover = heap_listp;
//#endif
  printf("\n 2 heap list val = %p \n",heap_listp);
  printf("\n 2 free list val = %p \n",free_listp);

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

  unsigned int asize;      /* adjusted block size */
  unsigned int extendsize; /* amount to extend heap if no fit */
  char *bp;      
 // if (heap_listp == 0){
 //   mm_init();
 // }

  /* Ignore spurious requests */
  if (size <= 0)
    return NULL;

  /* Adjust block size to include overhead and alignment reqs. */
  if (size <= OVERHEAD)
    asize = DSIZE + OVERHEAD;
  else
    asize = OVERHEAD * ((size + (DSIZE) + (OVERHEAD-1)) / OVERHEAD);

  asize = MAX(ALIGN(size)+DSIZE,MIN_BLK_SIZE);
  printf("\n requested size = %u\n",asize);

  /* Search the free list for a fit */
  if ((bp = find_fit(asize)) != NULL) {
    printf("\n found fit\n");
  printf("\n malloc heap list val = %p \n",heap_listp);
  printf("\n malloc free list val = %p \n",free_listp);
  printf("\n found bp val = %p \n",bp);

    place(bp, asize);
    return bp;
  }
  printf("\n didn't find fit \n");
  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize,CHUNKSIZE);
  if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
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

  if(bp == NULL) return;
//siz
  unsigned int size = GET_SIZE(HDRP(bp));
 // if (heap_listp == 0){
 //   mm_init();
 // }



  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
  mm_checkheap(0);
}

/* $end mmfree */

/*
 * realloc - naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{
      printf("\n in realloc\n");

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
  oldsize = GET_SIZE(HDRP(ptr))-DSIZE;
  if(size <= oldsize) return ptr;
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
  unsigned int size;
  void *return_ptr;



  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) < 0) 
    return NULL;

  /* Initialize free block header/footer and the epilogue header */
  PUT(HDRP(bp), PACK(size, 0));         /* free block header */
  PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

  /* Coalesce if the previous block was free */
  end = (unsigned int*)HDRP(NEXT_BLKP(bp));
  return_ptr = coalesce(bp);
  mm_checkheap(0);
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

  if ((csize - asize) >= MIN_BLK_SIZE) { 
    //siz
    size_t asize_form = (asize + 0x7)&(~0x7);
    printf("\n asize_form = %lu\n",asize_form);
    printf("\n asize = %lu\n",asize);
    printf("\n csize = %lu\n",csize);    
    REMOVE_BLOCK(bp);
    PUT(HDRP(bp), PACK(asize_form, 1));
    PUT(FTRP(bp), PACK(asize_form, 1));
/*    if(PREV_P(bp)) NEXT_P(PREV_P(bp)) = NEXT_P(bp);
    else free_listp = NEXT_P(bp);
    PREV_P(NEXT_P(bp)) = PREV_P(bp);*/
    //bp = NEXT_BLKP(bp);
    PUT(HDRP(NEXT_BLKP(bp)), PACK(csize-asize_form, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(csize-asize_form, 0));
    LIFO_INSERT((void*)NEXT_BLKP(bp));
    //coalesce(bp);
  }
  else { 
/*    if(PREV_P(bp)) NEXT_P(PREV_P(bp)) = NEXT_P(bp);
    else free_listp = NEXT_P(bp);
    PREV_P(NEXT_P(bp)) = PREV_P(bp);*/
    REMOVE_BLOCK(bp);
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
  void* bp;
  bp = (void *)CHAR_PT(free_listp);
  printf("\n hii bp = %p \n",GET_SIZE(HDRP(bp)));
  printf("\n hi2 bp = %p\n",bp);
  printf("\n hi2 free = %p\n",free_listp);
  printf("\n hi2 bp next = %p\n",(void*)NEXT_FR_BP(bp));
  while(bp){
    //printf("\nstuck here\n");
    if(GET_SIZE(HDRP(bp)) >= asize){
      return bp;
    }
    bp = (void*)NEXT_FR_BP(bp);
  }
  return NULL;
}

/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) 
{

      printf("\n in ffcoalesce\n");

  size_t prev_alloc = (GET_ALLOC((void *)HDRP(bp) - WSIZE)<<1);
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

 // if (prev_alloc && next_alloc) {            /* Case 1 */
 //   LIFO_INSERT(bp);
 // }

    if(prev_alloc && !next_alloc) {      /* Case 2 */
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
/*    if(PREV_P(NEXT_BLKP(bp))) NEXT_P(PREV_P(NEXT_BLKP(bp))) = NEXT_P(NEXT_BLKP(bp));
    else free_listp = NEXT_P(NEXT_BLKP(bp));
    PREV_P(NEXT_P(NEXT_BLKP(bp))) = PREV_P(NEXT_BLKP(bp));*/
    printf("\n case 1\n");
    REMOVE_BLOCK(NEXT_BLKP(bp));
    PUT(HDRP(bp), PACK(size, prev_alloc));
    PUT(FTRP(bp), PACK(size,prev_alloc));
    LIFO_INSERT(bp);
    return bp;
  }

  else if (!prev_alloc && next_alloc) {      /* Case 3 */
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
/*    if(PREV_P(PREV_BLKP(bp))) NEXT_P(PREV_P(PREV_BLKP(bp))) = NEXT_P(PREV_BLKP(bp));
    else free_listp = NEXT_P(PREV_BLKP(bp));
    PREV_P(NEXT_P(PREV_BLKP(bp))) = PREV_P(PREV_BLKP(bp));*/
        printf("\n case 1\n");

    REMOVE_BLOCK(PREV_BLKP(bp));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    LIFO_INSERT(PREV_BLKP(bp));
    return PREV_BLKP(bp);
  }

  else if(!prev_alloc && !next_alloc) {                                     /* Case 4 */
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
/*    if(PREV_P(NEXT_BLKP(bp))) NEXT_P(PREV_P(NEXT_BLKP(bp))) = NEXT_P(NEXT_BLKP(bp));
    else free_listp = NEXT_P(NEXT_BLKP(bp));
    PREV_P(NEXT_P(NEXT_BLKP(bp))) = PREV_P(NEXT_BLKP(bp));
    if(PREV_P(PREV_BLKP(bp))) NEXT_P(PREV_P(PREV_BLKP(bp))) = NEXT_P(PREV_BLKP(bp));
    else free_listp = NEXT_P(PREV_BLKP(bp));
    PREV_P(NEXT_P(PREV_BLKP(bp))) = PREV_P(PREV_BLKP(bp));*/
        printf("\n case 1\n");

    REMOVE_BLOCK(NEXT_BLKP(bp));
    REMOVE_BLOCK(PREV_BLKP(bp));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    LIFO_INSERT(PREV_BLKP(bp));
    return PREV_BLKP(bp);

  }

//#ifdef NEXT_FIT
  /* Make sure the rover isn't pointing into the free block */
  /* that we just coalesced */
//  if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp))) 
//    rover = bp;
//#endif

  LIFO_INSERT(bp);
  return bp;
}


static void printblock(void *bp) 
{
      printf("\n in printblock\n");

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
      printf("\n in checkblock\n");

  if ((size_t)bp % 8)
    printf("Error: %p is not doubleword aligned\n", bp);
  if (GET(HDRP(bp)) != GET(FTRP(bp)))
    printf("Error: header does not match footer\n");
}

void *mm_calloc (size_t nmemb, size_t size)
{

      printf("\n in calloc\n");

  void *ptr;
  if (heap_listp == 0){
    mm_init();
  }

  ptr = mm_malloc(nmemb*size);
  bzero(ptr, nmemb*size);


  return ptr;
}

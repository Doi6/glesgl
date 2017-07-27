#ifndef GSGAH
#define GSGAH

// allocate 

#define ALLOC( typ ) (typ *)malloc(sizeof(typ))
#define ALLOCN( typ, n ) (typ *)malloc((n)*sizeof(typ))
#define FREE( x ) free( x )
#define REALLOCN( typ, old, n ) (typ *)realloc((old), (n)*sizeof(typ))
#define COPY( typ, src, dst ) memcpy( dst, src, sizeof(typ))
#define COPYN( typ, src, dst, n ) memcpy( dst, src, (n)*sizeof(typ))

/// generic dynamic array

/// name of array type
#define ARR( base ) gsga_##base

/// define array type
#define ARRDEF( base ) \
   typedef struct {    \
      int count;       \
      int size;        \
      base * items;    \
   } ARR( base )

/// create new array
#define ARRINIT( arr, base, isize ) { \
   arr = ALLOC( ARR(base) );          \
   arr->count = 0;                    \
   arr->size = isize;                 \
   arr->items = ALLOCN(base, isize);  \
}

/// free array
#define ARRFREE( arr ) { \
   FREE( arr->items );   \
   FREE( arr );          \
}

/// lookup and return a value in array
#define ARRLOOKUP( arr, keyfield, key, valuefield )  \
   { int gsga_i;                                     \
     for (gsga_i=0; gsga_i < arr->count; ++gsga_i) { \
	if ( arr->items[gsga_i].keyfield == key )    \
	   return arr->items[gsga_i].valuefield;     \
     }                                               \
   }
   
/// set array size
#define ARRSIZE( arr, base, asize ) \
   { arr->size = asize;             \
     arr->items = REALLOCN( base, arr->items, asize ); \
   }
   
/// add new item to array
#define ARRADD( arr, base, item ) {       \
   if (arr->count >= arr->size)           \
      ARRSIZE( arr, base, 2*arr->size );  \
   arr->items[ arr->count ++ ] = (item);  \
}

/// remove item by index
#define ARRREMOVE( arr, base, i ) {    \
   COPYN( base, arr->items+i+1,       \
      arr->items+i, arr->count-i-1 ); \
   -- arr->count;                      \
}
   
/// remove item by field balue
#define ARRREMOVEBY( arr, base, field, val ) {      \
   int gsga_i;                                      \
   for( gsga_i=0; gsga_i < arr->count; ++gsga_i) {  \
      if ( arr->items[ gsga_i ].field == (val)) {   \
	 ARRREMOVE( arr, base, gsga_i );            \
	 break;                                     \
      }                                             \
   }                                                \
}
      


#endif // GSGAH

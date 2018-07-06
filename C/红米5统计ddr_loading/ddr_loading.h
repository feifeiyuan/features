typedef signed      int    si;  
typedef signed      char   s8;  
typedef unsigned    char   u8;  
typedef unsigned    int    ui;  
typedef unsigned long int  uli;  
typedef unsigned long long ulli;

#define INVALID_ARGV        -1  
#define FAILED_OPEN_FILE    -2  
#define ERROR				-3

#define DEFAULT_TIME		60
#define MAX_AVALLABLE_FREQ  40  
#define BUFFER_SIZE			256

typedef struct Node  * SearchTree;
typedef uli 		TreeElement;
 
struct Node {
		TreeElement element;
		SearchTree RightNode;
		SearchTree LeftNode;
		uli count;
};

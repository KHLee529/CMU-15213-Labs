#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "cachelab.h"

#define MAX_INFILE_NAME_LEN 31 
#define ULL_MAX ((ull) -1)
#define BLCK_MSK (ULL_MAX >> (64 - b))
#define ADDR_SET_MSK (ULL_MAX >> (64 - s) << b)
#define ADDR_TAG_MSK (ULL_MAX << (s + b))
#define CACHE_VALID_BIT ((ull) 1 << 63)
#define CACHE_TAG_MSK (~CACHE_VALID_BIT)
#define get_block(addr) (addr & BLCK_MSK)
#define get_addr_set(addr) ((addr & ADDR_SET_MSK) >> b)
#define get_addr_tag(addr) ((addr & ADDR_TAG_MSK) >> (s + b))
#define get_cache_vb(addr) (addr & CACHE_VALID_BIT)
#define get_cache_tag(addr) (addr & CACHE_TAG_MSK)

/* start of error function implementation */
#define error(...) errorf(__FILE__, __LINE__, __VA_ARGS__)

#define assert(expr)                           \
    do {                                       \
        if (!(expr))                           \
            error("Assertion failed: " #expr); \
    } while (0)

static inline void errorf(char *file, int line, char *fmt, ...)
{
    fprintf(stderr, "%s:%d: ", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}
/* end of error function implementation */

/* start of list implementation */
typedef unsigned long long ull;

typedef struct __ListNode {
    unsigned long long val;
    struct __ListNode *next;
} ListNode;

typedef struct {
    unsigned len;
    ListNode *head;
    ListNode *tail;
} List;

/* recycle nodes */
static List recy;
static int recy_init = 0;

#define list_pop(list) list_pop_node((list), (list)->head)
#define list_pop_tail(list) list_pop_node((list), (list)->tail)
#define list_pop_val(list, val) \
    list_pop_node((list), list_find_node((list), (val)))

/*
 * pop the node from a list
 * if node is not in list, return NULL
 * @list: the list to pop from
 * @node: the address of the node to be popped
 */
static inline ListNode *list_pop_node(List *list, ListNode *node)
{
    if (!node) return NULL;
    ListNode **ptr = &list->head;
    ListNode *tmp = NULL, *last = NULL;
    while (*ptr != node){
        last = *ptr;
        ptr = &(*ptr)->next;
    }
    tmp = *ptr;
    *ptr = (*ptr)->next;
    if (tmp) {
        if (tmp == list->tail)
            list->tail = last;
        list->len--;
    }
    return tmp;
}

/*
 * initialize the given node with given value
 * @node: the node to be initialized
 * @val: the value given to set to the node
 */
static inline void node_init(ListNode* node, ull val)
{
    if (!node)
        error("initialize a null node\n\n");
    node->val = val;
    node->next = NULL;
}

/*
 * Initialize node with value then return back
 * @node: the node address to be initialized, NULL if new node needed
 * @val: the value to be assigned into node
 */
static inline ListNode *new_node(ull val)
{
    ListNode *node = list_pop(&recy);
    if (!node)
        node = (ListNode *) malloc(sizeof(ListNode));
    node_init(node, val);
    return node;
}

/*
 * Initialize list then return
 * @list: the list address to be initialized, NULL if new list needed
 */
static inline List *list_init(List *list)
{
    if (!list)
        list = (List *) malloc(sizeof(List));
    list->len = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

/* 
 * push a node to the head of the given list
 * @list: the list to push into
 * @node: the node to push
 */
static inline void list_push_node(List *list, ListNode *node)
{
    node->next = list->head;
    list->head = node;
    if (!list->len)
        list->tail = node;
    list->len++;
}

/*
 * Push a node initialized with given value to the head of the given list 
 * @list: the list to push into
 * @val: the value to push into
 */
static inline void list_push_val(List *list, ull val)
{
    ListNode *node = new_node(val);
    list_push_node(list, node);
}

/*
 * find the node in the list with given value
 * @list: the list to find
 * @val: the given value to find
 */
static inline ListNode *list_find_node(List *list, ull val)
{
    ListNode *ptr = list->head;
    ListNode *tmp = NULL;
    while (ptr) {
        if (ptr->val == val)
            tmp = ptr;
        ptr = ptr->next;
    }
    return tmp;
}

/*
 * push the node into recycling list
 * @node: the node to be recycle
 */
static inline void del_node(ListNode *node)
{
    if (!recy_init){
        list_init(&recy);
        recy_init = 1;
    }
    list_push_node(&recy, node);
}

/*
 * recycle the whole list
 * @list: the list to be recycled
 */
static void list_clear(List *list)
{
    while (list->len)
        del_node(list_pop_tail(list));
}

/*
 * free the whole recycle list
 */
static void recy_free()
{
    ListNode *ptr = recy.head, *tmp;
    while (ptr) {
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    recy_init = 0;
}

#ifdef DEBUG
static void print_list(const List *list)
{
    ListNode *ptr = list->head;
    int cnt = 0;
    printf("len: %d, head: %p, tail: %p\n", list->len, list->head, list->tail);
    while (ptr) {
        printf("\tnode{%d} at %p: {val: %llu, next: %p}\n", cnt++, ptr, ptr->val, ptr->next);
        ptr = ptr->next;
        if (cnt > 20)
            error("infinite loop");
    }
}
#endif
/* end of list implementation */

typedef struct {
    char ins;
    int size;
    ull addr;
} Ins;

/* Global variables */
static int s = 0, E = 0, b = 0, verbose = 0;
static int hit = 0, miss = 0, evic = 0;
static char infile[MAX_INFILE_NAME_LEN + 1] = "";
static List *cache = NULL;

void usage()
{
    printf("-h: Optional help flag that prints usage info\n"
            "-v: Optional verbose flag that displays trace info\n"
            "-s <s>: Number of set index bits (S = 2^s is the number of sets)\n"
            "-E <E>: Associativity (number of lines per set)\n"
            "-b <b>: Number of block bits (B = 2^b is the block size)\n"
            "-t <tracefile>: Name of the valgrind trace to replay\n");
}

void print_usage_and_exit()
{
    usage();
    exit(1);
}

void parse_args(int argc, char **argv)
{
    unsigned judge = 15; // judge will set to 0 if s, E, b, t are all set
    int opt;
    char optstring[] = "hvs:E:b:t:";
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            print_usage_and_exit();
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            judge &= ~1;
            break;
        case 'E':
            E = atoi(optarg);
            judge &= ~2;
            break;
        case 'b':
            b = atoi(optarg);
            judge &= ~4;
            break;
        case 't':
            strncat(infile, optarg, MAX_INFILE_NAME_LEN);
            judge &= ~8;
            break;
        default:
            printf("unknown parameter %c\n", opt);
            print_usage_and_exit();
        } 
    }
    if (judge) {
        printf("not enough parameters\n");
        print_usage_and_exit();
    }
}

void cache_init() 
{
    int S = 1 << s;
    cache = malloc(S * sizeof(List));
    if (!cache)
        error("malloc failure\n\n");
    for (int i = 0; i < S; i++) {
        list_init(&cache[i]);
    }
}

void cache_free()
{
    int S = 1 << s;
    for (int i = 0; i < S; i++) {
        list_clear(&cache[i]);
    }
    recy_free();
    free(cache);
    cache = NULL;
}

void open_input_file()
{
    if (!strlen(infile))
        error("Input file is not specified\n\n");
    if (!freopen(infile, "r", stdin)) 
        error("Cannot open file %s\n\n", infile);
}

#define hit()                       \
    do {                            \
        hit++;                      \
        if (verbose) printf(" hit");\
    } while (0)
    
#define miss()                       \
    do {                             \
        miss++;                      \
        if (verbose) printf(" miss");\
    } while (0)

#define evic()                              \
    do {                                    \
        evic++;                             \
        if (verbose) printf(" eviction");   \
    } while (0)


void use_data(const ull addr)
{
    ull set = get_addr_set(addr);
    ull tag = get_addr_tag(addr);
    ull cache_tag = tag | CACHE_VALID_BIT;
    ListNode *n = list_pop_val(&cache[set], cache_tag);
    if (n) {
        hit();
        list_push_node(&cache[set], n);
    } else {
        miss();
        if (cache[set].len == E){
            evic();
            del_node(list_pop_tail(&cache[set]));
        }
        list_push_val(&cache[set], cache_tag);
    }
}

void process(Ins ins)
{
    switch (ins.ins)
    {
    case 'M':
        use_data(ins.addr);
    case 'S':
    case 'L':
        use_data(ins.addr);
        break;
    default:
        error("Invalid instruction\n\n");
        break;
    }
    if (verbose) printf("\n");
}

char *strstrip(char *str)
{
    int len = strlen(str);
    int p = len - 1;
    while (isspace(str[p])) p--;
    str[++p] = '\0';
    while (*str && isspace(*str)) str++;
    return str;
}

Ins parse_ins(char *buf){
    if (buf[0] != ' ') return (Ins) {.ins = 'I', .addr = 0, .size = 0};
    Ins ins;
    buf = strstrip(buf);
    if (verbose)
        printf("%s", buf);
    if (sscanf(buf, "%c %llx,%d", &ins.ins, &ins.addr, &ins.size) != 3)
        error("instruction parse error\n\n");
    return ins;
}

#ifdef DEBUG
void print_cache(){
    printf("=====print cache=====\n");
    int S = 1 << s;
    for (int i = 0; i < S; i++) {
        if (cache[i].len) {
            printf("cache[%d] ", i);
            print_list(&cache[i]);
        }
    }
    printf("=====print recy=====\n");
    print_list(&recy);
    printf("====================\n");
}
#endif

int main(int argc, char **argv)
{
    char buf[32];
    Ins ins;
    parse_args(argc, argv);
    open_input_file();
    cache_init();
    while (fgets(buf, sizeof(buf), stdin) != NULL){
        ins = parse_ins(buf);
        if (ins.ins == 'I') continue;
        process(ins);
        #ifdef DEBUG
        print_cache();
        #endif
    }
    printSummary(hit, miss, evic);
    cache_free();
    return 0;
}

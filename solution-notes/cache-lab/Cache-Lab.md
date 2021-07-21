# Cache Lab Solution

## Cache Simulator

### Data Structure

Consider the behavior the cache simulator will have.

- Access a block in a set
- Get a new block if not exist in the set
- Evict the least-recently used block in the set if the set is full

To implement these behavior, I choose linked list as the data structure of
implementation of the cache simulators, in which the behavior can be represent
into a corresponding method.

- find a node in the list and push it to the head
- push a node to the head of the linked list
- del the last node of the list

the implementation is shown below

```c
typedef struct __ListNode {
    unsigned long long val;
    struct __ListNode *next;
} ListNode;

typedef struct {
    unsigned len;
    ListNode *head;
    ListNode *tail;
} List;

List cache[S];
```

The cache is represented by a `List` object array with length S = 2 ^ s, then
implement the methods shown below.

```c
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
```

## Optimizing Matrix Transpose

Since the Cache is set to 1K direct mapped memory with block size 32byte,
which are 32 sets which contain 8 integers in it at the same time.

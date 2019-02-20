struct Node { 
    void* data; 
    struct Node* next; // Pointer to next node in DLL 
};

typedef struct dll_{
    struct Node *head;
} dll_t;


/* Public Function declaration to create and return a new empty doubly linked list*/
dll_t * get_new_dll();

/*declaration for element addition in douly linked list.*/
void At_front(struct Node** head_ref, void*app_data);
void insertAfter(struct Node* prev_node, void*new_data);
void At_end(struct Node** head_ref, void* new_data);

void deleteNode(struct Node **head_ref, struct Node *del); 
void deleteList(struct Node** head_ref); 

  


  

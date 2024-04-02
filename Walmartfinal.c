#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ORDER 3     //no of children 2/3  no of keys 1/2
#define MAXCAPACITY 3   
#define MINCAPACITY 2
#define ARRAYSIZE 50
#define ITEMSIZE 30

typedef struct date
{
    int day;
    int month;
    int year; // in 2 dig
} date;

date *CreateDate(int DD, int MM, int YY)
{
    date *newDate = (date *)malloc(sizeof(date));
    newDate->day = DD;
    newDate->month = MM;
    newDate->year = YY;
    return newDate;
}

typedef struct item_tag
{
    int item_id;
    char *itemname;
    int quantity;
    date *expiry_date;
    int threshold_quantity;
    struct item_tag *prev;
    struct item_tag *next;
} Item;

typedef struct DataNode
{
    Item *ItemArr[MAXCAPACITY + 1];     //MAXCAPACITY + 1//when it crosses max cap, we split 
    int size;
    int ParentIndex;
    struct DataNode *prev;
    struct DataNode *next;
    struct KeyNode *parent;
}DataNode;

typedef struct KeyNode
{
    int KeyVal[ORDER-1];  //max can be order-1
    int size;
    int utag;   //1 for Keyptr, 2 for Dataptr
    int ParentIndex;
    struct KeyNode *parent;
    union Pointer
    {
        struct KeyNode *Keyptr[ORDER];
        DataNode *Dataptr[ORDER];
    }ptr;

}KeyNode;

typedef struct Aisle_tag // for one ailse in the walmart
{
    int aisle_no; // starts from 1
    char *aisle_name;
    KeyNode *Bplusroot;
} Aisle_tag;

Aisle_tag *AisleArray[ARRAYSIZE]; // indices are array nos

typedef struct Bill {
    int quantities[ITEMSIZE];   //indices represent item ids
    struct Bill *left;
    struct Bill *right;
} Bill;

void initializeAisleArray()
{
    int i;
    for (i = 0; i < ARRAYSIZE; i++)
    {
        AisleArray[i] = NULL;
    }
}

Aisle_tag *CreateAisle(char *name, int a_no)
{
    Aisle_tag *aptr = (Aisle_tag *)malloc(sizeof(Aisle_tag));
    aptr->aisle_name = (char *)malloc(strlen(name) + 1);
    strcpy(aptr->aisle_name, name);
    aptr->Bplusroot = NULL;
    aptr->aisle_no = a_no;
    return aptr;
};

void DeleteBplusTree(KeyNode *root);

void DeleteAisle(int a_no)
{
    if(AisleArray[a_no])
        DeleteBplusTree(AisleArray[a_no]->Bplusroot);
    AisleArray[a_no] = NULL;
}

void UpdateAisle(char *name, int a_no)
{
    if (AisleArray[a_no])
    {
        free(AisleArray[a_no]->aisle_name);
        AisleArray[a_no]->aisle_name = (char *)malloc(sizeof(strlen(name) + 1));
        strcpy(AisleArray[a_no]->aisle_name, name);
    }
    else
    {
        printf("Given Aisle no. does not exist\nCreating new Aisle\n");
        AisleArray[a_no] = CreateAisle(name, a_no);
    }
}

Item *CreateItem(int id, char *name, int quan, date *d, int tq)
{
    Item *nitem = (Item *)malloc(sizeof(Item));
    nitem->item_id = id;
    nitem->itemname = (char *)malloc(strlen(name) + 1);
    strcpy(nitem->itemname, name);
    nitem->quantity = quan;
    nitem->expiry_date = d;
    nitem->threshold_quantity = tq;
    nitem->next = nitem->prev = NULL;
    return nitem;
}

KeyNode* InsertintoDataNode(DataNode *dptr, Item *iptr, KeyNode* origin_root);

DataNode* EmptyDataNode()
{
    DataNode* d = (DataNode*)malloc(sizeof(DataNode));
    d->next = d->prev = NULL;
    d->size = 0;
    d->ParentIndex = -1;
    return d;
}

KeyNode* initializeBpTree(int key)      //for order 3, cap 3
{
    KeyNode* root = (KeyNode*)malloc(sizeof(KeyNode));
    root->KeyVal[0] = key;
    root->size = 1;
    root->utag = 2;
    root->parent = NULL;
    root->ParentIndex = -1;

    root->ptr.Dataptr[0] = EmptyDataNode();
    root->ptr.Dataptr[0]->ParentIndex = 0;
    root->ptr.Dataptr[0]->parent = root;
    root->ptr.Dataptr[1] = EmptyDataNode();
    root->ptr.Dataptr[1]->ParentIndex = 1;
    root->ptr.Dataptr[1]->parent = root;

    root->ptr.Dataptr[0]->next = root->ptr.Dataptr[1];
    root->ptr.Dataptr[1]->prev = root->ptr.Dataptr[0];  

    return root;
}

int isExpired(date *expiry, date *target)
{
    int retval;
    if (expiry->year < target->year)
    {
        retval = 1;
    }
    else if (expiry->year > target->year)
    {
        retval = 0;
    }
    else
    {
        if (expiry->month < target->month)
        {
            retval = 1;
        }
        else if (expiry->month > target->month)
        {
            retval = 0;
        }
        else
        {
            if (expiry->day < target->day)
            {
                retval = 1;
            }
            else
            {
                retval = 0;
            }
        }
    }
    return retval;
}

KeyNode* Insert(KeyNode* root, Item *iptr, KeyNode* origin_root)      //for order 3, no of keys 1 or 2
{
    //printf("\nInsert function with value %d\n", iptr->item_id);
    int newval = iptr->item_id;
    int i;

    for(i = 0; i < root->size; i++)
    {
        if(newval < root->KeyVal[i])
        {
            if(root->utag == 2)
            {
                //printf("going left of %d which is at index %d \n", root->KeyVal[i], i);
                return InsertintoDataNode(root->ptr.Dataptr[i], iptr, origin_root);
            }
            else
            {
                //printf("going left of %d\n", root->KeyVal[i]);
                return Insert(root->ptr.Keyptr[i], iptr, origin_root);
            }
        }
    }

    if(root->utag == 2)
    {
        //printf("going right of %d\n", root->KeyVal[i-1]);
        return InsertintoDataNode(root->ptr.Dataptr[i], iptr, origin_root);
    }
    else
    {
        //printf("going right of %d\n", root->KeyVal[i-1]);
        return Insert(root->ptr.Keyptr[i], iptr, origin_root);  //here i = size, last child
    }
}

KeyNode* PushKeyUp(int key, KeyNode* p,int parentindex, KeyNode* k1, KeyNode* k2, KeyNode* origin_root) //push key into keynode p and also set parent index and parent of k1 k2
{
    //printf("Inside push key up\n");
    if(p == NULL)
    {
        KeyNode *neworigin_root = (KeyNode *)malloc(sizeof(KeyNode));
        neworigin_root->size = 1;
        neworigin_root->parent = NULL;
        neworigin_root->KeyVal[0] = key;
        neworigin_root->ParentIndex = -1;
        neworigin_root->utag = 1;

        neworigin_root->ptr.Keyptr[0] = k1;
        neworigin_root->ptr.Keyptr[0]->parent = neworigin_root;
        neworigin_root->ptr.Keyptr[0]->ParentIndex = 0;

        neworigin_root->ptr.Keyptr[1] = k2;
        neworigin_root->ptr.Keyptr[1]->parent = neworigin_root;
        neworigin_root->ptr.Keyptr[1]->ParentIndex = 1;

        //printf("New origin root made with key %d\n", key);
        return neworigin_root;
    }
    if(p->size == 1)
    {
        //printf("push key case 1, p->size == 1\n");

        if(parentindex == 1)
        {
            p->KeyVal[1] = key;
            p->size = 2;

            p->ptr.Keyptr[1] = k1;
            p->ptr.Keyptr[1]->parent = p;
            p->ptr.Keyptr[1]->ParentIndex = 1;

            p->ptr.Keyptr[2] = k2;
            p->ptr.Keyptr[2]->parent = p;
            p->ptr.Keyptr[2]->ParentIndex = 2;
        }
        else if(parentindex == 0)
        {
            p->KeyVal[1] = p->KeyVal[0];
            p->KeyVal[0] = key;

            p->size = 2;
            p->ptr.Keyptr[2] = p->ptr.Keyptr[1];
            p->ptr.Keyptr[2]->ParentIndex = 2;

            p->ptr.Keyptr[1] = k2;
            p->ptr.Keyptr[1]->ParentIndex = 1;

            p->ptr.Keyptr[0] = k1;
            p->ptr.Keyptr[0]->ParentIndex = 0;
        }

        return origin_root;  
    }
    else if(p->size == 2)
    {
        //printf("push key case 2, p->size == 2\n");
        KeyNode *newk1, *newk2;
        int nextpush;
        if(parentindex == 0)
        {
            newk1 = (KeyNode *)malloc(sizeof(KeyNode));
            newk1->KeyVal[0] = key;
            newk1->size = 1;
            newk1->utag = 1;
            newk1->ptr.Keyptr[0] = k1;
            newk1->ptr.Keyptr[0]->ParentIndex = 0;
            newk1->ptr.Keyptr[0]->parent = newk1;
            newk1->ptr.Keyptr[1] = k2;
            newk1->ptr.Keyptr[1]->ParentIndex = 1;
            newk1->ptr.Keyptr[1]->parent = newk1;

            nextpush = p->KeyVal[0];
            p->KeyVal[0] = p->KeyVal[1];
            p->ptr.Keyptr[0] = p->ptr.Keyptr[1];
            p->ptr.Keyptr[1] = p->ptr.Keyptr[2];
            p->size = 1;
            p->ptr.Keyptr[0]->ParentIndex = 0;
            p->ptr.Keyptr[0]->parent = p;
            p->ptr.Keyptr[1]->ParentIndex = 1;
            p->ptr.Keyptr[1]->parent = p;
            newk2 = p;

        }
        else if(parentindex == 1)
        {
            nextpush = key;

            p->size = 1;
            p->ptr.Keyptr[1] = k1;
            p->ptr.Keyptr[1]->ParentIndex = 1;
            p->ptr.Keyptr[1]->parent = p;

            KeyNode *temp = p->ptr.Keyptr[2];
            newk1 = p;

            newk2 = (KeyNode*)malloc(sizeof(KeyNode));
            newk2->KeyVal[0] = p->KeyVal[1];
            newk2->size = 1;
            newk2->utag = 1;
            newk2->ptr.Keyptr[0] = k2;
            newk2->ptr.Keyptr[0]->ParentIndex = 0;
            newk2->ptr.Keyptr[0]->parent = newk2;

            newk2->ptr.Keyptr[1] = temp;
            newk2->ptr.Keyptr[1]->ParentIndex = 1;
            newk2->ptr.Keyptr[1]->parent = newk2;
        }
        else if(parentindex == 2)
        {
            p->size = 1;
            newk1 = p;

            nextpush = p->KeyVal[1];

            newk2 = (KeyNode*)malloc(sizeof(KeyNode));
            newk2->KeyVal[0] = key;
            newk2->size = 1;
            newk2->utag = 1;
            newk2->ptr.Keyptr[0] = k1;
            newk2->ptr.Keyptr[0]->ParentIndex = 0;
            newk2->ptr.Keyptr[0]->parent = newk2;

            newk2->ptr.Keyptr[1] = k2;
            newk2->ptr.Keyptr[1]->ParentIndex = 1;
            newk2->ptr.Keyptr[1]->parent = newk2;
        }
        return PushKeyUp(nextpush, p->parent, p->ParentIndex, newk1, newk2, origin_root);

        // if(p == origin_root)
        // {
        //     KeyNode *newRoot = (KeyNode*)malloc(sizeof(KeyNode));
        //     newRoot->KeyVal[0] = nextpush;
        //     newRoot->size = 1;
        //     newRoot->utag = 1;
        //     newRoot->ptr.Keyptr[0] = newk1;
        //     newRoot->ptr.Keyptr[0]->ParentIndex = 0;
        //     newRoot->ptr.Keyptr[1] = newk2;
        //     newRoot->ptr.Keyptr[1]->ParentIndex = 1;

        //     return newRoot;
        // }
        // else
        // {
        //     return PushKeyUp(nextpush, p->parent, p->ParentIndex, newk1, newk2, origin_root);
        // }
    }
}

KeyNode* InsertintoDataNode(DataNode *dptr, Item *iptr, KeyNode* origin_root)
{
    //printf("\nInside InsertintoDataNode with value of newitem = %d\n", iptr->item_id);
    int i = 0;
    Item *temp, *temp2;
    while(i < dptr->size && dptr->ItemArr[i]->item_id < iptr->item_id)
    {
        i++;
    }
    temp = dptr->ItemArr[i];
    dptr->ItemArr[i] = iptr;
    i++;
    while (i < dptr->size)
    {
        temp2 = dptr->ItemArr[i];
        dptr->ItemArr[i] = temp;
        temp = temp2;
        i++;
    }
    if(i <= MAXCAPACITY)
        dptr->ItemArr[i] = temp;

    // //printf("Current data node after adding becomes\n");
    // for(i = 0; i < dptr->size ; i++)
    // {
    //     printf("%d\n", dptr->ItemArr[i]->item_id);
    // }

    if (dptr->size < MAXCAPACITY)   //general
    {
        //printf("first case of insertion, into same datanode\n");
        dptr->size += 1;
        return origin_root;
    }
    else if(dptr->parent->size < ORDER-1)   //1 //splitting
    {
        //printf("second case of insertion, datanode split and new root keyval\n");
        DataNode *d2 = EmptyDataNode();
        d2->ItemArr[0] = dptr->ItemArr[2];
        d2->ItemArr[1] = dptr->ItemArr[3];
        dptr->ItemArr[2] = NULL;
        dptr->ItemArr[3] = NULL;

        d2->size = 2;
        dptr->size = 2;

        d2->next = dptr->next;
        d2->prev = dptr;
        dptr->next = d2;
        if(d2->next)
            d2->next->prev = d2;

        if(dptr->ParentIndex == 1)
        {
            dptr->parent->KeyVal[1] = d2->ItemArr[0]->item_id;
            dptr->parent->ptr.Dataptr[2] = d2;
            dptr->parent->ptr.Dataptr[2]->ParentIndex = 2;
        }
        else if(dptr->ParentIndex == 0)
        {
            dptr->parent->KeyVal[1] = dptr->parent->KeyVal[0];
            dptr->parent->KeyVal[0] = d2->ItemArr[0]->item_id;

            dptr->parent->ptr.Dataptr[2] = dptr->parent->ptr.Dataptr[1];
            dptr->parent->ptr.Dataptr[2]->ParentIndex = 2;
    
            dptr->parent->ptr.Dataptr[1] = d2;
            dptr->parent->ptr.Dataptr[1]->ParentIndex = 1;
        }
        dptr->parent->size ++;
        d2->parent = dptr->parent;
        
        //printf("new 2 keyvals = %d and %d\n", dptr->parent->KeyVal[0], dptr->parent->KeyVal[1]);
        return origin_root;
    }
    else    //third case of insertion
    {
        //printf("Im in the third case of insertion\n");

        DataNode *d2 = EmptyDataNode();
        d2->ItemArr[0] = dptr->ItemArr[2];
        d2->ItemArr[1] = dptr->ItemArr[3];
        dptr->ItemArr[2] = NULL;
        dptr->ItemArr[3] = NULL;

        d2->size = 2;
        dptr->size = 2;

        d2->next = dptr->next;
        d2->prev = dptr;
        dptr->next = d2;
        if(d2->next)
            d2->next->prev = d2;

        KeyNode* gparent = dptr->parent->parent; //storing the grandparent
        KeyNode* parent = dptr->parent;

        KeyNode *k1 = (KeyNode *)malloc(sizeof(KeyNode));
        KeyNode *k2 = (KeyNode *)malloc(sizeof(KeyNode));
        k1->size = 1;
        k1->utag = 2;
        k2->size = 1;
        k2->utag = 2;
        int nextpush;

        if(dptr->ParentIndex == 0)
        {
            nextpush = parent->KeyVal[0];

            k2->KeyVal[0] = parent->KeyVal[1];

            k2->ptr.Dataptr[0] = parent->ptr.Dataptr[1];
            k2->ptr.Dataptr[0]->parent = k2;
            k2->ptr.Dataptr[0]->ParentIndex = 0;

            k2->ptr.Dataptr[1] = parent->ptr.Dataptr[2];
            k2->ptr.Dataptr[1]->parent = k2;
            k2->ptr.Dataptr[1]->ParentIndex = 1;

            parent->KeyVal[0] = d2->ItemArr[0]->item_id;
            parent->size = 1;
            parent->ptr.Dataptr[1] = d2;
            parent->ptr.Dataptr[1]->parent = parent;
            parent->ptr.Dataptr[1]->ParentIndex = 1;
            k1 = parent;
        }
        else if(dptr->ParentIndex == 1)
        {
            nextpush = d2->ItemArr[0]->item_id;

            k2->KeyVal[0] = parent->KeyVal[1];

            k2->ptr.Dataptr[0] = d2;
            k2->ptr.Dataptr[0]->parent = k2;
            k2->ptr.Dataptr[0]->ParentIndex = 0;

            k2->ptr.Dataptr[1] = parent->ptr.Dataptr[2];
            k2->ptr.Dataptr[1]->parent = k2;
            k2->ptr.Dataptr[1]->ParentIndex = 1;

            parent->size = 1;
            k1 = parent;
        }
        else if(dptr->ParentIndex == 2)
        {
            nextpush = parent->KeyVal[1];

            k2->KeyVal[0] = d2->ItemArr[0]->item_id;

            k2->ptr.Dataptr[0] = dptr;
            k2->ptr.Dataptr[0]->parent = k2;
            k2->ptr.Dataptr[0]->ParentIndex = 0;

            k2->ptr.Dataptr[1] = d2;
            k2->ptr.Dataptr[1]->parent = k2;
            k2->ptr.Dataptr[1]->ParentIndex = 1;

            parent->size = 1;
            k1 = parent;
        }
        //printf("new 2 keyvals splitted are %d and %d and pushing %d\n", k1->KeyVal[0], k2->KeyVal[0], nextpush);
        return PushKeyUp(nextpush, gparent, parent->ParentIndex, k1, k2, origin_root);
    }
}

DataNode* HeadofDoubly(KeyNode* root)
{
    //printf("\ninside HeadofD\n");
    if(root->utag == 2)
    {
        return root->ptr.Dataptr[0];
    }
    else
    {
        return HeadofDoubly(root->ptr.Keyptr[0]);
    }
}

void PrintBplus (KeyNode* root)
{
    DataNode *head = HeadofDoubly(root);
    // printf("\nlets traverse in print\n");
    // printf("at root first key is %d\n", root->KeyVal[0]);
    if(root->size == 2)
    {
        printf("at root second key is %d\n", root->KeyVal[1]);
    }
    while(head)
    {
        Item *ptr;
        for(int i = 0; i < head->size; i++)
        {
            ptr = head->ItemArr[i];
            printf("Item ID: %d\t\t\tItem name: %s\t\t\tQuantity: %d\t\t\tThreshold qty: %d\t\t\texpiry date: %d/%d/%d\n", ptr->item_id, ptr->itemname, ptr->quantity, ptr->threshold_quantity, ptr->expiry_date->day, ptr->expiry_date->month, ptr->expiry_date->year);
        }
        printf("\n");
        head = head->next;
    }
}

void DeleteBplusTree(KeyNode *root)
{
    if(!root)
        return;
    if(root->utag == 2)     //child is dataptr
    {
        for(int i = 0; i <= root->size; i++)
        {
            free(root->ptr.Dataptr[i]);
        }
        return;
    }
    else
    {
        for(int i = 0; i <= root->size; i++)
        {
            DeleteBplusTree(root->ptr.Keyptr[i]);
        }
    }
}

void PrintMart()
{
    for (int i = 0; i < ARRAYSIZE; i++)
    {
        if (AisleArray[i] != NULL)
        {
            printf("\nAisle no = %d  Aisle name: %s \n", AisleArray[i]->aisle_no, AisleArray[i]->aisle_name);
            PrintBplus(AisleArray[i]->Bplusroot);
        }
    }
}


int BorrowFromRight(DataNode *curr)
{
    //printf("Inside Borrow From Right\n");
    int retval = 0;
    if(curr != NULL)
    {
        if(curr->next != NULL && curr->next->parent == curr->parent)
        {
            DataNode *next = curr->next;
            if(next->size == MINCAPACITY)
            {
                retval = BorrowFromRight(next);
            }
            if(next->size > MINCAPACITY)
            {
                retval = 1;
                curr->ItemArr[curr->size] = next->ItemArr[0];
                int i = 1;
                while(i < next->size)
                {
                    next->ItemArr[i-1] = next->ItemArr[i];
                    i++;
                }
                next->size--;
                curr->size++;
                next->parent->KeyVal[next->ParentIndex - 1] = next->ItemArr[0]->item_id;
            }
        }
    }
    return retval;
}

int BorrowFromRightKey(KeyNode *curr)
{
    //printf("inside Borrow from RIght key\n");
    int retval = 0;
    if(curr != NULL)
    {
        KeyNode *Parent = curr->parent;
        int i = curr->ParentIndex + 1;
        if(i < Parent->size + 1)
        {
            KeyNode *bptr = Parent->ptr.Keyptr[i];
            if(bptr != NULL)
            {
                if(bptr->size == ORDER/2)
                {
                    retval = BorrowFromRightKey(bptr);
                }
                if(bptr->size > ORDER/2)
                {
                    retval = 1;
                    curr->KeyVal[curr->size] = Parent->KeyVal[curr->ParentIndex];
                    curr->size++;
                    if(curr->utag == 1)
                    {
                        curr->ptr.Keyptr[curr->size + 1] = bptr->ptr.Keyptr[0];
                        curr->ptr.Keyptr[curr->size + 1]->ParentIndex = curr->size;
                        Parent->KeyVal[curr->ParentIndex] = bptr->KeyVal[0];
                        int j = 0;
                        while(j < bptr->size - 1)
                        {
                            bptr->KeyVal[j] = bptr->KeyVal[j+1];
                            bptr->ptr.Keyptr[j] = bptr->ptr.Keyptr[j+1];
                            bptr->ptr.Keyptr[j]->ParentIndex--;
                            j++;
                        }
                        bptr->ptr.Keyptr[j] = bptr->ptr.Keyptr[j+1];
                        bptr->ptr.Keyptr[j]->ParentIndex--;
                        bptr->size--;
                        curr->ptr.Keyptr[curr->size]->parent = curr;
                    }
                    else
                    {
                        curr->ptr.Dataptr[curr->size + 1] = bptr->ptr.Dataptr[0];
                        curr->ptr.Dataptr[curr->size + 1]->ParentIndex = curr->size;
                        Parent->KeyVal[curr->ParentIndex] = bptr->KeyVal[0];
                        int j = 0;
                        while(j < bptr->size - 1)
                        {
                            bptr->KeyVal[j] = bptr->KeyVal[j+1];
                            bptr->ptr.Dataptr[j] = bptr->ptr.Dataptr[j+1];
                            bptr->ptr.Dataptr[j]->ParentIndex--;
                            j++;
                        }
                        bptr->ptr.Dataptr[j] = bptr->ptr.Dataptr[j+1];
                        bptr->ptr.Dataptr[j]->ParentIndex--;
                        bptr->size--;
                        curr->ptr.Dataptr[curr->size]->parent = curr;
                        //printf("%d", bptr->ptr.Dataptr[0]->parent->KeyVal[0]);
                    }
                }
            }
        }
    }
    return retval;
}

int BorrowFromLeft(DataNode *curr)
{
    //printf("Inside Borrow From Left\n");
    int retval = 0;
    if(curr != NULL)
    {
        if(curr->prev != NULL && curr->prev->parent == curr->parent)
        {
            DataNode *prev = curr->prev;
            if(prev->size == MINCAPACITY)
            {
                retval = BorrowFromLeft(prev);
            }
            if(prev->size > MINCAPACITY)
            {
                retval = 1;
                int temp = curr->size;
                while(temp > 0)
                {
                    curr->ItemArr[temp] = curr->ItemArr[temp - 1];
                    temp--;
                }
                curr->ItemArr[0] = prev->ItemArr[prev->size - 1];
                curr->parent->KeyVal[curr->ParentIndex - 1]= curr->ItemArr[0]->item_id = prev->ItemArr[prev->size - 1]->item_id;
                prev->size--;
                curr->size++;
            }
        }
    }
    return retval;
}

int BorrowFromLeftKey(KeyNode *curr)
{
    int retval = 0;
    if(curr != NULL)
    {
        KeyNode *Parent = curr->parent;
        int i = curr->ParentIndex - 1;
        if(i >= 0)
        {
            KeyNode *bptr = Parent->ptr.Keyptr[i];
            if(bptr != NULL)
            {
                if(bptr->size == ORDER/2)
                {
                    retval = BorrowFromLeftKey(bptr);
                }
                if(bptr->size > ORDER/2)
                {
                    retval = 1;
                    int j = curr->size;
                    if(curr->utag == 1)
                    {
                        curr->ptr.Keyptr[j + 1] = curr->ptr.Keyptr[j];
                        curr->ptr.Keyptr[j+1]->ParentIndex++;
                        while(j > 0)
                        {
                            curr->KeyVal[j] = curr->KeyVal[j-1];
                            curr->ptr.Keyptr[j] = curr->ptr.Keyptr[j-1];
                            curr->ptr.Keyptr[j]->ParentIndex++;
                            j--;
                        }
                        curr->KeyVal[0] = Parent->KeyVal[bptr->ParentIndex];
                        curr->ptr.Keyptr[0] = bptr->ptr.Keyptr[bptr->size];
                        curr->ptr.Keyptr[0]->parent = curr;
                        curr->ptr.Keyptr[0]->ParentIndex = 0;
                        bptr->size--;
                        Parent->KeyVal[bptr->ParentIndex] = bptr->KeyVal[bptr->size];
                        curr->size++; 
                    }
                    else
                    {
                        curr->ptr.Dataptr[j + 1] = curr->ptr.Dataptr[j];
                        curr->ptr.Dataptr[j+1]->ParentIndex++;
                        while(j > 0)
                        {
                            curr->KeyVal[j] = curr->KeyVal[j-1];
                            curr->ptr.Dataptr[j] = curr->ptr.Dataptr[j-1];
                            curr->ptr.Dataptr[j]->ParentIndex++;
                            j--;
                        }
                        curr->KeyVal[0] = Parent->KeyVal[bptr->ParentIndex];
                        curr->ptr.Dataptr[0] = bptr->ptr.Dataptr[bptr->size];
                        curr->ptr.Dataptr[0]->parent = curr;
                        curr->ptr.Dataptr[0]->ParentIndex = 0;
                        bptr->size--;
                        curr->size++;
                        Parent->KeyVal[bptr->ParentIndex] = bptr->KeyVal[bptr->size];   
                    }
                }
            }

        }
    }
    return retval;
}

int MergeWithRight(DataNode *dptr)
{
    //printf("Inside Merge With Right\n");
    int retval = 0;
    if(dptr != NULL)
    {
        //printf("%d\n", dptr->ParentIndex);
        if(dptr->next != NULL && dptr->next->parent == dptr->parent && dptr->next->size == MINCAPACITY)
        {
            retval = 1;
            DataNode *next = dptr->next;
            int i = dptr->size, j = 0;
            while(j < next->size)
            {
                dptr->ItemArr[i] = next->ItemArr[j];
                i++;
                j++;
            }
            dptr->size += next->size;
            KeyNode *Parent = dptr->parent;
            j = dptr->ParentIndex;
            while(j < Parent->size - 1)
            {
                Parent->KeyVal[j] = Parent->KeyVal[j+1];
                Parent->ptr.Dataptr[j+1] = Parent->ptr.Dataptr[j+2];
                Parent->ptr.Dataptr[j]->ParentIndex--;
                j++;
            }
            Parent->size--;
            dptr->next = next->next;
            if(next->next != NULL) next->next->prev = dptr;
            free(next);
        }
    }
    return retval;
}

int MergeWithRightKey(KeyNode *curr)
{
    //printf("Inside Merge with Right Key\n");
    int retval = 0;
    if(curr != NULL)
    {
        KeyNode *Parent = curr->parent;
        int i = curr->ParentIndex + 1;
        if(i < Parent->size + 1)
        {
            KeyNode *bptr = Parent->ptr.Keyptr[i];
            if(bptr != NULL && bptr->size == ORDER/2)
            {
                retval = 1;
                int i = curr->size+1, j = 0;
                curr->KeyVal[curr->size] = Parent->KeyVal[curr->ParentIndex];
                if(curr->utag == 1)
                {
                    curr->ptr.Keyptr[curr->size]->ParentIndex = curr->size;
                    while(j < bptr->size)
                    {
                        curr->KeyVal[i] = bptr->KeyVal[j];
                        curr->ptr.Keyptr[i+1] = bptr->ptr.Keyptr[j];
                        curr->ptr.Keyptr[i+1]->parent = curr;
                        curr->ptr.Keyptr[i+1]->ParentIndex = i+1;
                        i++;
                        j++;
                    }
                    curr->ptr.Keyptr[i+1] = bptr->ptr.Keyptr[j];
                    curr->ptr.Keyptr[i+1]->parent = curr;
                    curr->ptr.Keyptr[i+1]->ParentIndex = i+1;
                    curr->size += bptr->size+1;
                    j = curr->ParentIndex;
                    while(j < Parent->size-1)
                    {
                        Parent->KeyVal[j] = Parent->KeyVal[j+1];
                        Parent->ptr.Keyptr[j+1] = Parent->ptr.Keyptr[j+2];
                        Parent->ptr.Keyptr[j+1]->ParentIndex--;
                        j++;
                    }
                    Parent->ptr.Keyptr[j+1] = Parent->ptr.Keyptr[j+2];
                    Parent->ptr.Keyptr[j+1]->ParentIndex--;
                    Parent->size--;
                    
                    free(bptr);
                }
                else
                {
                    curr->ptr.Dataptr[curr->size]->ParentIndex = curr->size;
                    while(j < bptr->size)
                    {
                        curr->KeyVal[i] = bptr->KeyVal[j];
                        curr->ptr.Dataptr[i+1] = bptr->ptr.Dataptr[j];
                        curr->ptr.Dataptr[i+1]->parent = curr;
                        curr->ptr.Dataptr[i+1]->ParentIndex = i+1;
                        i++;
                        j++;
                    }
                    curr->ptr.Dataptr[i+1] = bptr->ptr.Dataptr[j];
                    curr->ptr.Dataptr[i+1]->parent = curr;
                    curr->ptr.Dataptr[i+1]->ParentIndex = i+1;
                    curr->size += bptr->size+1;
                    j = curr->ParentIndex;
                    while(j < Parent->size-1)
                    {
                        Parent->KeyVal[j] = Parent->KeyVal[j+1];
                        Parent->ptr.Keyptr[j+1] = Parent->ptr.Keyptr[j+2];
                        Parent->ptr.Keyptr[j+1]->ParentIndex--;
                        j++;
                    }
                    Parent->ptr.Keyptr[j] = Parent->ptr.Keyptr[j+1];
                    Parent->ptr.Keyptr[j]->ParentIndex--;
                    Parent->size--;
                    free(bptr);
                }
                
            }

        }


    }
    return retval;
}


int MergeWithLeft(DataNode *dptr)
{
    //printf("Inside MergeWithLeft\n");
    int retval = 0;
    if(dptr != NULL)
    {
        if(dptr->prev != NULL && dptr->prev->parent == dptr->parent && dptr->prev->size == MINCAPACITY)
        {
            DataNode *prev = dptr->prev;
            int i = prev->size, j = 0;
            while(j < dptr->size)
            {
                prev->ItemArr[i] = dptr->ItemArr[j];
                i++;
                j++;
            }
            prev->size += dptr->size;
            KeyNode *Parent = dptr->parent;
            j = dptr->ParentIndex;
             j = prev->ParentIndex;
            while(j < Parent->size - 1)
            {
                Parent->KeyVal[j] = Parent->KeyVal[j+1];
                Parent->ptr.Dataptr[j+1] = Parent->ptr.Dataptr[j+2];
                Parent->ptr.Dataptr[j]->ParentIndex--;
                j++;
            }
            Parent->size--;
            prev->next = dptr->next;
            if(dptr->next != NULL) dptr->next->prev = prev;
            free(dptr);
        }
    }
    return retval;
}

int MergeWithLeftKey(KeyNode *curr)
{
    //printf("Inside Merge with Left Key\n");
    int retval = 0;
    if(curr != NULL)
    {
        KeyNode *Parent = curr->parent;
        int i = curr->ParentIndex - (curr->ParentIndex != Parent->size -1);
        if(i >= 0)
        {
            KeyNode *bptr = Parent->ptr.Keyptr[i];
            if(bptr != NULL && bptr->size == ORDER/2)
            {
                retval = 1;
                bptr->KeyVal[bptr->size] = Parent->KeyVal[bptr->ParentIndex];
                int i = bptr->size+1, j = 0;
                if(bptr->utag == 1)
                {
                    bptr->ptr.Keyptr[bptr->size]->ParentIndex = bptr->size;
                    while(j < curr->size)
                    {
                        bptr->KeyVal[i] = curr->KeyVal[j];
                        bptr->ptr.Keyptr[i+1] = curr->ptr.Keyptr[j];
                        bptr->ptr.Keyptr[i+1]->parent = bptr;
                        bptr->ptr.Keyptr[i+1]->ParentIndex = i+1;
                        i++;
                        j++;
                    }
                    bptr->ptr.Keyptr[i+1] = curr->ptr.Keyptr[j];
                    bptr->ptr.Keyptr[i+1]->parent = bptr;
                    bptr->ptr.Keyptr[i+1]->ParentIndex = i+1;
                    bptr->size += curr->size+1;
                    j = bptr->ParentIndex;
                    while(j < Parent->size-1)
                    {
                        Parent->KeyVal[j] = Parent->KeyVal[j+1];
                        Parent->ptr.Keyptr[j+1] = Parent->ptr.Keyptr[j+2];
                        Parent->ptr.Keyptr[j+1]->ParentIndex--;
                        j++;
                    }
                    Parent->ptr.Keyptr[j] = Parent->ptr.Keyptr[j+1];
                    Parent->ptr.Keyptr[j]->ParentIndex--;
                    Parent->size--;
                    free(curr);
                }
                else
                {
                    bptr->ptr.Dataptr[bptr->size]->ParentIndex = bptr->size;
                    while(j < curr->size)
                    {
                        bptr->KeyVal[i] = curr->KeyVal[j];
                        bptr->ptr.Dataptr[i+1] = curr->ptr.Dataptr[j];
                        bptr->ptr.Dataptr[i+1]->parent = bptr;
                        bptr->ptr.Dataptr[i+1]->ParentIndex = i+1;
                        i++;
                        j++;
                    }
                    bptr->ptr.Dataptr[i+1] = curr->ptr.Dataptr[j];
                    bptr->ptr.Dataptr[i+1]->parent = bptr;
                    bptr->ptr.Dataptr[i+1]->ParentIndex = i+1;
                    bptr->size += curr->size+1;
                    j = bptr->ParentIndex;
                    while(j < Parent->size-1)
                    {
                        Parent->KeyVal[j] = Parent->KeyVal[j+1];
                        Parent->ptr.Keyptr[j+1] = Parent->ptr.Keyptr[j+2];
                        Parent->ptr.Keyptr[j+1]->ParentIndex--;
                        j++;
                    }
                    Parent->ptr.Keyptr[j] = Parent->ptr.Keyptr[j+1];
                    Parent->ptr.Keyptr[j]->ParentIndex--;
                    Parent->size--;
                    free(curr);
                }
            }                        

        }

    }
    return retval;
}

KeyNode *CheckParent(KeyNode *dptr, KeyNode *origin_root)
{
    //printf("Inside check parent\n");
    if(dptr != NULL)
    {
        if(dptr->size < ORDER/2)
        {
            KeyNode *Parent = dptr->parent;
            if(Parent != NULL)
            {
                int rightcheck = BorrowFromRightKey(dptr), leftcheck = 0;
                if(rightcheck == 0)
                {
                    leftcheck = BorrowFromLeftKey(dptr);
                }
                if(rightcheck == 0 && leftcheck == 0)
                {
                    rightcheck = MergeWithRightKey(dptr);
                    if(rightcheck == 0)
                    {
                        leftcheck = MergeWithLeftKey(dptr);
                    }
                    origin_root = CheckParent(Parent, origin_root);
                }

            }
            else
            {
                if(dptr->size == 0)
                {
                    origin_root = dptr->ptr.Keyptr[0];
                }
            }
            
        }
    }
    return origin_root;
}

KeyNode *DeleteFromDataNode(DataNode *dptr,int val, KeyNode *origin_root)
{
    //printf("Inside DeleteFromDataNode to delete %d \n", val);
    if(dptr != NULL)
    {
        int i;
        for(i = 0; i < dptr->size && dptr->ItemArr[i]->item_id != val; i++){}
        if(i == dptr->size)
        {
            printf("No item match the item id\n");
        }
        else
        {
            i++;
            while(i < dptr->size)
            {
                dptr->ItemArr[i-1] = dptr->ItemArr[i];
                i++;
            }
            dptr->size -= 1;
            //printf("%d\n", dptr->size);
        
            if(dptr->size < MINCAPACITY)
            {
                int rightcheck = BorrowFromRight(dptr);
                int leftcheck= 0;
                if(rightcheck == 0)
                {
                    leftcheck = BorrowFromLeft(dptr);
                }
                if(rightcheck == 0 && leftcheck == 0)
                {
                    rightcheck = MergeWithRight(dptr);
                    if(rightcheck == 0)
                    {
                        leftcheck = MergeWithLeft(dptr);
                    }
                    origin_root = CheckParent(dptr->parent, origin_root);
                }
            }
            else printf("Deletion Successful\n");
        }
    }
    return origin_root;
}


KeyNode *Delete(KeyNode *root, int val, KeyNode *origin_root)
{
    if(root != NULL)
    {
        int i;
        for(i = 0; i < root->size; i++)
        {
            //printf("In for loop \n");
            if(val < root->KeyVal[i])
            {
                //printf("Current keyval %d\n", root->KeyVal[i]);
                if(root->utag == 2)
                {
                    //printf("Deleting value %d from index %d\n", val, root->KeyVal[i]);
                    return DeleteFromDataNode(root->ptr.Dataptr[i], val, origin_root);
                }
                else
                {
                    //printf("Going to the left of %d \n", root->KeyVal[i]);
                    //printf("%d\n", root->ptr.Keyptr[i]->utag);
                    return Delete(root->ptr.Keyptr[i], val, origin_root);
                }
            }
        }
        if(root->utag == 2)
        {

            //printf("Deleting value %d from right of %d\n", val, root->KeyVal[i]);
            return DeleteFromDataNode(root->ptr.Dataptr[i], val, origin_root);
        }
        else
        {
            //printf("Going to the rightmost child of %d", root->KeyVal[root->size-1]);
            return Delete(root->ptr.Keyptr[root->size], val, origin_root);
        }
    }
    else
    {
        printf("Node Empty\n");
    }
}

Item *BplusSearchintoDatanode(int itemid, DataNode *d)
{
    for(int i = 0; i < d->size; i++)
    {
        if(d->ItemArr[i]->item_id == itemid)
        {
            return d->ItemArr[i];
        }
    }
    return NULL;
}

Item *BplusSearch(int itemid, KeyNode *root) { 
    if (root == NULL) {
        return NULL;
    }

    int i;
    if (root->utag == 1) {
        //child is Key node
        for (i = 0; i < root->size; i++) {
            if (itemid < root->KeyVal[i]) {
                return BplusSearch(itemid, root->ptr.Keyptr[i]);
            }
        }
        return BplusSearch(itemid, root->ptr.Keyptr[i]);
    } else {
        //child is Data node
        for (i = 0; i < root->size; i++) {
            if (itemid < root->KeyVal[i]) {
                return BplusSearchintoDatanode(itemid, root->ptr.Dataptr[i]);
            }
        }
        return BplusSearchintoDatanode(itemid, root->ptr.Dataptr[i]);
    }
}

DataNode* BplusSearchDatanode(int itemid, KeyNode *root) { 
    if (root == NULL) {
        return NULL;
    }

    int i;
    if (root->utag == 1) {
        //child is Key node
        for (i = 0; i < root->size; i++) {
            if (itemid < root->KeyVal[i]) {
                return BplusSearchDatanode(itemid, root->ptr.Keyptr[i]);
            }
        }
        return BplusSearchDatanode(itemid, root->ptr.Keyptr[i]);
    } else {
        //child is Data node
        for (i = 0; i < root->size; i++) {
            if (itemid < root->KeyVal[i]) {
                return root->ptr.Dataptr[i];
            }
        }
        return root->ptr.Dataptr[i];
    }
}

void RemoveItem(int id)
{
    Item *iptr;
    int i, flag = 0;
    for (i = 0; i < ARRAYSIZE && flag == 0; i++)
    {
        if (AisleArray[i] != NULL && AisleArray[i]->Bplusroot != NULL)
        {
            iptr = BplusSearch(id, AisleArray[i]->Bplusroot);
            if(iptr)
            {
                AisleArray[i]->Bplusroot = Delete(AisleArray[i]->Bplusroot, id, AisleArray[i]->Bplusroot);
                flag = 1;
            }
        }
    }
    if(i == ARRAYSIZE) printf("Cannot remove\n");  
}

void AddOrUpdateItem(int id, char *name, int quan, int DD, int MM, int YY, int tq, int aisle_no)
{
    //aisle array already initialized
    date *d;
    d = CreateDate(DD, MM, YY);

    if (AisleArray[aisle_no] == NULL)
    {
        Item *iptr = CreateItem(id, name, quan, d, tq);
        char Aislename[50];
        printf("No Aisle with this no, please enter Aisle name to create Aisle\n");
        scanf("%s", Aislename);
        AisleArray[aisle_no] = CreateAisle(Aislename, aisle_no);
        KeyNode *root = initializeBpTree(id + 2);
        AisleArray[aisle_no]->Bplusroot = Insert(root, iptr, root);
        printf("Item Successfully Added\n");
    }
    else if(AisleArray[aisle_no]->Bplusroot == NULL)
    {
        Item *iptr = CreateItem(id, name, quan, d, tq);
        KeyNode *root = initializeBpTree(id + 2);
        AisleArray[aisle_no]->Bplusroot = Insert(root, iptr, root);
        printf("Item Successfully Added\n");
    }
    else
    {
        Item *ptr = BplusSearch(id, AisleArray[aisle_no]->Bplusroot);
        if(ptr && ptr->expiry_date->day == DD && ptr->expiry_date->month == MM && ptr->expiry_date->year == YY) //item already exists, we will update
        {
            free(ptr->expiry_date);
            ptr->expiry_date = d;
            ptr->quantity += quan;
            printf("Item Successfully Updated\n");
        }
        else
        {
            Item *iptr = CreateItem(id, name, quan, d, tq);
            AisleArray[aisle_no]->Bplusroot = Insert(AisleArray[aisle_no]->Bplusroot, iptr, AisleArray[aisle_no]->Bplusroot);
            printf("Item Successfully Added\n");
        }
    }
}

Item* isItemAvailable(int id, int qty_req, int DD, int MM, int YY)
{
    date *d = CreateDate(DD, MM, YY);
    int i, retval, flag;
    retval = flag = 0;

    Item *iptr;

    for (i = 0; i < ARRAYSIZE && flag == 0; i++)
    {
        if (AisleArray[i] != NULL && AisleArray[i]->Bplusroot != NULL)
        {
            iptr = BplusSearch(id, AisleArray[i]->Bplusroot);
            if(iptr)
            {
                if(iptr->quantity >= qty_req && !isExpired(iptr->expiry_date, d))
                {
                    printf("Item is available\n");
                    return iptr;
                }
            }
        }
    }
    printf("Item is unavailable\n");
    return NULL;
}

void ThresholdCheck(Item *iptr) // To be called at the time of purchase
{
    if (iptr->quantity < iptr->threshold_quantity)
    {
        printf("Item id: %d %s has gone beyond Threshold\n", iptr->item_id, iptr->itemname);
    }
    else
    {
        printf("Item id: %d %s within Threshold limits\n",iptr->item_id, iptr->itemname);
    }
}

void fullMartThresholdCheck()
{
    printf("Here's the result of Full Mart Threshold check\n");
    for(int i = 0; i < ARRAYSIZE; i++)
    {
        if (AisleArray[i] != NULL && AisleArray[i]->Bplusroot != NULL)
        {
            DataNode *head = HeadofDoubly(AisleArray[i]->Bplusroot);
            while(head)
            {
                for(int i = 0; i < head->size; i++)
                {
                    ThresholdCheck(head->ItemArr[i]);
                }
                head = head->next;
            }
        }
    }
}

int isWithinOneWeek(date *today, date *otherDate)
{
    // Convert both dates to days since some fixed reference date
    int todayDays = today->day + today->month * 30 + today->year * 360;
    int otherDays = otherDate->day + otherDate->month * 30 + otherDate->year * 360;

    // Check if otherDate is within 7 days after today
    return (otherDays - todayDays <= 7 && otherDays - todayDays >= 0);
}

void Expiresin1week_ReadyToeat_itemsPrint(int DD, int MM, int YY)
{
    date *today = CreateDate(DD, MM, YY);
    Item *iptr;
    if (AisleArray[3] != NULL && AisleArray[3]->Bplusroot != NULL)
    {
        printf("\nFollowing Ready to eat items are going to expire within a week\n");
        DataNode *head = HeadofDoubly(AisleArray[3]->Bplusroot);
        while(head)
        {
            for(int i = 0; i < head->size; i++)
            {
                iptr = head->ItemArr[i];
                if (isWithinOneWeek(today, iptr->expiry_date))
                {
                    printf("Name of item:%s\tExpiry: %d/%d/%d\n", iptr->itemname, iptr->expiry_date->day, iptr->expiry_date->month, iptr->expiry_date->year);
                }
            }
            head = head->next;
        }
    }
}

void RangeSearchinAisle(int itemid1, int itemid2, int aisleno)
{
    DataNode *dptr = BplusSearchDatanode(itemid1, AisleArray[aisleno]->Bplusroot);
    Item *iptr;

    while(dptr)
    {
        for(int i = 0; i < dptr->size; i++)
        {
            if(dptr->ItemArr[i]->item_id > itemid2)
            {
                return;
            }
            else if(dptr->ItemArr[i]->item_id >= itemid1)
            {
                iptr = dptr->ItemArr[i];
                printf("id: %d Name of item:%s\tExpiry: %d/%d/%d\n",iptr->item_id, iptr->itemname, iptr->expiry_date->day, iptr->expiry_date->month, iptr->expiry_date->year);
            }
        }
        dptr = dptr->next;
    }
}

void RangeSearchinMart(int itemid1, int itemid2)
{
    printf("\nRange Search: item id %d to item id %d\n", itemid1, itemid2);
    for(int i = 0; i < ARRAYSIZE; i++)
    {
        if (AisleArray[i] != NULL && AisleArray[i]->Bplusroot != NULL)
        {
            RangeSearchinAisle(itemid1, itemid2, i);
        }
    }
}

void CreateMart(char *filename)
{
    initializeAisleArray();
    int i, j, martSize, aisleSize;
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fscanf(file, "%d\n", &martSize);
    for (i = 0; i < martSize; i++)
    {
        char aislename[50];
        fgets(aislename, sizeof(aislename), file);
        fscanf(file, "%d\n", &aisleSize);
        AisleArray[i] = CreateAisle(aislename, i);

        for (j = 0; j < aisleSize; j++)
        {
            int DD, MM, YY, id, quan, tq;
            char name[50];
            fscanf(file, "%d,%49[^,],%d,%d,%d,%d,%d\n", &id, name, &quan, &DD, &MM, &YY, &tq);
            AddOrUpdateItem(id, name, quan, DD, MM, YY, tq, i);
        }
    }
}

Bill* createBillNode(int quantities[]) {
    Bill* newBill = (Bill*)malloc(sizeof(Bill));
    if (newBill != NULL) {
        memcpy(newBill->quantities, quantities, ITEMSIZE * sizeof(int));
        newBill->left = NULL;
        newBill->right = NULL;
    }
    return newBill;
}

Bill* insertBill(Bill* root, Bill* newBill) {
    if (root == NULL) {
        return newBill;
    }

    // Insert into left subtree if quantity is smaller
    if (root->quantities[0] < newBill->quantities[0]) {
        root->left = insertBill(root->left, newBill);
    }
    // Insert into right subtree if quantity is larger
    else {
        root->right = insertBill(root->right, newBill);
    }

    return root;
}

Bill* buildBillTreeFromCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    Bill* root = NULL;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int quantities[ITEMSIZE];
        for (int i = 0; i < ITEMSIZE && token != NULL; i++) {
            quantities[i] = atoi(token);
            token = strtok(NULL, ",");
        }
        Bill* newBill = createBillNode(quantities);
        if (newBill != NULL) {
            root = insertBill(root, newBill);
        }
    }

    fclose(file);
    return root;
}

void printBill(Bill* bill) {
    printf("\n------------BILL--------------\n");
    for (int i = 0; i < ITEMSIZE; i++)
    {
        if (bill->quantities[i] != 0)
            printf("item id: %d\tquantity:%d\n", i, bill->quantities[i]);
    }
}

// Function to print all bills in a binary tree
void printAllBills(Bill* root) {
    if (root == NULL) {
        return;
    }

    printAllBills(root->left);
    printBill(root);
    printAllBills(root->right);
}

void countCoOccurrence(Bill* root, int item_id, int itemCount[]) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < ITEMSIZE; i++) {
        if (root->quantities[i] > 0) {
            itemCount[i]++;
        }
    }

    countCoOccurrence(root->left, item_id, itemCount);
    countCoOccurrence(root->right, item_id, itemCount);
}

void findTopRelatedItems(int item_id, Bill* root) {
    if (root == NULL) {
        return;
    }

    int itemCount[ITEMSIZE] = {0}; // Initialize array to count co-occurrence of items
    countCoOccurrence(root, item_id, itemCount);

    // Find top 3 items
    printf("\nTop 2 items often bought with Item %d:\n", item_id);
    for (int i = 0; i < 2; i++) {
        int maxCount = 0;
        int maxIndex = -1;
        for (int j = 0; j < ITEMSIZE; j++) {
            if (itemCount[j] > maxCount && j != item_id) {
                maxCount = itemCount[j];
                maxIndex = j;
            }
        }
        if (maxIndex != -1) {
            printf("item id %d\n", maxIndex);
            itemCount[maxIndex] = 0; // Set count to 0 to avoid selecting it again
        } else {
            break; // No more items found
        }
    }
}

void MergeTwoAisles(int a_no1, int a_no2)
{
    if (AisleArray[a_no2] == NULL)
    {
        printf("No Aisle with Aisle no %d\n", a_no2);
        return;
    }
    if (AisleArray[a_no1] == NULL)
    {
        printf("No Aisle with Aisle no %d\n", a_no1);
        return;
    }
    DataNode *head = HeadofDoubly(AisleArray[a_no2]->Bplusroot);
    while(head)
    {
        Item *ptr;
        for(int i = 0; i < head->size; i++)
        {
            ptr = head->ItemArr[i];
            AisleArray[a_no1]->Bplusroot = Insert(AisleArray[a_no1]->Bplusroot, head->ItemArr[i], AisleArray[a_no1]->Bplusroot);
        }
        head = head->next;
    }
    //call Delete Aisle
    AisleArray[a_no2] = NULL;
    printf("Aisles successfully Merged\n");
}

Bill* Purchase(Bill *Billbook)
{
    int flag, itemid, DD, MM, YY, quan, i, searchflag;
    Item *iptr;

    flag = 0;
    //Default dates
    DD = 7;
    MM = 4;
    YY = 24;
    date *d = CreateDate(DD, MM, YY);
    printf("Welcome to my Walmart\n");
    int quantities[ITEMSIZE] = {0};

    while (flag == 0)
    {
        printf("Enter itemid to purchase: (Enter -1 to stop purchasing)\n");
        scanf("%d", &itemid);

        if (itemid != -1)
        {
            printf("Enter quantity required\n");
            scanf("%d", &quan);
            Item *iptr = isItemAvailable(itemid, quan, DD, MM, YY);

            if (iptr)
            {
                iptr->quantity -= quan;
                if(iptr->quantity == 0) RemoveItem(iptr->item_id); 
                else
                {
                    ThresholdCheck(iptr);
                }
                quantities[itemid] += quan;
            }
            else
            {
                printf("Sorry item is unavailable\n");
            }
        }
        else
        {
            flag = 1;
        }
    }
    Bill *newbill = createBillNode(quantities);
    Billbook = insertBill(Billbook, newbill);
    printBill(newbill);
    printf("Thankyou For Shopping\n");
    return Billbook;
}



int main()
{
    CreateMart("walmart.csv");
    Bill *BillBook = buildBillTreeFromCSV("billbook.csv");
    int n;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("Press 1 if you are a Customer and Press 2 if you are the Owner: ");
    scanf("%d", &n);
    printf("\n");
    printf("\3 \3 \3 \3 \3 \3 MENU \3 \3 \3 \3 \3 \3\n");
    if(n == 1)
    {
        int m;
        do
        {
            printf("1.Know Your Mart\n2.Purchase Item\n3.Print Bills\n4.Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &m);
            printf("\n");
            if(m == 1) PrintMart();
            else if(m == 2) 
            {
                BillBook = Purchase(BillBook);
            }
            else if( m == 3) printAllBills(BillBook);
            else if(m != 4) printf("Error, Try agaain\n");
        }while(m != 4);
    }
    else if(n == 2)
    {
        int m;
        do
        {
            printf("1.Add/Update Item\n2.Add/Update Aisle\n3.Delete Aisle\n4.Check Availibility\n5.Threshold Check\n6.Check Ready to Eat items, expiring within one week\n7.Merge Aisles\n8.Most Often Bought With Items\n9.Search Items within said range\n10.Print Your Mart\n11.PrintBills\n12.Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &m);
            printf("\n");
            if(m == 1)
            {
                printf("Enter ID, name, quantity to be added, Expiry DD, Expiry MM, Expiry YY, Threshold Quantity: ");
                int id,quan, dd, mm,yy,tq,a_no;
                char name[50];
                scanf("%d %s %d %d %d %d %d",&id,name, &quan, &dd,&mm,&yy,&tq);
                printf("\nEnter Aisle No.: ");
                scanf("%d", &a_no);
                printf("\n");
                AddOrUpdateItem(id,name,quan,dd,mm,yy,tq,a_no);
            }
            else if(m == 2)
            {
                char name[50];
                int a_no;
                printf("Enter name of aisle and Aisle no. : ");
                scanf("%s %d",name, &a_no);
                printf("\n");
                UpdateAisle(name, a_no);
            }
            else if(m == 3)
            {
                int a_no;
                printf("Aisle No. to be deleted: ");
                scanf("%d", &a_no);
                printf("\n");
                DeleteAisle(a_no);
            }
            else if(m == 4)
            {
                printf("Enter Item Id to check, Quantity required, required expiry DD, MM, YY: ");
                int id, quan, dd, mm,yy;
                scanf("%d %d %d %d %d", &id,&quan,&dd,&mm,&yy);
                printf("\n");
                isItemAvailable(id, quan,dd,mm,yy);
            }
            else if(m == 5)
            {
                fullMartThresholdCheck();
            }
            else if(m == 6)
            {
                int dd,mm,yy;
                printf("Enter expiry dd, mm, yy: ");
                scanf(" %d %d %d",&dd,&mm,&yy);
                printf("\n");
                Expiresin1week_ReadyToeat_itemsPrint(dd,mm,yy);
            }
            else if(m == 7)
            {
                int a1,a2;
                printf("Enter the two aisle nos to merge: ");
                scanf(" %d %d", &a1, &a2);
                printf("\n");
                MergeTwoAisles(a1,a2);
            }
            else if(m == 8)
            {
                int x;
                printf("Enter Item ID to search with: ");
                scanf("%d", &x);
                printf("\n");
                findTopRelatedItems(x, BillBook);
            }
            else if(m == 9)
            {
                int id1, id2;
                printf("Enter the lower and bigger item id no. to search between: ");
                scanf("%d %d",&id1, &id2);
                printf("\n");
                RangeSearchinMart(id1,id2);
            }
            else if(m == 10)
            {
                PrintMart();
            }
            else if(m == 11)
            {
                printAllBills(BillBook);
            }
            else if(m != 12)
            {
                printf("Invalid Input, Try again\n");
            }

        } while (m !=12);       
    }
    return 0;
}
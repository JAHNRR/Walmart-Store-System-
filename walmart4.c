#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRAYSIZE 50
#define ITEMSIZE 50

typedef struct date
{
    int day;
    int month;
    int year; // in 2 dig
} date;

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

typedef struct Aisle_tag // for one ailse in the walmart
{
    int aisle_no; // starts from 1
    char *aisle_name;
    Item *head;
} Aisle_tag;

Aisle_tag *AisleArray[ARRAYSIZE]; // indices are array nos

typedef struct Bill
{
    int Quantities[ITEMSIZE]; // indices represent item ids
    struct Bill *next;
} Bill;

Bill *BillListHead = NULL;

void InitializeBill(Bill *b)
{
    int i;
    for (i = 0; i < ITEMSIZE; i++)
    {
        b->Quantities[i] = 0;
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

Aisle_tag *CreateAisle(char *name, int a_no)
{
    Aisle_tag *aptr = (Aisle_tag *)malloc(sizeof(Aisle_tag));
    aptr->aisle_name = (char *)malloc(strlen(name) + 1);
    strcpy(aptr->aisle_name, name);
    aptr->head = NULL;
    aptr->aisle_no = a_no;
    return aptr;
};

date *CreateDate(int DD, int MM, int YY)
{
    date *newDate = (date *)malloc(sizeof(date));
    newDate->day = DD;
    newDate->month = MM;
    newDate->year = YY;
    return newDate;
}

void initializeAisleArray()
{
    int i;
    for (i = 0; i < ARRAYSIZE; i++)
    {
        AisleArray[i] = NULL;
    }
}

void FreeItem(Item *iptr) // frees
{
    free(iptr->itemname);
    free(iptr->expiry_date);
    free(iptr);
}

void AddOrUpdateItem(int id, char *name, int quan, int DD, int MM, int YY, int tq, int aisle_no) // To add item with item ID id in the aisleno aisle_no
{
    date *d, *olddate;
    d = CreateDate(DD, MM, YY);

    Item *iptr = CreateItem(id, name, quan, d, tq);

    if (AisleArray[aisle_no] == NULL)
    {
        char *Aislename;
        printf("No Aisle with this no, please enter Aisle name to create Aisle\n");
        scanf("%s", Aislename);
        AisleArray[aisle_no] = CreateAisle(Aislename, aisle_no);
        AisleArray[aisle_no]->head = iptr;
        printf("Item Successfully Added\n");
    }
    else
    {
        Item *ptr = AisleArray[aisle_no]->head;
        if (ptr == NULL)
        {
            printf("Aisle is Empty\n");
            AisleArray[aisle_no]->head = iptr;
            printf("This is a new item, adding the item, Addition successful\n");
        }
        else
        {
            Item *prev = NULL;
            while (ptr != NULL && ptr->item_id < id)
            {
                prev = ptr;
                ptr = ptr->next;
            }
            if (ptr == NULL)
            {
                printf("This is a new item, adding the item!\n");
                prev->next = iptr;
                iptr->prev = prev;
                printf("Addition successful\n");
            }
            else
            {
                if (ptr->item_id > id)
                {
                    iptr->next = ptr;
                    ptr->prev = iptr;
                    if (prev == NULL)
                    {
                        AisleArray[aisle_no]->head = iptr;
                    }
                    else
                    {
                        iptr->prev = prev;
                        prev->next = iptr;
                    }
                }
                else // ptr->item_id == id
                {
                    while (ptr != NULL && ptr->item_id == id && !(ptr->expiry_date->year == d->year && ptr->expiry_date->month == d->month && ptr->expiry_date->day == d->day))
                    {
                        prev = ptr;
                        ptr = ptr->next;
                    }
                    if (ptr == NULL)
                    {
                        printf("This is a new item, adding the item!\n");
                        prev->next = iptr;
                        iptr->prev = prev;
                        printf("Addition successful\n");
                    }
                    else
                    {
                        if (ptr->item_id == id) // expiry dates are same and item id is same
                        {
                            printf("The item already exists\n");
                            ptr->quantity += quan;
                            FreeItem(iptr);
                            printf("Addition successful!\n");
                        }
                        else
                        {
                            printf("This is a new item, adding item!\n");
                            ptr->prev = iptr;
                            iptr->next = ptr;
                            iptr->prev = prev;
                            if (prev == NULL)
                            {
                                (AisleArray[aisle_no])->head = iptr;
                            }
                            else
                                prev->next = iptr;
                            printf("Addition Successful!\n");
                        }
                    }
                }
            }
        }
    }
}


void DeleteItemId(int itemid)   //O(n)
{
    int i, flag;
    Item *iptr;

    flag = 0;

    for (i = 0; i < ARRAYSIZE && flag == 0; i++)    //flag
    {
        if (AisleArray[i] != NULL && AisleArray[i]->head != NULL)
        {
            iptr = AisleArray[i]->head;
            while (iptr != NULL && iptr->item_id != itemid)
            {
                iptr = iptr->next;
            }
            if (iptr != NULL)
            {
                if(iptr->prev != NULL)
                    iptr->prev->next = iptr->next;
                else
                    AisleArray[i]->head = iptr->next;
                if (iptr->next != NULL)
                    iptr->next->prev = iptr->prev;
                FreeItem(iptr); // Frees    //Prevents memory leak
                flag = 1;
            }
        }
    }
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
        printf("Given Aisle no. does not exist\n");
}

void DeleteAisle(int a_no)
{
    if (AisleArray[a_no] != NULL)
    {
        Item *iptr, *ptr;
        while (iptr != NULL)
        {
            ptr = iptr;
            iptr = iptr->next;
            FreeItem(ptr);  //Prevents memory leak
        }
        AisleArray[a_no] = NULL;
    }
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

int isItemAvailable(int id, int qty_req, int DD, int MM, int YY)
{
    date *d = CreateDate(DD, MM, YY);
    int i, retval, flag;
    retval = flag = 0;

    Item *iptr;

    for (i = 0; i < ARRAYSIZE && flag == 0; i++)
    {
        if (AisleArray[i] != NULL && AisleArray[i]->head != NULL)
        {
            iptr = AisleArray[i]->head;
            while (iptr != NULL && iptr->item_id != id)
            {
                iptr = iptr->next;
            }
            if (iptr != NULL)
            {
                if (iptr->quantity >= qty_req && !isExpired(iptr->expiry_date, d))
                {
                    retval = 1;
                    flag = 1;
                }
            }
        }
    }
    if (retval)
    {
        printf("Item is available\n");
    }
    else
    {
        printf("Item is unavailable\n");
    }
    return retval;
}

int isWithinOneWeek(date *today, date *otherDate)
{
    // Convert both dates to days since some fixed reference date
    int todayDays = today->day + today->month * 30 + today->year * 360;
    int otherDays = otherDate->day + otherDate->month * 30 + otherDate->year * 360;

    // Check if otherDate is within 7 days after today
    return (otherDays - todayDays <= 7);
}

date *dateOneDayLater(date *inputDate)
{
    date *nextDay = (date *)malloc(sizeof(date));

    // Increment day by one
    nextDay->day = inputDate->day + 1;
    nextDay->month = inputDate->month;
    nextDay->year = inputDate->year;

    // Adjust if day exceeds 30 (assuming all months have 30 days)
    if (nextDay->day > 30)
    {
        nextDay->day = 1;
        nextDay->month++;

        // Adjust if month exceeds 12
        if (nextDay->month > 12)
        {
            nextDay->month = 1;
            nextDay->year++; // Increment year if month exceeds 12
        }
    }
    return nextDay;
}

void Expiresin1week_ReadyToeat_itemsPrint(int DD, int MM, int YY)
{
    date *today = CreateDate(DD, MM, YY);
    Item *iptr;
    if (AisleArray[3] != NULL && AisleArray[3]->head != NULL)
    {
        printf("Following Ready to eat items are going to expire within a week\n");
        iptr = AisleArray[3]->head;
        while (iptr != NULL)
        {
            if (isWithinOneWeek(today, iptr->expiry_date))
            {
                printf("Name of item:%s\tExpiry: %d/%d/%d\n", iptr->itemname, iptr->expiry_date->day, iptr->expiry_date->month, iptr->expiry_date->year);
            }
            iptr = iptr->next;
        }
    }
}

void Expiresin1day_dairy_itemsPrint(int DD, int MM, int YY)
{
    date *today = CreateDate(DD, MM, YY);
    date *target = dateOneDayLater(today);
    int i;
    Item *iptr;

    printf("Printing dairy items that'll expire within one day\n");

    if (AisleArray[0] != NULL && AisleArray[0]->head != NULL)
    {
        iptr = AisleArray[0]->head;
        while (iptr != NULL)
        {
            if (iptr->expiry_date->day == target->day && iptr->expiry_date->month == target->month && iptr->expiry_date->year == target->year)
            {
                printf("Name of item:%s\tExpiry: %d/%d/%d\n", iptr->itemname, iptr->expiry_date->day, iptr->expiry_date->month, iptr->expiry_date->year);
            }
            iptr = iptr->next;
        }
    }
}

void ThresholdCheck(Item *iptr) // To be called at the time of purchase
{
    if (iptr->quantity < iptr->threshold_quantity)
    {
        printf("Item has gone beyond Threshold\n");
    }
    else
    {
        printf("Item within Threshold limits\n");
    }
}

void MergeAisles(int a1, int a2) // Aisle no a2 will be merged with Aisle no a1, aisle no a2 will be empty
{
    if (AisleArray[a2] == NULL)
    {
        printf("No Aisle with Aisle no %d\n", a2);
        return;
    }
    if (AisleArray[a1] == NULL)
    {
        printf("No Aisle with Aisle no %d\n", a1);
        return;
    }
    else
    {
        if (AisleArray[a1]->head == NULL)
        {
            AisleArray[a1]->head = AisleArray[a2]->head;
            printf("Aisles Merged Successfully\n");
        }
        else
        {
            Item *ptr1 = AisleArray[a1]->head, *ptr2 = AisleArray[a2]->head, *ptr;
            if(ptr1->item_id >ptr2->item_id)
            {
                AisleArray[a1]->head = ptr2;
                ptr2 = ptr2->next;
            }
            else ptr1 = ptr1->next;
            ptr = AisleArray[a1]->head;
            while(ptr1 != NULL && ptr2 != NULL)
            {
                if(ptr1->item_id < ptr2->item_id)
                {
                    ptr->next = ptr1;
                    ptr1->prev = ptr;
                    ptr1 = ptr1->next;
                }
                else
                {
                    ptr->next = ptr2;
                    ptr2->prev = ptr;
                    ptr2 = ptr2->next;
                }
                ptr = ptr->next;
            }
            if(ptr1 != NULL)
            {
                ptr1->prev = ptr;
                ptr->prev = ptr1;
            }
            if(ptr2 != NULL)
            {
                ptr2->prev = ptr;
                ptr->next = ptr2;
            }
            printf("Aisles Merged Successfully\n");
        }
        AisleArray[a2] = NULL;
    }
}

void PrintMart()
{
    int i;
    Item *ptr;

    for (i = 0; i < ARRAYSIZE; i++)
    {
        if (AisleArray[i] != NULL)
        {
            printf("\nAisle no = %d  Aisle name: %s \n", AisleArray[i]->aisle_no, AisleArray[i]->aisle_name);
            ptr = AisleArray[i]->head;
            while (ptr != NULL)
            {
                printf("Item ID: %d\t\t\tItem name: %s\t\t\tQuantity: %d\t\t\tThreshold qty: %d\t\t\texpiry date: %d/%d/%d\n", ptr->item_id, ptr->itemname, ptr->quantity, ptr->threshold_quantity, ptr->expiry_date->day, ptr->expiry_date->month, ptr->expiry_date->year);
                ptr = ptr->next;
            }
            printf("\n");
        }
    }
}

void CreateMart(char *filename)
{
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

        Item *last = NULL;
        for (j = 0; j < aisleSize; j++)
        {
            int DD, MM, YY, id, quan, tq;
            char name[50];

            fscanf(file, "%d,%49[^,],%d,%d,%d,%d,%d\n", &id, name, &quan, &DD, &MM, &YY, &tq);

            date *d = CreateDate(DD, MM, YY);
            //Item *iptr = CreateItem(id, name, quan, d, tq);
            AddOrUpdateItem(id, name, quan, DD, MM, YY, tq, i);
            //Item *ptr = iptr;

            // if (j == 0)
            // {
            //     AisleArray[i]->head = iptr;
            //     last = iptr;
            //     last->next = NULL;
            //     last->prev = NULL;
            // }
            // else
            // {
            //     last->next = iptr;
            //     iptr->prev = last;
            //     last = iptr;
            // }
        }
    }
}

void CreateBillList(char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    int i, j;
    Bill *billlast = NULL; // Initialize billlast to NULL

    for (i = 0; i < 5; i++)
    {
        Bill *b = (Bill *)malloc(sizeof(Bill));

        InitializeBill(b);

        if (i == 0)
        {
            BillListHead = billlast = b;
        }
        else
        {
            billlast->next = b;
            billlast = b;
        }

        for (j = 0; j < 10; j++)
        {
            int index, qty;
            fscanf(file, "%d,%d\n", &index, &qty);
            b->Quantities[index] += qty;
        }
        b->next = NULL; // Set the next pointer of the last bill to NULL
    }

    fclose(file);
}

void PrintBills()
{
    Bill *b = BillListHead;
    int i;

    while (b != NULL)
    {
        printf("\n--------------------------BILL----------------------------\n");
        for (i = 0; i < ITEMSIZE; i++)
        {
            if (b->Quantities[i] != 0)
                printf("item id: %d\tquantity:%d\n", i, b->Quantities[i]);
        }
        b = b->next;
    }
}

void PrintTop3indices(int arr[], int size)
{
    int largest = 0;
    int secondLargest = -1;
    int thirdLargest = -1;

    for (int i = 1; i < size; i++)
    {
        if (arr[i] > arr[largest])
        {
            thirdLargest = secondLargest;
            secondLargest = largest;
            largest = i;
        }
        else if (arr[i] > arr[secondLargest] && arr[i] != arr[largest])
        {
            thirdLargest = secondLargest;
            secondLargest = i;
        }
        else if (arr[i] > arr[thirdLargest] && arr[i] != arr[secondLargest] && arr[i] != arr[largest])
        {
            thirdLargest = i;
        }
    }

    printf("Item number: %d  freq = %d\n", largest, arr[largest]);
    printf("Item number: %d  freq = %d\n", secondLargest, arr[secondLargest]);
    printf("Item number: %d  freq = %d\n", thirdLargest, arr[thirdLargest]);
}

void OftenBoughtWith(int itemid)
{
    Bill *b = BillListHead;
    int i;
    int count[ITEMSIZE] = {0};

    while (b != NULL)
    {
        if (b->Quantities[itemid] > 0)
        {
            for (i = 0; i < ITEMSIZE; i++)
            {
                if (i != itemid)
                {
                    count[i] += b->Quantities[i];
                }
            }
        }
        b = b->next;
    }

    for (i = 1; i < ITEMSIZE; i++)
    {
        printf("itemid = %d\tcount = %d\n", i, count[i]);
    }

    printf("Item id %d is often bought with:\n", itemid);
    PrintTop3indices(count, ITEMSIZE);
}

void Purchase()
{
    int flag, itemid, DD, MM, YY, quan, i, searchflag;
    Item *iptr;

    flag = 0;
    DD = 7;
    MM = 4;
    YY = 24;

    printf("Welcome to my Walmart\n");
    date *d = CreateDate(DD, MM, YY);
    Bill *newbill = NULL;

    while (flag == 0)
    {
        printf("Enter itemid to purchase: (Enter -1 to stop purchasing)\n");
        scanf("%d", &itemid);

        if (itemid != -1)
        {
            printf("Enter quantity required\n");
            scanf("%d", &quan);

            if (isItemAvailable(itemid, quan, DD, MM, YY))
            {
                searchflag = 0;
                for (i = 0; i < ARRAYSIZE && searchflag == 0; i++)
                {
                    if (AisleArray[i] != NULL && AisleArray[i]->head != NULL)
                    {
                        iptr = AisleArray[i]->head;
                        while (iptr != NULL && iptr->item_id != itemid)
                        {
                            iptr = iptr->next;
                        }
                        if (iptr != NULL)
                        {
                            iptr->quantity -= quan;
                            ThresholdCheck(iptr);
                            searchflag = 1;
                        }
                    }
                }
                if (newbill == NULL)
                {
                    newbill = (Bill *)malloc(sizeof(Bill));
                    InitializeBill(newbill);
                }
                newbill->Quantities[itemid] += quan;
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

    if (newbill)
    {
        newbill->next = BillListHead;
        BillListHead = newbill;
    }
    printf("Thankyou For Shopping\n");
}

void main()
{
    initializeAisleArray();
    CreateMart("walmart6.csv");
    CreateBillList("bill.csv");
    
    PrintMart();
    //Purchase();
    //PrintMart();
    //PrintBills();
    //Expiresin1week_ReadyToeat_itemsPrint(9,5,24);
    Expiresin1day_dairy_itemsPrint(9,5,24);
    //MergeAisles(2,3);
    //AddOrUpdateItem(6,"Lentils",10,10,5,24,10,1);
    //PrintMart();
}

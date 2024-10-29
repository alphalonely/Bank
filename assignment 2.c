#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

//customers linked list
struct customer {
    char customer_ID[12];
    char first_name[100];
    char last_name[100];
    double balance;
    char secret_password[100];
    char transaction_ID[12];
    struct customer *next;
};

struct customer_list {
    struct customer *front;
};
//add new customer
void generate_ID(char *id) {
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 5; ++i) {
        id[i] = alphabet[rand() % 26];
    }
    id[5] = '-';
    for (int i = 6; i < 11; ++i) {
        id[i] = rand() % 10 + '0';
    }
}

void add(struct customer_list *list, char *first_name, char *last_name, double balance, char *secret_password) {
    struct customer *new_customer = malloc(sizeof(struct customer));
    generate_ID(new_customer->customer_ID);
    strcpy(new_customer->first_name, first_name);
    strcpy(new_customer->last_name, last_name);
    new_customer->balance = balance;
    strcpy(new_customer->secret_password, secret_password);
    generate_ID(new_customer->transaction_ID);

    new_customer->next = NULL;
    if (list->front == NULL) {
        list->front = new_customer;
    } else {
        struct customer *temp = list->front;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_customer;
    }
    printf("Customer Successfully Saved with customer ID: %s\n", new_customer->customer_ID);
}

//delete customer
void delete(struct customer_list *list, char *customer_ID, char *secret_password) {
    struct customer *temp = list->front;
    struct customer *before = NULL;

    while (temp != NULL && strcmp(temp->customer_ID, customer_ID) != 0) {
        before = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Customer not found\n");
        return;
    }
    if (strcmp(temp->secret_password, secret_password) != 0) {
        printf("Incorrect password\n");
        return;
    }
    if (temp->balance != 0) {
        printf("Balance is not 0. Cannot delete\n");
        return;
    }
    if (before == NULL) {
        list->front = temp->next;
    } else {
        before->next = temp->next;
    }
    printf("Customer account for %s %s has been successfully deleted.\n", temp->first_name, temp->last_name);
    free(temp);
}

//display customer
void display(struct customer_list *list, char *customer_ID) {
    struct customer *temp = list->front;
    while (temp != NULL && strcmp(temp->customer_ID, customer_ID) != 0) {
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("Customer not found\n");
        return;
    }
    printf("Customer details: \n");
    printf("Name: %s %s\n", temp->first_name, temp->last_name);
    printf("Balance: %.2lf\n", temp->balance);
    printf("Customer ID: %s\n", temp->customer_ID);
    printf("Transaction ID: %s\n", temp->transaction_ID);
}
//block of transactions linked list
struct transaction {
    char source_trans_ID[12];
    double amount_sent;
    char dest_trans_ID[12];
};

struct block {
    struct transaction trans_block[5];
    int amount_of_trans;
    struct block *next;
};

struct block_list {
    struct block *front;
};
//check password
bool check(struct customer_list *list, char *customer_ID, char *secret_password) {
    struct customer *temp = list->front;
    while (temp != NULL && strcmp(temp->customer_ID, customer_ID) != 0) {
        temp = temp->next;
    }
    if (temp == NULL) {
        return false;
    }
    return strcmp(temp->secret_password, secret_password) == 0;
}
//get balance
void get_balance(struct customer_list *cus_list, struct block *block) {
    struct customer *cus;
    struct transaction *trans;
    
    for (int i = 0; i < block->amount_of_trans; ++i) {
        trans = &block->trans_block[i];
        
        if (strcmp(trans->source_trans_ID, "Deposit") != 0) {
            cus = cus_list->front;
            while (cus != NULL && strcmp(cus->transaction_ID, trans->source_trans_ID) != 0) {
                cus = cus->next;
            }
            if (cus != NULL) {
                cus->balance -= trans->amount_sent;
            }
        }
        
        cus = cus_list->front;
        while (cus != NULL && strcmp(cus->transaction_ID, trans->dest_trans_ID) != 0) {
            cus = cus->next;
        }
        if (cus != NULL) {
            cus->balance += trans->amount_sent;
        }
    }
}

//insert block of transactions
void insert_block(struct block_list *block_list, struct customer_list *cus_list, struct transaction transactions[], int count, char *passwords[]) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(transactions[i].source_trans_ID, "Deposit") != 0) {
            if (check(cus_list, transactions[i].source_trans_ID, passwords[i]) == false) {
                printf("Password check failed for transaction %d\n", i+1);
                return;
            }
        }
    }
    struct block *new_block = malloc(sizeof(struct block));
    if (new_block == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    for (int i = 0; i < count; ++i) {
        new_block->trans_block[i] = transactions[i];
    }
    new_block->amount_of_trans = count;
    new_block->next = NULL;

    if (block_list->front == NULL) {
        block_list->front = new_block;
    } else {
        struct block *temp = block_list->front;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_block;
    }
    get_balance(cus_list, new_block);
}


int main() {
    struct customer_list customer_list = {NULL};
    struct block_list block_list = {NULL};
    char customer_ID[12];
    char first_name[100];
    char last_name[100];
    double balance;
    char secret_password[100];
    char confirm_password[100];
    int option;
    struct transaction transactions[5];
    char *passwords[5] = {NULL};

    srand(time(0));
    while (1) {
        printf("Bank Menu:\n");
        printf("1 - Create new customer\n");
        printf("2 - Delete customer\n");
        printf("3 - Display customer\n");
        printf("4 - Insert transactions\n");
        printf("5 - Exit\n");
        printf("Please select an option: ");
        scanf("%d", &option);
        getchar();
        
        switch (option) {
            case 1:
                printf("Please enter the customer information:\n");
                printf("First name: ");
                scanf("%s", first_name);
                printf("Last name: ");
                scanf("%s", last_name);
                printf("Password: ");
                scanf("%s", secret_password);
                printf("Confirm Password: ");
                scanf("%s", confirm_password);
                if (strcmp(secret_password, confirm_password) != 0) {
                    printf("Password doesn't match\n");
                    break;
                }
                balance = 0.0;
                add(&customer_list, first_name, last_name, balance, secret_password);
                break;
            case 2:
                printf("Please enter the customer ID: ");
                scanf("%s", customer_ID);
                printf("Please enter password: ");
                scanf("%s", secret_password);
                delete(&customer_list, customer_ID, secret_password);
                break;
            case 3:
                printf("Please enter the customer ID: ");
                scanf("%s", customer_ID);
                display(&customer_list, customer_ID);
                break;
            case 4:
                {
                    int i;
                    for (i = 0; i < 5; ++i) {
                        printf("Please insert transactions:\n");
                        printf("Source: ");
                        scanf("%s", transactions[i].source_trans_ID);
                        if (strcmp(transactions[i].source_trans_ID, "[EOF]") == 0) {
                            break;
                        }
                        printf("Amount: ");
                        scanf("%lf", &transactions[i].amount_sent);
                        printf("Destination: ");
                        scanf("%s", transactions[i].dest_trans_ID);
                        if (strcmp(transactions[i].source_trans_ID, "Deposit") != 0) {
                            passwords[i] = malloc(100 * sizeof(char));
                            printf("Password: ");
                            scanf("%s", passwords[i]);
                        } else {
                            passwords[i] = NULL;
                        }
                    }
                    insert_block(&block_list, &customer_list, transactions, i, passwords);
                    for (int k = 0; k < 5; ++k) {
                        if (passwords[k] != NULL) {
                            free(passwords[k]);
                        }
                    }
                }
                break;
            case 5:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid option\n");
                break;
        }
    }
    return 0;
}
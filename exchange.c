/*	Author: Joshua DeLawter-Gourlay
	Program: Single Item Exchange Simulator
	Description: The purpose of this program is to successfully
    facilitate the buy and sell orders of a single product through
    maintaining and updating buy and sell order stack data structures.
    In the future, the plan is to integrate it with a database and
	expand it to handle multiple items.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//////////////////////
// data definitions //
//////////////////////

#define NAME_LEN 50

struct orders {
    char username[NAME_LEN + 1];
    double price;
    double quantity;
    double total;
    char type[5];
    struct orders *next;
};


/////////////////////////
// function prototypes //
/////////////////////////

struct orders * create_order(char *type);
struct orders * update_orders(struct orders *orders, struct orders *new_order);
struct orders * clear_up_to_target(struct orders *orders, struct orders *target);
void print_all_orders(struct orders *orders);
void clear_all(struct orders *orders);
int read_line(char str[], int n);
void help();

/**********************************************************
 * main: Prompts the user to enter an operation code,     *
 *       then calls a function to perform the requested   *
 *       action. Repeats until the user enters the        *
 *       command 'q'. Prints an error message if the user *
 *       enters an illegal code.                          *
 **********************************************************/
int main() {
	char code;
    int choice;

	struct orders *buy_orders = NULL;
    struct orders *sell_orders = NULL;
    struct orders *new_order = NULL;

	help();
	printf("\n");

	for (;;) {
		// read operation code
		printf("Enter operation code: ");
		scanf(" %c", &code);
		while(getchar() != '\n') /* skips to end of line */
			;

		// run operation
		switch (code) {
			case 'b':
                new_order = create_order("buy");
                sell_orders = clear_up_to_target(sell_orders, new_order);
				buy_orders = update_orders(buy_orders, new_order);
				break;
            case 's':
                new_order = create_order("sell");
                buy_orders = clear_up_to_target(buy_orders, new_order);
				sell_orders = update_orders(sell_orders, new_order);
				break;
            case 'p':
                printf("\n");
                printf("Would you like to display the buy orders or sell orders?\n");
                printf("\t1). Buy\n");
                printf("\t2). Sell\n");
                printf("Enter your choice: ");
                scanf("%d", &choice);
                while (choice != 1 && choice != 2) {
                    printf("Invalid choice. Please type 1 or 2.\n");
                    printf("Enter your choice: ");
                    scanf("%d", &choice);
                }
                if (choice == 1) {
                    print_all_orders(buy_orders);
                }
                else {
                    print_all_orders(sell_orders);
                }
				break;
			case 'h':
				help();
				break;
			case 'q':
				clear_all(buy_orders);
                clear_all(sell_orders);
				return 0;
			default:
				printf("Illegal operation code!\n");
		}
		printf("\n");
	}
}

//////////////////////////
// function definitions //
//////////////////////////

void help() {
	printf("List of operation codes:\n");
	printf("\t'b' for adding a buy order;\n");
    printf("\t's' for adding a sell order;\n");
    printf("\t'p' for printing all orders;\n");
	printf("\t'h' for help;\n");
	printf("\t'q' for quit.\n");
}

struct orders * create_order(char *type) {
    struct orders * new_order = malloc(sizeof(struct orders));
    if (new_order == NULL) {
		printf("malloc failed in create_order\n");
		return NULL;
	}
    printf("Enter your username: ");
    read_line(new_order -> username, NAME_LEN);
    printf("Enter your price: ");
    scanf("%lf", &new_order -> price);
    printf("Enter your quantity: ");
    scanf("%lf", &new_order -> quantity);
    new_order -> total = new_order -> price * new_order -> quantity;
    strcpy(new_order -> type, type);
    new_order -> next = NULL;
    return new_order;
}

struct orders * update_orders(struct orders *orders, struct orders *new_order) {
    struct orders *curr;
    struct orders *prev;

    if (orders == NULL) {
        return new_order;
    }
    if (new_order == NULL) {
        return orders;
    }
    if (new_order -> total == 0) {
        return orders;
    }
    if (strcmp(new_order -> type, "buy") == 0) {
        // iterate through buy orders until you find its priority
        // buy orders are listed from highest to lowest
        for (curr = orders, prev = NULL; curr != NULL && new_order -> price <= curr -> price; prev = curr, curr = curr -> next)
            ; // stop when new_order price > curr price
        if (prev == NULL) {
            new_order -> next = curr;
            return new_order;
        }
        else if (curr == NULL) {
            prev -> next = new_order;
            return orders;
        }
        else {
            prev -> next = new_order;
            new_order -> next = curr;
            return orders;
        }
    }
    else { // type == sell
        // iterate through sell orders until you find its priority
        // sell orders are listed from lowest to highest
        for (curr = orders, prev = NULL; curr != NULL && new_order -> price >= curr -> price; prev = curr, curr = curr -> next)
            ; // stop when new order price < curr price
        if (prev == NULL) {
            new_order -> next = curr;
            return new_order;
        }
        else if (curr == NULL) {
            prev -> next = new_order;
            return orders;
        }
        else {
            prev -> next = new_order;
            new_order -> next = curr;
            return orders;
        }
    }
}

struct orders * clear_up_to_target(struct orders *orders, struct orders *target) {
    struct orders *p;
	while (orders != NULL && orders -> price <= target -> price && target -> total != 0) {
        if (target -> total >= orders -> total) {
           // adjust total down by adjusting quantity down and recalculating total
           target -> quantity -= orders -> quantity;
           printf("%s has bought %.2lf @ $%.2lf from %s\n", target -> username, orders -> quantity, orders -> price, orders -> username);
           target -> total = target -> quantity * target -> price;
        }
        else {
            target -> quantity -= target -> total / orders -> price;
            if (target -> quantity < 0) {
                printf("Error, quantity is %lf\n", target -> quantity);
            }
            printf("%s has bought %lf @ $%lf from %s", target -> username, orders -> quantity, orders -> price, orders -> username);
            target -> total = 0;
            break;
        }
        
        // clear after done
		p = orders;
		orders = orders -> next;
		free(p);
	}
    return orders;
}

void print_all_orders(struct orders *orders) {
    struct orders *p;
    printf("\n");
    if (orders == NULL) {
        printf("No orders found");
        return;
    }
    printf("|--------------------------------------|---------|------------|-----------|\n");
	printf("| Username                             |  Price  |  Quantity  |   Total   |\n");
	printf("|--------------------------------------|---------|------------|-----------|\n");
    for (p = orders; p != NULL; p = p -> next) {
        printf("| %-36s | %7.2f | %10.2f | %9.2f |\n", p -> username, p -> price, p -> quantity, p -> total);
    }
    printf("|--------------------------------------|---------|------------|-----------|\n");
}

void clear_all(struct orders *orders) {
    struct orders *p;
	while (orders != NULL) {
		p = orders;
		orders = orders -> next;
        printf("Clearing %s's order\n", p -> username);
		free(p);
	}
}

int read_line(char str[], int n) {
	int ch, i=0;

	while (isspace(ch = getchar()))
		;
	str[i++] = ch;
	while ((ch = getchar()) != '\n') {
		if (i < n)
			str[i++] = ch;    
	}
	str[i] = '\0';
	return i;
}

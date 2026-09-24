#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//Structure

// Struct for storing primary account information.
typedef struct {
    int account_number;
    char name[100];
    char address[200];
    char password[20];
    float balance;
    
} Account;

// Struct for storing individual transaction records.
typedef struct {
    int account_number;
    char date[11];
    char type[10]; // D for Deposit, W for Withdrawal
    char mode[20];   // e.g., "Cash", "Cheque"
    float amount;
    float balance;
} Transaction;

void display_list();
void modify();
void close_account();
void transaction_function();
void update_balance(int acc_num, float new_balance);
void add_account_to_file(Account newAcc);
void new_account();
void box_for_display();
int no_of_days(int,int);
void month_report();
void add_transaction_to_file(Transaction newTransc);
int found_account(int target_account);
double give_balance(int acc_no);
void display_account();
void main_menu();  
void edit_menu(); 
void delete_account();
void clear();
void get_today_date(int *day, int *month, int *year);



int main(){
    printf("Banking system\n");
    main_menu();
    printf("Thank you\n");
    return 0;
}

    void new_account() {
        Transaction banking;
        Account data;
    
        int new_account_number = 100001;
        int last_account_number = 100000;
    
        FILE *fp;
    
    start:
        printf("Enter name:\n");
        fgets(data.name, sizeof(data.name), stdin);
        data.name[strcspn(data.name, "\n")] = 0;
    
        // Validate name (only alphabets and spaces)
        for (int i = 0; data.name[i] != '\0'; i++) {
            if (!((data.name[i] >= 'A' && data.name[i] <= 'Z') ||
                  (data.name[i] >= 'a' && data.name[i] <= 'z') ||
                  data.name[i] == ' ')) {
                printf("Error: Name contains invalid characters. Only alphabets and spaces are allowed.\n");
                goto start;
            }
        }
    
        // Check for duplicate name
        FILE *check_fp = fopen("initial.dat", "rb");
        if (check_fp) {
            Account temp;
            while (fread(&temp, sizeof(Account), 1, check_fp) == 1) {
                if (strcmp(data.name, temp.name) == 0) {
                    fclose(check_fp);
                    printf("Error: Account with the name '%s' already exists.\n", data.name);
                    goto start;
                }
            }
            fclose(check_fp);
        }
    
        // Get address, password, and opening balance
        printf("Enter your address:\n");
        fgets(data.address, sizeof(data.address), stdin);
        data.address[strcspn(data.address, "\n")] = 0;
    
        printf("Create a password (max 20 characters):\n");
        fgets(data.password, sizeof(data.password), stdin);
        data.password[strcspn(data.password, "\n")] = 0;
    
        printf("Enter opening balance amount:\n");
        scanf("%f", &data.balance);
        getchar(); // consume newline
         if (data.balance < 500) {
        printf("Error: Minimum opening balance must be ₹500 or more.\n");
        goto start;
    }
    
         
        // Determine next account number from banking.dat
        fp = fopen("banking.dat", "rb");
        if (fp) {
            Transaction temp_transaction;
            while (fread(&temp_transaction, sizeof(Transaction), 1, fp) == 1) {
                if (temp_transaction.account_number > last_account_number) {
                    last_account_number = temp_transaction.account_number;
                }
            }
            fclose(fp);
        }
    
        new_account_number = last_account_number + 1;
        data.account_number = new_account_number;
    
        banking.account_number = new_account_number;
        banking.balance = data.balance;
    
        // Save to banking.dat (Transaction)
        fp = fopen("banking.dat", "ab");
        if (!fp) {
            printf("Error opening banking.dat\n");
            return;
        }
        fwrite(&banking, sizeof(Transaction), 1, fp);
        fclose(fp);
    
        // Save to initial.dat (Account) using the provided helper
        add_account_to_file(data);
    
        printf("\nAccount created successfully!\n");
        printf("Your account number is: %d\n", data.account_number);
    }


void box_for_display(){
    printf("%-12s","DATE");
    printf("%-18s","PARTICULARS");
    printf("%-12s","DEPOSIT");
    printf("%-14s","WITHDRAWAL");
    printf("%-10s\n","BALANCE");
    printf("--------------------------------------------------------------");
}


int no_of_days(int month, int year) {
    if (month < 1 || month > 12) return -1;

    int month_day[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    // Check for leap year
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        month_day[1] = 29;
    }
    return month_day[month - 1];
}

void month_report() {
    int acc_num, day, month, year;
    char entered_pass[20];
start:
    printf("Enter the account number: ");
    scanf("%d", &acc_num);
    printf("Enter the password: ");
    clear(); // clear input buffer
    scanf("%s", entered_pass);

    // Verify from initial.dat
    FILE *fpa = fopen("initial.dat", "rb");
    if (fpa == NULL) {
        printf("Could not open initial.dat file.\n");
        return;
    }

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, fpa) == 1) {
        if (acc.account_number == acc_num) {
            found = 1;
            if (strcmp(acc.password, entered_pass) != 0) {
                printf("Incorrect password.\n");
                fclose(fpa);
                return;
            }
            break;
        }
    }
    fclose(fpa);

    if (!found) {
        printf("Account number not found.\n");
        goto start;
    }

    // Ask for the month & year
    printf("Enter a date (DD MM YYYY): ");
    scanf("%d %d %d", &day, &month, &year);

    int last_day = no_of_days(month, year);
    if (last_day == -1) {
        printf("Invalid month/year provided.\n");
        return;
    }

    FILE *fp = fopen("banking.dat", "rb");
    if (fp == NULL) {
        printf("Could not open banking.dat file.\n");
        return;
    }

    Transaction trans;
    int t_day, t_month, t_year;
    int header_printed = 0;
    float final_balance = 0.00;
    int match_found = 0;

    while (fread(&trans, sizeof(Transaction), 1, fp) == 1) {
        if (trans.account_number != acc_num) continue;

        char clean_date[15];
        strcpy(clean_date, trans.date);
        clean_date[strcspn(clean_date, "\n")] = '\0';

        if (sscanf(clean_date, "%d-%d-%d", &t_day, &t_month, &t_year) != 3) continue;

        if (t_month == month && t_year == year) {
            if (!header_printed) {
                printf("\n--- Monthly Transaction Report ---\n");
                printf("Date\t\tType\tAmount\tBalance\n");
                header_printed = 1;
            }
            printf("%s\t%s\t%.2f\t%.2f\n", trans.date, trans.type, trans.amount, trans.balance);
            match_found = 1;
        }
    }
    fclose(fp);

    if (!match_found) {
        printf("No transactions found for this month.\n");
    }
}



void add_account_to_file(Account newAcc) {
    FILE *fp = fopen("initial.dat", "ab");     // Opens file in append binary mode
    if (fp == NULL) {
      perror("Error opening file.\n"); 
        return;
    }
    fwrite(&newAcc, sizeof(Account), 1, fp);     // Write the new account to file
    fclose(fp);
}
//

void add_transaction_to_file(Transaction newTransc) {
    FILE *fp = fopen("banking.dat", "ab");
    if (fp == NULL) {
        perror("Error opening file.\n");
        return;
    }
    fwrite(&newTransc, sizeof(Transaction), 1, fp);   // Writes the new transaction to file
    fclose(fp);
}
//

int recordno(int acc_no) {
    FILE *fp = fopen("initial.dat", "rb");   // Opens file in read binary mode
    if (fp == NULL) {
        perror("Error opening file.\n");
        return -1;
    }

    Account newAcc;
    int pos = 0;

    while (fread(&newAcc, sizeof(Account), 1, fp)) {      //compares acc nos.
        if (newAcc.account_number == acc_no) {
            fclose(fp);
            return pos;
        }
        pos++;
    }

    fclose(fp);
    return -1;   //acc not found
}

//
void display_account() {
    int acc_no;
    char entered_password[20];

    printf("Enter account number to display: ");
    scanf("%d", &acc_no);

    printf("Enter password: ");
    scanf("%s", entered_password);

    FILE *fp_acc = fopen("initial.dat", "rb");
    if (!fp_acc) {
        perror("Error opening initial.dat file.\n");
        return;
    }

    Account acc;
    int auth_success = 0;

    while (fread(&acc, sizeof(Account), 1, fp_acc)) {
        if (acc.account_number== acc_no) {
            if (strcmp(acc.password, entered_password) == 0) {
                auth_success = 1;
            }
            break;
        }
    }

    fclose(fp_acc);

    if (!auth_success) {
        printf("Incorrect password or account number.\n");
        return;
    }

    FILE *fp = fopen("banking.dat", "rb");
    if (!fp) {
        perror("Error opening banking.dat file.\n");
        return;
    }

    Transaction trans;
    float total_deposit = 0, total_withdraw = 0;
    int found = 0;
    int dd, mm, yy;

    get_today_date(&dd, &mm, &yy);

    printf("\nAccount Number: %d", acc_no);
    printf("\t\tDate: %02d/%02d/%04d\n", dd, mm, yy);
    printf("-------------------------------------------------------------\n");
   box_for_display();
   printf("\n");

    while (fread(&trans, sizeof(Transaction), 1, fp)) {
        if (trans.account_number == acc_no) {
            found = 1;

            float deposit = 0, withdraw = 0;
            if (trans.type[0] == 'D' || trans.type[0] == 'd') {
                deposit = trans.amount;
                total_deposit += trans.amount;
            } else if (trans.type[0] == 'W' || trans.type[0] == 'w') {
                withdraw = trans.amount;
                total_withdraw += trans.amount;
            }

            printf(" %s %-10s %-10.2f %-10.2f %-10.2f\n",
                   trans.date,
                   trans.type,
                   deposit,
                   withdraw,
                   trans.balance);
        }
    }

    fclose(fp);

    if (!found) {
        printf("No transactions found for Account %d.\n", acc_no);
        return;
    }

    float current_balance = give_balance(acc_no);

    printf("-------------------------------------------------------------\n");
    printf(" TOTAL ->        %-10.2f %-10.2f %-10.2f\n",
           total_deposit, total_withdraw, current_balance);
}
void display_list() {
    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    int acc_num;
    printf("Enter account number: ");
    scanf("%d", &acc_num);
    getchar();

    if (!found_account(acc_num)) {
        printf("Account not found.\n");
        fclose(fp);
        return;
    }

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, fp)) {
        if (acc.account_number == acc_num) {
            found = 1;
            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter password: ");
                scanf("%s", entered_pass);
                if (strcmp(entered_pass, acc.password) == 0) {
                    matched = 1;
                    break;
                } else {
                    printf("Incorrect password. ");
                    if (attempts > 0)
                        printf("Try again (%d attempts left).\n", attempts);
                }
            }

            if (!matched) {
                printf("Too many incorrect attempts.\n");
                fclose(fp);
                return;
            }

            printf("Acc No | Name | Address | Balance\n");
            printf("%d | %s | %s | %.2f\n", acc.account_number, acc.name, acc.address, acc.balance);
            break;
        }
    }

    if (!found) printf("Account not found.\n");
    fclose(fp);
}


void modify() {
    FILE *dat = fopen("initial.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!dat || !temp) {
        printf("File error.\n");
        if (dat) fclose(dat);
        if (temp) fclose(temp);
        return;
    }

    int acc_no, found = 0;
    Account acc, newAcc;

    printf("Enter account number to modify: ");
    scanf("%d", &acc_no);
    clear();

    while (fread(&acc, sizeof(Account), 1, dat)) {
        if (acc.account_number == acc_no) {
            found = 1;

            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter password: ");
                scanf("%s", entered_pass);
                clear();
                if (strcmp(entered_pass, acc.password) == 0) {
                    matched = 1;
                    break;
                } else {
                    printf("Incorrect password. ");
                    if (attempts > 0)
                        printf("Try again (%d attempts left).\n", attempts);
                }
            }

            if (!matched) {
                printf("Too many incorrect attempts.\n");
                fclose(dat);
                fclose(temp);
                remove("temp.dat");
                return;
            }

            int option;
            do {
                printf("0. Exit Modify\n1. Change Name\n2. Change Address\n3. Change Password\n");
                scanf("%d", &option);
                clear();

                if (option == 1) {
                    printf("New name: ");
                    fgets(acc.name, sizeof(acc.name), stdin);
                    acc.name[strcspn(acc.name, "\n")] = '\0';
                } else if (option == 2) {
                    printf("New address: ");
                    fgets(acc.address, sizeof(acc.address), stdin);
                    acc.address[strcspn(acc.address, "\n")] = '\0';
                } else if (option == 3) {
                    printf("New password: ");
                    fgets(acc.password, sizeof(acc.password), stdin);
                    acc.password[strcspn(acc.password, "\n")] = '\0';
                }
            } while (option != 0);
        }

        fwrite(&acc, sizeof(Account), 1, temp);
    }

    fclose(dat);
    fclose(temp);
    remove("initial.dat");
    rename("temp.dat", "initial.dat");

    if (found)
        printf("Account %d modified successfully.\n", acc_no);
    else
        printf("Account number not found.\n");
}


void close_account() {
    FILE *fp = fopen("initial.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!fp || !temp) {
        printf("Error opening file(s).\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    int acc_no;
    printf("Enter account number to close: ");
    scanf("%d", &acc_no);
    clear();

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, fp)) {
        if (acc.account_number == acc_no) {
            found = 1;
            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter your password: ");
                scanf("%s", entered_pass);
                clear();
                if (strcmp(entered_pass, acc.password) == 0) {
                    matched = 1;
                    break;
                } else {
                    printf("Incorrect password. ");
                    if (attempts > 0)
                        printf("Try again (%d attempts left).\n", attempts);
                }
            }

            if (!matched) {
                printf("Account closure cancelled.\n");
                fclose(fp);
                fclose(temp);
                remove("temp.dat");
                return;
            }

            acc.balance = 0;
            printf("Account %d closed successfully.\n", acc.account_number);
        }

        fwrite(&acc, sizeof(Account), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("initial.dat");
    rename("temp.dat", "initial.dat");

    if (!found)
        printf("Account number not found.\n");
}

void transaction_function() {
    Transaction bank;
    int acc_num, choice;

again_acc_num:
    printf("Enter your account number: ");
    scanf("%d", &acc_num);
    clear();

    if (!found_account(acc_num)) {
        printf("Account not found.\n");
        printf("Enter 1 to retry, 0 to exit: ");
        scanf("%d", &choice);
        clear();
        if (choice == 1) goto again_acc_num;
        else return;
    }

    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    Account acc;
    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_num) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Account not found in file.\n");
        return;
    }

    char entered_pass[20];
    int attempts = 3, matched = 0;
    while (attempts--) {
        printf("Enter password: ");
        scanf("%s", entered_pass);
        clear();
        if (strcmp(entered_pass, acc.password) == 0) {
            matched = 1;
            break;
        } else {
            printf("Incorrect password. ");
            if (attempts > 0)
                printf("Try again (%d attempts left).\n", attempts);
        }
    }
    if (!matched) {
        printf("Too many incorrect attempts.\n");
        return;
    }

    float balance = give_balance(acc_num);
    bank.account_number = acc_num;
    int option;

    do {
        printf("1. DEPOSIT\n2. WITHDRAWAL\n0. EXIT\n");
        printf("Choose: ");
        scanf("%d", &option);
        clear();

        if (option == 1) {
            strcpy(bank.type, "DEPOSIT");
            printf("Enter today's date (dd-mm-yyyy): ");
            fgets(bank.date, sizeof(bank.date), stdin);
            bank.date[strcspn(bank.date, "\n")] = 0;

            printf("Enter mode of transaction: ");
            scanf("%s", bank.mode);
            clear();
            printf("Enter amount: ");
            scanf("%f", &bank.amount);
            clear();

            if (bank.amount <= 0) {
                printf("Invalid amount.\n");
                continue;
            }
            balance += bank.amount;
        } else if (option == 2) {
            strcpy(bank.type, "WITHDRAWAL");
            printf("Enter today's date (dd-mm-yyyy): ");
            fgets(bank.date, sizeof(bank.date), stdin);
            bank.date[strcspn(bank.date, "\n")] = 0;

            printf("Enter mode of transaction: ");
            scanf("%s", bank.mode);
            clear();
            printf("Enter amount: ");
            scanf("%f", &bank.amount);
            clear();

            if ((balance - 500) < bank.amount || bank.amount <= 0) {
                printf("Transaction failed: insufficient funds or invalid amount.\n");
                continue;
            }
            balance -= bank.amount;
        } else if (option == 0) {
            break;
        } else {
            printf("Invalid option.\n");
            continue;
        }

        update_balance(acc_num, balance);
        bank.balance = balance;
        add_transaction_to_file(bank);
        printf("Transaction successful. Updated balance: %.2f\n", bank.balance);
    } while (option != 0);
}



void get_today_date(int *day, int *month, int *year) {
    time_t now;
    struct tm *current_time;

    time(&now);
    current_time = localtime(&now);

    *day = current_time->tm_mday;
    *month = current_time->tm_mon + 1;
    *year = current_time->tm_year + 1900;
}
void main_menu() {
    int choice;
    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Open a New Account\n");
        printf("2. Make a Transaction\n");
        printf("3. View Account Statement\n");
        printf("4. Display Account Details\n");
        printf("5. Edit Account Information\n");
        printf("6. monthly report\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear(); // Important to clear buffer after scanf

        switch (choice) {
            case 1: new_account(); break;
            case 2: transaction_function(); break;
            case 3: display_list(); break;
            case 4: display_account(); break;
            case 5: edit_menu(); break;
            case 6: month_report();break; 
            case 7: return;
            default: printf("Invalid input. Please try again.\n"); break;
        }
    } while (choice != 7);
}
void edit_menu() {
    int choice;
    do {
        printf("Edit Menu\n");
        printf("1. Modify Account Details\n");
        printf("2. Close an Account\n");
        printf("3. Delete an account\n");
        printf("4. Return to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear(); // Clear buffer

        switch (choice) {
            case 1: modify(); break;
            case 2: close_account(); break;
            case 3: delete_account(); break; 
            case 4: return; // Exit loop
            default: printf("Invalid input. Please try again.\n"); break;
        }
    } while (choice != 3);
}
void delete_account() {
    Account acc;
    FILE *fp, *temp;
    int account_number, found = 0;
    printf("Enter account number to delete: ");
    scanf("%d", &account_number);

    if (!found_account(account_number)) {
        printf("Account number %d not found.\n", account_number);
        return;
    }

    fp = fopen("initial.dat", "rb");
    temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("Error opening files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == account_number) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Account not found in file.\n");
        return;
    }

    char entered_pass[20];
    int attempts = 3, matched = 0;
    while (attempts--) {
        printf("Enter password: ");
        scanf("%s", entered_pass);
        clear();
        if (strcmp(entered_pass, acc.password) == 0) {
            matched = 1;
            break;
        } else {
            printf("Incorrect password. ");
            if (attempts > 0)
                printf("Try again (%d attempts left).\n", attempts);
        }
    }
    if (!matched) {
        printf("Too many incorrect attempts.\n");
        return;
    }

    fp = fopen("initial.dat", "rb");
    temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("Error opening files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == account_number && strcmp(entered_pass, acc.password) == 0) {
            printf("Account %d deleted successfully.\n", acc.account_number);
            // Do not write this one
        } else {
            fwrite(&acc, sizeof(Account), 1, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("initial.dat");
    rename("temp.dat", "initial.dat");

}


double give_balance(int acc_no) {
    Account acc;
    FILE *fp;
    int found = 0;

    fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening file.\n");
        return -1;  
    }
    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.account_number == acc_no) {
            printf("\nAccount Number: %d\nName: %s\nBalance: %.2f\n",
                   acc.account_number, acc.name, acc.balance);
            found = 1;
            break;
        }
    }

    if (!found){
         printf("Account not found.\n");
         return -1;

    }
       
    return acc.balance; 
    fclose(fp);
    
}
void clear() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} // Clear the input buffer
}
int found_account(int target_account) {
    Account data;
    FILE *fp = fopen("initial.dat", "rb");

    if (!fp) {
        printf("Error opening file.\n");
        return 0;  // treat as not found
    }

    while (fread(&data, sizeof(Account), 1, fp) == 1) {
        if (data.account_number == target_account) {
            fclose(fp);
            return 1;  // found
        }
    }

    fclose(fp);
    return 0;  // not found
}
 	
void update_balance(int acc_num, float new_balance) {
    Account data;
    FILE *fp, *temp;

    fp = fopen("initial.dat", "rb");
    temp = fopen("temp.dat", "wb");
    if (!fp || !temp) {
        printf("Error opening file(s).\n");
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }

    int updated = 0;

    while (fscanf(fp, "%d \"%99[^\"]\" \"%199[^\"]\" %f \"%19[^\"]\"\n",
                  &data.account_number, data.name, data.address, &data.balance, data.password) == 5) {
        if (data.account_number == acc_num) {
            data.balance += new_balance;
            updated = 1;
        }
        fprintf(temp, "%d \"%s\" \"%s\" %.2f \"%s\"\n",
                data.account_number, data.name, data.address, data.balance, data.password);
    }

    fclose(fp);
    fclose(temp);

    if (updated) {
        remove("initial.dat");
        rename("temp.dat", "initial.dat");
        printf("Balance updated successfully.\n");
    } else {
        remove("temp.dat");
        printf("Account not found, balance not updated.\n");
    }
}

void display() {
    int target_account;
    Account data;
    int found = 0;

    printf("Enter account number to display:\n");
    scanf("%d", &target_account);
    getchar(); // clear newline

    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    // Read each record and search
    while (fscanf(fp, "%d \"%99[^\"]\" \"%199[^\"]\" %f \"%19[^\"]\"\n",
                  &data.account_number, data.name, data.address, &data.balance, data.password) == 5) {
        if (data.account_number == target_account) {
            printf("\nAccount Details:\n");
            printf("Account Number : %d\n", data.account_number);
            printf("Name           : %s\n", data.name);
            printf("Address        : %s\n", data.address);
            printf("Opening Balance: %.2f\n", data.balance);
            // printf("Password       : %s\n", data.password); // usually skip showing password
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (!found) {
        printf("Account not found.\n");
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure definitions
typedef struct {
    int account_number;
    char name[100];
    char address[200];
    char password[20];
    float balance;
} Account;

typedef struct {
    int account_number;
    char date[11];      // dd-mm-yyyy
    char type[10];      // "DEPOSIT" or "WITHDRAWAL"
    char mode[20];      // e.g., "Cash", "Cheque"
    float amount;
    float balance;      // balance after this transaction
} Transaction;

/* Function prototypes */
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
void clear_input_buffer();
void get_today_date(int *day, int *month, int *year);

/* ------------------ Implementation ------------------ */

int main() {
    printf("Banking system\n");
    main_menu();
    printf("Thank you\n");
    return 0;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
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

void add_account_to_file(Account newAcc) {
    FILE *fp = fopen("initial.dat", "ab");
    if (fp == NULL) {
        perror("Error opening initial.dat for append");
        return;
    }
    fwrite(&newAcc, sizeof(Account), 1, fp);
    fclose(fp);
}

void add_transaction_to_file(Transaction newTransc) {
    FILE *fp = fopen("banking.dat", "ab");
    if (fp == NULL) {
        perror("Error opening banking.dat for append");
        return;
    }
    fwrite(&newTransc, sizeof(Transaction), 1, fp);
    fclose(fp);
}

int found_account(int target_account) {
    Account data;
    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        // File doesn't exist or can't open -> treat as not found
        return 0;
    }
    while (fread(&data, sizeof(Account), 1, fp) == 1) {
        if (data.account_number == target_account) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

double give_balance(int acc_no) {
    Account acc;
    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        // Error opening file
        return -1;
    }
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_no) {
            fclose(fp);
            return acc.balance;
        }
    }
    fclose(fp);
    return -1;
}

void update_balance(int acc_num, float new_balance) {
    Account acc;
    FILE *fp = fopen("initial.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (!fp || !temp) {
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Error opening file(s) for update_balance\n");
        return;
    }

    int updated = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_num) {
            acc.balance = new_balance;   // set to new balance
            updated = 1;
        }
        fwrite(&acc, sizeof(Account), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    if (updated) {
        remove("initial.dat");
        rename("temp.dat", "initial.dat");
        // don't spam success messages here; caller can show if needed
    } else {
        remove("temp.dat");
        printf("Account not found, balance not updated.\n");
    }
}

void box_for_display() {
    printf("%-12s%-18s%-12s%-14s%-10s\n", "DATE", "PARTICULARS", "DEPOSIT", "WITHDRAWAL", "BALANCE");
    printf("-----------------------------------------------------------------\n");
}

int no_of_days(int month, int year) {
    if (month < 1 || month > 12) return -1;
    int month_day[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) month_day[1] = 29;
    return month_day[month - 1];
}

void new_account() {
    Transaction banking;
    Account data;
    int last_account_number = 100000;

    // determine next account number from initial.dat (binary)
    FILE *fp_check = fopen("initial.dat", "rb");
    if (fp_check) {
        Account temp;
        while (fread(&temp, sizeof(Account), 1, fp_check) == 1) {
            if (temp.account_number > last_account_number) last_account_number = temp.account_number;
        }
        fclose(fp_check);
    }

start:
    printf("Enter name:\n");
    fgets(data.name, sizeof(data.name), stdin);
    data.name[strcspn(data.name, "\n")] = 0;

    // Validate name
    for (int i = 0; data.name[i] != '\0'; i++) {
        if (!((data.name[i] >= 'A' && data.name[i] <= 'Z') ||
              (data.name[i] >= 'a' && data.name[i] <= 'z') ||
              data.name[i] == ' ')) {
            printf("Error: Name contains invalid characters. Only alphabets and spaces are allowed.\n");
            goto start;
        }
    }

    // Check duplicate name (not ideal for real systems but kept)
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

    printf("Enter your address:\n");
    fgets(data.address, sizeof(data.address), stdin);
    data.address[strcspn(data.address, "\n")] = 0;

    printf("Create a password (max 19 characters):\n");
    fgets(data.password, sizeof(data.password), stdin);
    data.password[strcspn(data.password, "\n")] = 0;

    printf("Enter opening balance amount:\n");
    if (scanf("%f", &data.balance) != 1) {
        clear_input_buffer();
        printf("Invalid amount input.\n");
        goto start;
    }
    clear_input_buffer();

    if (data.balance < 500) {
        printf("Error: Minimum opening balance must be ₹500 or more.\n");
        goto start;
    }

    // assign account number
    data.account_number = last_account_number + 1;

    // write account to initial.dat (binary)
    add_account_to_file(data);

    // create initial transaction entry for opening deposit
    banking.account_number = data.account_number;
    {
        int d, m, y;
        get_today_date(&d, &m, &y);
        snprintf(banking.date, sizeof(banking.date), "%02d-%02d-%04d", d, m, y);
    }
    strncpy(banking.type, "DEPOSIT", sizeof(banking.type) - 1);
    banking.type[sizeof(banking.type) - 1] = '\0';
    strncpy(banking.mode, "Opening", sizeof(banking.mode) - 1);
    banking.mode[sizeof(banking.mode) - 1] = '\0';
    banking.amount = data.balance;
    banking.balance = data.balance;
    add_transaction_to_file(banking);

    printf("\nAccount created successfully!\n");
    printf("Your account number is: %d\n", data.account_number);
}

void transaction_function() {
    Transaction bank;
    int acc_num, choice;

again_acc_num:
    printf("Enter your account number: ");
    if (scanf("%d", &acc_num) != 1) {
        clear_input_buffer();
        printf("Invalid input.\n");
        return;
    }
    clear_input_buffer();

    if (!found_account(acc_num)) {
        printf("Account not found.\n");
        printf("Enter 1 to retry, 0 to exit: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            return;
        }
        clear_input_buffer();
        if (choice == 1) goto again_acc_num;
        else return;
    }

    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening account file.\n");
        return;
    }

    Account acc;
    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_num) { found = 1; break; }
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
        if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); break; }
        clear_input_buffer();
        if (strcmp(entered_pass, acc.password) == 0) { matched = 1; break; }
        else {
            printf("Incorrect password. ");
            if (attempts > 0) printf("Try again (%d attempts left).\n", attempts);
        }
    }
    if (!matched) {
        printf("Too many incorrect attempts.\n");
        return;
    }

    float balance = (float) give_balance(acc_num);
    if (balance < 0) {
        printf("Unable to retrieve balance.\n");
        return;
    }
    bank.account_number = acc_num;

    int option;
    do {
        printf("\n1. DEPOSIT\n2. WITHDRAWAL\n0. EXIT\n");
        printf("Choose: ");
        if (scanf("%d", &option) != 1) {
            clear_input_buffer();
            printf("Invalid choice.\n");
            continue;
        }
        clear_input_buffer();

        if (option == 1) {
            strncpy(bank.type, "DEPOSIT", sizeof(bank.type)); bank.type[sizeof(bank.type)-1] = '\0';
            printf("Enter today's date (dd-mm-yyyy) or press enter to use today: ");
            char tmp[32]; fgets(tmp, sizeof(tmp), stdin);
            if (tmp[0] == '\n' || tmp[0] == '\0') {
                int d,m,y; get_today_date(&d,&m,&y);
                snprintf(bank.date, sizeof(bank.date), "%02d-%02d-%04d", d,m,y);
            } else {
                tmp[strcspn(tmp, "\n")] = '\0';
                strncpy(bank.date, tmp, sizeof(bank.date)-1); bank.date[sizeof(bank.date)-1] = '\0';
            }
            printf("Enter mode of transaction: ");
            if (scanf("%19s", bank.mode) != 1) { clear_input_buffer(); strcpy(bank.mode, "Unknown"); }
            clear_input_buffer();
            printf("Enter amount: ");
            if (scanf("%f", &bank.amount) != 1) { clear_input_buffer(); printf("Invalid amount.\n"); continue; }
            clear_input_buffer();

            if (bank.amount <= 0) {
                printf("Invalid amount.\n");
                continue;
            }
            balance += bank.amount;
        }
        else if (option == 2) {
            strncpy(bank.type, "WITHDRAWAL", sizeof(bank.type)); bank.type[sizeof(bank.type)-1] = '\0';
            printf("Enter today's date (dd-mm-yyyy) or press enter to use today: ");
            char tmp[32]; fgets(tmp, sizeof(tmp), stdin);
            if (tmp[0] == '\n' || tmp[0] == '\0') {
                int d,m,y; get_today_date(&d,&m,&y);
                snprintf(bank.date, sizeof(bank.date), "%02d-%02d-%04d", d,m,y);
            } else {
                tmp[strcspn(tmp, "\n")] = '\0';
                strncpy(bank.date, tmp, sizeof(bank.date)-1); bank.date[sizeof(bank.date)-1] = '\0';
            }
            printf("Enter mode of transaction: ");
            if (scanf("%19s", bank.mode) != 1) { clear_input_buffer(); strcpy(bank.mode, "Unknown"); }
            clear_input_buffer();
            printf("Enter amount: ");
            if (scanf("%f", &bank.amount) != 1) { clear_input_buffer(); printf("Invalid amount.\n"); continue; }
            clear_input_buffer();

            if (bank.amount <= 0) {
                printf("Invalid amount.\n");
                continue;
            }
            if ((balance - 500) < bank.amount) {
                printf("Transaction failed: insufficient funds (min ₹500 required to keep account active).\n");
                continue;
            }
            balance -= bank.amount;
        }
        else if (option == 0) {
            break;
        }
        else {
            printf("Invalid option.\n");
            continue;
        }

        // update and record transaction
        update_balance(acc_num, balance);
        bank.balance = balance;
        add_transaction_to_file(bank);
        printf("Transaction successful. Updated balance: %.2f\n", bank.balance);

    } while (option != 0);
}

void display_account() {
    int acc_no;
    char entered_password[20];

    printf("Enter account number to display: ");
    if (scanf("%d", &acc_no) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    printf("Enter password: ");
    if (scanf("%19s", entered_password) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    FILE *fp_acc = fopen("initial.dat", "rb");
    if (!fp_acc) { perror("Error opening initial.dat"); return; }

    Account acc;
    int auth_success = 0;

    while (fread(&acc, sizeof(Account), 1, fp_acc) == 1) {
        if (acc.account_number == acc_no) {
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

    float current_balance = (float) give_balance(acc_no);
    if (current_balance < 0) {
        printf("Unable to fetch balance.\n");
        return;
    }

    int dd, mm, yy;
    get_today_date(&dd, &mm, &yy);

    printf("\nAccount Number: %d\t\tDate: %02d/%02d/%04d\n", acc_no, dd, mm, yy);
    printf("-------------------------------------------------------------\n");
    box_for_display();

    FILE *fp = fopen("banking.dat", "rb");
    if (!fp) {
        printf("No transactions recorded yet.\n");
        return;
    }

    Transaction trans;
    float total_deposit = 0, total_withdraw = 0;
    int found = 0;

    while (fread(&trans, sizeof(Transaction), 1, fp) == 1) {
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
            printf("%-12s %-12s %-10.2f %-10.2f %-10.2f\n",
                   trans.date, trans.type, deposit, withdraw, trans.balance);
        }
    }
    fclose(fp);

    if (!found) {
        printf("No transactions found for Account %d.\n", acc_no);
        return;
    }

    printf("-------------------------------------------------------------\n");
    printf(" TOTAL ->        %-10.2f %-10.2f %-10.2f\n", total_deposit, total_withdraw, current_balance);
}

void display_list() {
    FILE *fp = fopen("initial.dat", "rb");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    int acc_num;
    printf("Enter account number: ");
    if (scanf("%d", &acc_num) != 1) { clear_input_buffer(); fclose(fp); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    if (!found_account(acc_num)) {
        printf("Account not found.\n");
        fclose(fp);
        return;
    }

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_num) {
            found = 1;
            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter password: ");
                if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); break; }
                clear_input_buffer();
                if (strcmp(entered_pass, acc.password) == 0) { matched = 1; break; }
                else {
                    printf("Incorrect password. ");
                    if (attempts > 0) printf("Try again (%d attempts left).\n", attempts);
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
        if (dat) fclose(dat);
        if (temp) fclose(temp);
        printf("File error.\n");
        return;
    }

    int acc_no;
    printf("Enter account number to modify: ");
    if (scanf("%d", &acc_no) != 1) { clear_input_buffer(); fclose(dat); fclose(temp); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, dat) == 1) {
        if (acc.account_number == acc_no) {
            found = 1;

            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter password: ");
                if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); break; }
                clear_input_buffer();
                if (strcmp(entered_pass, acc.password) == 0) { matched = 1; break; }
                else {
                    printf("Incorrect password. ");
                    if (attempts > 0) printf("Try again (%d attempts left).\n", attempts);
                }
            }

            if (!matched) {
                printf("Too many incorrect attempts.\n");
                // write the rest unmodified
            } else {
                int option;
                do {
                    printf("0. Exit Modify\n1. Change Name\n2. Change Address\n3. Change Password\n");
                    printf("Enter option: ");
                    if (scanf("%d", &option) != 1) { clear_input_buffer(); printf("Invalid option.\n"); continue; }
                    clear_input_buffer();

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
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Error opening file(s).\n");
        return;
    }

    int acc_no;
    printf("Enter account number to close: ");
    if (scanf("%d", &acc_no) != 1) { clear_input_buffer(); fclose(fp); fclose(temp); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    Account acc;
    int found = 0;

    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == acc_no) {
            found = 1;
            char entered_pass[20];
            int attempts = 3, matched = 0;
            while (attempts--) {
                printf("Enter your password: ");
                if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); break; }
                clear_input_buffer();
                if (strcmp(entered_pass, acc.password) == 0) { matched = 1; break; }
                else {
                    printf("Incorrect password. ");
                    if (attempts > 0) printf("Try again (%d attempts left).\n", attempts);
                }
            }

            if (!matched) {
                printf("Account closure cancelled.\n");
            } else {
                acc.balance = 0;
                printf("Account %d closed successfully.\n", acc.account_number);
            }
        }
        fwrite(&acc, sizeof(Account), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("initial.dat");
    rename("temp.dat", "initial.dat");

    if (!found) printf("Account number not found.\n");
}

void delete_account() {
    Account acc;
    FILE *fp;
    FILE *temp;
    int account_number;
    printf("Enter account number to delete: ");
    if (scanf("%d", &account_number) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    if (!found_account(account_number)) {
        printf("Account number %d not found.\n", account_number);
        return;
    }

    // read account to get password for verification
    fp = fopen("initial.dat", "rb");
    if (!fp) { printf("Error opening initial.dat\n"); return; }

    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == account_number) { found = 1; break; }
    }
    fclose(fp);

    if (!found) { printf("Account not found in file.\n"); return; }

    char entered_pass[20];
    int attempts = 3, matched = 0;
    while (attempts--) {
        printf("Enter password: ");
        if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); break; }
        clear_input_buffer();
        if (strcmp(entered_pass, acc.password) == 0) { matched = 1; break; }
        else {
            printf("Incorrect password. ");
            if (attempts > 0) printf("Try again (%d attempts left).\n", attempts);
        }
    }
    if (!matched) {
        printf("Too many incorrect attempts.\n");
        return;
    }

    fp = fopen("initial.dat", "rb");
    temp = fopen("temp.dat", "wb");
    if (!fp || !temp) {
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Error opening files.\n");
        return;
    }

    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.account_number == account_number && strcmp(entered_pass, acc.password) == 0) {
            printf("Account %d deleted successfully.\n", acc.account_number);
            // skip writing this account
        } else {
            fwrite(&acc, sizeof(Account), 1, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("initial.dat");
    rename("temp.dat", "initial.dat");
}

void month_report() {
    int acc_num, day, month, year;
    char entered_pass[20];

start:
    printf("Enter the account number: ");
    if (scanf("%d", &acc_num) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    printf("Enter the password: ");
    if (scanf("%19s", entered_pass) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    // Verify account and password
    FILE *fpa = fopen("initial.dat", "rb");
    if (!fpa) { printf("Could not open initial.dat file.\n"); return; }

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

    printf("Enter a date (DD MM YYYY): ");
    if (scanf("%d %d %d", &day, &month, &year) != 3) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    int last_day = no_of_days(month, year);
    if (last_day == -1) { printf("Invalid month/year provided.\n"); return; }

    FILE *fp = fopen("banking.dat", "rb");
    if (!fp) { printf("Could not open banking.dat file.\n"); return; }

    Transaction trans;
    int t_day, t_month, t_year;
    int header_printed = 0;
    int match_found = 0;

    while (fread(&trans, sizeof(Transaction), 1, fp) == 1) {
        if (trans.account_number != acc_num) continue;
        if (sscanf(trans.date, "%d-%d-%d", &t_day, &t_month, &t_year) != 3) continue;
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

void main_menu() {
    int choice = 0;
    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Open a New Account\n");
        printf("2. Make a Transaction\n");
        printf("3. View Account Statement (Basic)\n");
        printf("4. Display Account Details (Statement)\n");
        printf("5. Edit Account Information\n");
        printf("6. Monthly report\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) { clear_input_buffer(); printf("Invalid input.\n"); continue; }
        clear_input_buffer();

        switch (choice) {
            case 1: new_account(); break;
            case 2: transaction_function(); break;
            case 3: display_list(); break;
            case 4: display_account(); break;
            case 5: edit_menu(); break;
            case 6: month_report(); break;
            case 7: return;
            default: printf("Invalid input. Please try again.\n"); break;
        }
    } while (1);
}

void edit_menu() {
    int choice = 0;
    do {
        printf("\nEdit Menu\n");
        printf("1. Modify Account Details\n");
        printf("2. Close an Account\n");
        printf("3. Delete an account\n");
        printf("4. Return to Main Menu\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) { clear_input_buffer(); printf("Invalid input.\n"); continue; }
        clear_input_buffer();

        switch (choice) {
            case 1: modify(); break;
            case 2: close_account(); break;
            case 3: delete_account(); break;
            case 4: return;
            default: printf("Invalid input. Please try again.\n"); break;
        }
    } while (1);
}

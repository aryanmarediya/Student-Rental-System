#define _CRT_SECURE_NO_WARNINGS

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void connectDatabase(MYSQL** db);
void displayRentalMenu();
void insertStudent(MYSQL* db);
void viewProperties(MYSQL* db);
void leaseProperty(MYSQL* db);
void makePayment(MYSQL* db);
void checkLeaseStatus(MYSQL* db);
void logMaintenanceRequest(MYSQL* db);
void requestDetailsUpdate(MYSQL* db);
void getStudentDetails(MYSQL* db, int studentId);

int isNumber(const char* str);
int isAlpha(const char* str);

int main() {
    MYSQL* db = NULL;
    char input[10]; 
    int choice;

    connectDatabase(&db);

    do {
        displayRentalMenu();
        while (1) {
            printf("Enter your choice: ");
            scanf("%s", input);
            if (!isNumber(input)) {
                printf("Invalid input! Please enter a number.\n");
            }
            else {
                break; 
            }
        }

        choice = atoi(input); 
        switch (choice) {
        case 1:
            insertStudent(db);
            break;
        case 2:
            viewProperties(db);
            break;
        case 3:
            leaseProperty(db);
            break;
        case 4:
            makePayment(db);
            break;
        case 5:
            checkLeaseStatus(db);
            break;
        case 6:
            logMaintenanceRequest(db);
            break;
        case 7:
            requestDetailsUpdate(db);
            break;
        case 0:
            printf("Exiting the Rental Management program.\n");
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 0);
    mysql_close(db);
    return EXIT_SUCCESS;
}


void connectDatabase(MYSQL** db) {
    const char* server = "localhost";
    const char* user = "root";
    const char* password = "mmaryan890";
    const char* database = "StudentRentalSystem";

    *db = mysql_init(NULL);
    if (*db == NULL) {
        printf("Error: Unable to initialize database connection.\n");
        exit(EXIT_FAILURE);
    }
    if (!mysql_real_connect(*db, server, user, password, database, 3306, NULL, 0)) {
        printf("Error: Unable to connect to the database. %s\n", mysql_error(*db));
        exit(EXIT_FAILURE);
    }
    printf("Connected to the database successfully.\n");
}

void displayRentalMenu() {
    printf("\n===== Rental Management Menu =====\n");
    printf("1. Register a new student\n");
    printf("2. View available properties\n");
    printf("3. Lease a property\n");
    printf("4. Make a payment\n");
    printf("5. Check lease status\n");
    printf("6. Maintenance request\n");
    printf("7. Request details update\n");
    printf("0. Exit\n");
    printf("===================================\n");
}

int isNumber(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int isAlpha(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ')
            return 0;
    }
    return 1;
}


void insertStudent(MYSQL* db) {
    char name[255], email[255], address[255], gender[10], institute[255];
    char contactNumber[20];

    printf("\nEnter Student Details:\n");
    do {
        printf("Name: ");
        scanf(" %[^\n]", name);
        if (strlen(name) == 0 || !isAlpha(name)) {
            printf("Invalid input. Name should contain only alphabets and cannot be empty. Please try again.\n");
        }
    } while (strlen(name) == 0 || !isAlpha(name));

    do {
        printf("Email: ");
        scanf(" %[^\n]", email);
        if (strlen(email) == 0 || strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
            printf("Invalid email format or empty input. Please enter a valid email address.\n");
        }
    } while (strlen(email) == 0 || strchr(email, '@') == NULL || strchr(email, '.') == NULL);

    do {
        printf("Contact Number: ");
        scanf(" %[^\n]", contactNumber);
        if (strlen(contactNumber) == 0 || !isNumber(contactNumber)) {
            printf("Invalid input. Contact number should contain only digits and cannot be empty. Please try again.\n");
        }
    } while (strlen(contactNumber) == 0 || !isNumber(contactNumber));

    do {
        printf("Address: ");
        scanf(" %[^\n]", address);
        if (strlen(address) == 0) {
            printf("Address cannot be empty. Please try again.\n");
        }
    } while (strlen(address) == 0);

    do {
        printf("Gender (Male/Female/Other): ");
        scanf(" %[^\n]", gender);
        if (strlen(gender) == 0 || (strcmp(gender, "Male") != 0 && strcmp(gender, "Female") != 0 && strcmp(gender, "Other") != 0)) {
            printf("Invalid gender or empty input. Please enter 'Male', 'Female', or 'Other'.\n");
        }
    } while (strlen(gender) == 0 || (strcmp(gender, "Male") != 0 && strcmp(gender, "Female") != 0 && strcmp(gender, "Other") != 0));

    do {
        printf("Institute: ");
        scanf(" %[^\n]", institute);
        if (strlen(institute) == 0) {
            printf("Institute cannot be empty. Please try again.\n");
        }
    } while (strlen(institute) == 0);

    char query[1024];
    sprintf(query,
        "INSERT INTO Student (Name, Email, ContactNumber, Address, Gender, Institute) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');",
        name, email, contactNumber, address, gender, institute);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to insert student. %s\n", mysql_error(db));
    }
    else {
        MYSQL_RES* result = mysql_store_result(db);
        if (result == NULL && mysql_field_count(db) == 0) {
            int lastStudentId = (int)mysql_insert_id(db);
            printf("Student Registered successfully. Your Student ID is: %d\n", lastStudentId);
        }
        else {
            printf("Error retrieving Student ID.\n");
        }
    }
}

void viewProperties(MYSQL* db) {
    const char* query = "SELECT p.PropertyId, p.PropertyAddress, p.City, p.PropertyType, u.UnitId, u.FloorNumber, u.Status "
        "FROM Property p "
        "LEFT JOIN Unit u ON p.PropertyId = u.PropertyId "
        "WHERE u.Status = 'Available';";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch properties. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    printf("\nAvailable Properties and Units:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Property ID: %s, Address: %s, City: %s, Type: %s, Unit ID: %s, Floor: %s, Status: %s\n",
            row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
    }

    mysql_free_result(result);
}

void leaseProperty(MYSQL* db) {
    int studentId, propertyId, unitId;
    char startDate[20], endDate[20];
    char query[1024];
    MYSQL_RES* result;
    MYSQL_ROW row;

    printf("\nEnter Lease Details:\n");
    printf("Student ID: ");
    scanf("%d", &studentId);

    printf("Property ID: ");
    scanf("%d", &propertyId);

    sprintf(query, "SELECT UnitId, FloorNumber FROM Unit WHERE PropertyId = %d AND Status = 'Available';", propertyId);
    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch available units. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: No available units for the selected property. %s\n", mysql_error(db));
        return;
    }

    printf("\nAvailable Units:\n");
    int hasAvailableUnits = 0; 
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Unit ID: %s, Floor: %s\n", row[0], row[1]);
        hasAvailableUnits = 1; 
    }
    mysql_free_result(result);

    if (!hasAvailableUnits) {
        printf("No available units for the selected property.\n");
        return;
    }

    printf("\nEnter Unit ID to lease: ");
    scanf("%d", &unitId);

    sprintf(query, "SELECT COUNT(*) FROM Unit WHERE UnitId = %d AND PropertyId = %d AND Status = 'Available';", unitId, propertyId);
    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to validate the selected unit. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store validation result. %s\n", mysql_error(db));
        return;
    }

    row = mysql_fetch_row(result);
    int isValidUnit = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (!isValidUnit) {
        printf("Error: The selected Unit ID %d does not exist or is not available.\n", unitId);
        return;
    }

    printf("Start Date (YYYY-MM-DD): ");
    scanf("%s", startDate);
    printf("End Date (YYYY-MM-DD): ");
    scanf("%s", endDate);

    sprintf(query,
        "INSERT INTO LeaseApplication (StudentId, PropertyId, UnitId, StartDate, EndDate, ApplicationStatus) "
        "VALUES (%d, %d, %d, '%s', '%s', 'Pending');",
        studentId, propertyId, unitId, startDate, endDate);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to lease property. %s\n", mysql_error(db));
    }
    else {
        printf("Lease application submitted successfully.\n");
    }
}

void makePayment(MYSQL* db) {
    int studentId, propertyId, paymentOption;
    char paymentMethod[50];
    double leasePrice = 0.0, utilityPrice = 0.0;
    char query[1024];

    printf("\nEnter Payment Details:\n");
    printf("Student ID: ");
    scanf("%d", &studentId);
    printf("Property ID: ");
    scanf("%d", &propertyId);

    sprintf(query,
        "SELECT LeasePrice, UtilityPrice, ApplicationStatus "
        "FROM LeaseApplication JOIN Property USING(PropertyId) "
        "WHERE StudentId = %d AND PropertyId = %d AND ApplicationStatus = 'Approved';",
        studentId, propertyId);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch lease information. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    MYSQL_ROW row = mysql_fetch_row(result);

    if (row == NULL) {
        printf("Error: No approved lease found for Student ID %d and Property ID %d.\n", studentId, propertyId);
        mysql_free_result(result);
        return;
    }

    leasePrice = atof(row[0]);
    utilityPrice = atof(row[1]);

    mysql_free_result(result);

    printf("\nPayment Options:\n");
    printf("1. Pay Rent ($%.2f)\n", leasePrice);
    printf("2. Pay Utilities ($%.2f)\n", utilityPrice);
    printf("Enter your choice: ");
    scanf("%d", &paymentOption);

    double paymentAmount = 0.0;
    char paymentType[50];

    if (paymentOption == 1) {
        paymentAmount = leasePrice;
        strcpy(paymentType, "Rent");
    }
    else if (paymentOption == 2) {
        paymentAmount = utilityPrice;
        strcpy(paymentType, "Utilities");
    }
    else {
        printf("Invalid option selected. Payment canceled.\n");
        return;
    }

    sprintf(query,
        "SELECT COUNT(*) FROM Payment WHERE StudentId = %d AND PropertyId = %d AND PaymentType = '%s';",
        studentId, propertyId, paymentType);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to verify payment history. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    row = mysql_fetch_row(result);
    int recurringPaymentExists = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (recurringPaymentExists > 0) {
        printf("\nPayment for %s is already set up for automatic deduction.\n", paymentType);
        printf("Recurring payments will continue to deduct monthly.\n");
        return;
    }

    printf("Enter Payment Method (e.g., Card/UPI): ");
    scanf("%s", paymentMethod);

    sprintf(query,
        "INSERT INTO Payment (StudentId, PropertyId, PaymentMethod, PaymentType, Amount, PaymentDate, RecurringPayment) "
        "VALUES (%d, %d, '%s', '%s', %.2f, CURDATE(), 'Yes');",
        studentId, propertyId, paymentMethod, paymentType, paymentAmount);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to process payment. %s\n", mysql_error(db));
    }
    else {
        printf("Payment for %s successfully processed. Recurring payments activated.\n", paymentType);
    }
}

void checkLeaseStatus(MYSQL* db) {
    int studentId, propertyId;
    char query[1024];

    printf("\nEnter Lease Status Details:\n");
    printf("Student ID: ");
    scanf("%d", &studentId);

    sprintf(query, "SELECT COUNT(*) FROM Student WHERE StudentId = %d;", studentId);
    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to verify Student ID. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int studentExists = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (studentExists == 0) {
        printf("Error: No student found with ID %d.\n", studentId);
        return;
    }

    printf("Property ID: ");
    scanf("%d", &propertyId);

    sprintf(query,
        "SELECT l.ApplicationStatus, "
        "IF(l.ApplicationStatus = 'Approved' AND "
        "(SELECT COUNT(*) FROM Payment WHERE StudentId = %d AND PropertyId = %d) > 0, 100, "
        "IF(l.ApplicationStatus = 'Approved', 50, 0)) AS LeaseCompletion, "
        "l.StartDate, l.EndDate, r.RejectionReason "
        "FROM LeaseApplication l "
        "LEFT JOIN LeaseRejectionReason r ON l.LeaseApplicationId = r.LeaseApplicationId "
        "WHERE l.StudentId = %d AND l.PropertyId = %d;",
        studentId, propertyId, studentId, propertyId);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch lease status. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        printf("\nLease Status: No lease found for Student ID %d and Property ID %d.\n", studentId, propertyId);
    }
    else {
        const char* status = row[0] ? row[0] : "Unknown";
        int completion = atoi(row[1]);
        const char* startDate = row[2] ? row[2] : "N/A";
        const char* endDate = row[3] ? row[3] : "N/A";
        const char* rejectionReason = row[4] ? row[4] : "N/A";

        printf("\nLease Status: %s\n", status);
        printf("Lease Start Date: %s\n", startDate);
        printf("Lease End Date: %s\n", endDate);

        if (strcmp(status, "Rejected") == 0) {
            printf("Rejection Reason: %s\n", rejectionReason);
        }
        else if (completion == 100) {
            printf("Your lease is fully complete. Please collect your keys from the Rental Office.\n");
        }
        else if (completion == 50) {
            printf("Your lease is approved but pending payment to reach 100%%.\n");
        }
        else if (strcmp(status, "Pending") == 0) {
            printf("Your lease application is still pending approval.\n");
        }
    }

    mysql_free_result(result);
}



void logMaintenanceRequest(MYSQL* db) {
    int studentId, propertyId;
    char description[255];
    char query[512];

    printf("\nEnter Maintenance Request Details:\n");
    printf("Student ID: ");
    scanf("%d", &studentId);
    printf("Property ID: ");
    scanf("%d", &propertyId);

    sprintf(query,
        "SELECT COUNT(*) FROM LeaseApplication "
        "WHERE StudentId = %d AND PropertyId = %d AND ApplicationStatus = 'Approved';",
        studentId, propertyId);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to verify lease ownership. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int leaseCount = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (leaseCount == 0) {
        printf("Error: No active lease found for Student ID %d and Property ID %d.\n", studentId, propertyId);
        return;
    }

    printf("Description: ");
    scanf(" %[^\n]", description);

    sprintf(query,
        "INSERT INTO MaintenanceRequest (StudentId, PropertyId, RequestDate, Description) "
        "VALUES (%d, %d, CURDATE(), '%s');",
        studentId, propertyId, description);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to log maintenance request. %s\n", mysql_error(db));
    }
    else {
        printf("Maintenance request logged successfully.\n");
    }
}


void requestDetailsUpdate(MYSQL* db) {
    int studentId;
    char newName[255], newEmail[255], newAddress[255], newGender[10], newInstitute[255];
    char newContactNumber[20];
    char query[1024];



    printf("\nEnter Student ID: ");
    scanf("%d", &studentId);

    printf("Fetching current details...\n");
    getStudentDetails(db, studentId);

    printf("\nEnter New Details:\n");

    printf("New Name: ");
    scanf(" %[^\n]", newName);
    while (!isAlpha(newName)) {
        printf("Invalid input. Name should contain only alphabets. Try again: ");
        scanf(" %[^\n]", newName);
    }

    printf("New Email: ");
    scanf(" %[^\n]", newEmail);

    printf("New Contact Number: ");
    scanf(" %[^\n]", newContactNumber);
    while (!isNumber(newContactNumber)) {
        printf("Invalid input. Contact number should contain only digits. Try again: ");
        scanf(" %[^\n]", newContactNumber);
    }

    printf("New Address: ");
    scanf(" %[^\n]", newAddress);

    printf("New Gender: ");
    scanf(" %[^\n]", newGender);
    while (!(strcmp(newGender, "Male") == 0 || strcmp(newGender, "Female") == 0 || strcmp(newGender, "Other") == 0)) {
        printf("Invalid gender. Please enter 'Male', 'Female', or 'Other': ");
        scanf(" %[^\n]", newGender);
    }

    printf("New Institute: ");
    scanf(" %[^\n]", newInstitute);

    sprintf(query,
        "INSERT INTO UpdateRequests (StudentId, NewName, NewEmail, NewContactNumber, NewAddress, NewGender, NewInstitute, RequestDate, Status) "
        "VALUES (%d, '%s', '%s', %s, '%s', '%s', '%s', CURDATE(), 'Pending');",
        studentId, newName, newEmail, newContactNumber, newAddress, newGender, newInstitute);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to submit update request. %s\n", mysql_error(db));
    }
    else {
        printf("Update request submitted successfully. Await management approval.\n");
    }
}

void getStudentDetails(MYSQL* db, int studentId) {
    char query[512];
    sprintf(query, "SELECT Name, Email, ContactNumber, Address, Gender, Institute FROM Student WHERE StudentId = %d;", studentId);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch student details. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    if (row == NULL) {
        printf("Error: No student found with ID %d.\n", studentId);
    }
    else {
        printf("\nHello, %s!\n", row[0]);
        printf("Here are your details:\n");
        printf("Email: %s\n", row[1]);
        printf("Contact Number: %s\n", row[2]);
        printf("Address: %s\n", row[3]);
        printf("Gender: %s\n", row[4]);
        printf("Institute: %s\n", row[5]);
    }

    mysql_free_result(result);
}



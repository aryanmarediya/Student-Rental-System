#define _CRT_SECURE_NO_WARNINGS

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_ATTEMPTS 3
#define PASSWORD_FILE "password.txt"
#define DEFAULT_PASSWORD "admin123"

void connectDatabase(MYSQL** db);
void displayManagementMenu();
void viewPendingLeases(MYSQL* db);
void viewAndManageMaintenanceRequests(MYSQL* db);
void viewAndManageUpdateRequests(MYSQL* db);
void removeUnits(MYSQL* db);
void addNewProperty(MYSQL* db);
void viewAllLeases(MYSQL* db);
void viewAllRegisteredStudents(MYSQL* db);
void removeStudent(MYSQL* db);
void displayPreviousOwners(MYSQL* db);
void changePassword();
int verifyPassword();

int main() {
    MYSQL* db = NULL;
    int choice;

    if (!verifyPassword()) {
        printf("Access denied. Exiting program.\n");
        return EXIT_FAILURE;
    }

    connectDatabase(&db);

    do {
        displayManagementMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid choice.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
        case 1:
            viewPendingLeases(db);
            break;
        case 2:
            viewAndManageMaintenanceRequests(db);
            break;
        case 3:
            viewAndManageUpdateRequests(db);
            break;
        case 4:
            removeUnits(db);
            break;
        case 5:
            addNewProperty(db);
            break;
        case 6:
            viewAllLeases(db);
            break;
        case 7:
            viewAllRegisteredStudents(db);
            break;
        case 8:
            removeStudent(db);
            break;
        case 9:
            displayPreviousOwners(db);
            break;
        case 10:
            changePassword();
            break;
        case 0:
            printf("Exiting the Management System.\n");
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
    const char* password = "890890890";
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

void displayManagementMenu() {
    printf("\n===================================\n");
    printf("      Management System       \n");
    printf("===================================\n");
    printf("1. Manage Pending Lease Applications\n");
    printf("2. Assign and Track Maintenance Requests\n");
    printf("3. Update Student Information Requests\n");
    printf("4. Remove Units from Properties\n");
    printf("5. Add New Properties\n");
    printf("6. View Lease Agreements\n");
    printf("7. View All Registered Students\n");
    printf("8. Remove And Move To Previous\n");
    printf("9. View Previous Lease History\n");
    printf("10. Change System Access Password\n");
    printf("\n0. Exit System\n");
    printf("===================================\n");
}

int verifyPassword() {
    char inputPassword[50];
    char storedPassword[50] = { 0 };
    FILE* passwordFile;

    passwordFile = fopen(PASSWORD_FILE, "r");
    if (passwordFile == NULL) {
        passwordFile = fopen(PASSWORD_FILE, "w");
        if (passwordFile == NULL) {
            fprintf(stderr, "Error: Unable to create password file.\n");
            return 0;
        }
        fprintf(passwordFile, "%s", DEFAULT_PASSWORD);
        fclose(passwordFile);
        strncpy(storedPassword, DEFAULT_PASSWORD, sizeof(storedPassword) - 1);
    }
    else {
        if (fscanf(passwordFile, "%49s", storedPassword) != 1) {
            fprintf(stderr, "Error: Unable to read password file.\n");
            fclose(passwordFile);
            return 0;
        }
        fclose(passwordFile);
    }

    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
        printf("User Name: Admin\n");
        printf("Enter password: ");
        if (scanf("%49s", inputPassword) != 1) {
            fprintf(stderr, "Invalid input.\n");
            while (getchar() != '\n');
            continue;
        }

        if (strcmp(inputPassword, storedPassword) == 0) {
            printf("Access granted.\n");
            return 1;
        }
        else {
            printf("Incorrect password. Attempts remaining: %d\n", MAX_ATTEMPTS - attempts - 1);
        }
    }

    return 0;
}

void changePassword() {
    char newPassword[50] = { 0 }, confirmPassword[50] = { 0 };
    FILE* passwordFile;
    
    printf("Enter new password: ");
    if (scanf("%49s", newPassword) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return;
    }

    printf("Confirm new password: ");
    if (scanf("%49s", confirmPassword) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return;
    }

    if (strcmp(newPassword, confirmPassword) != 0) {
        printf("Error: Passwords do not match.\n");
        return;
    }

    passwordFile = fopen(PASSWORD_FILE, "w");
    if (passwordFile == NULL) {
        fprintf(stderr, "Error: Unable to update password file.\n");
        return;
    }

    fprintf(passwordFile, "%s", newPassword);
    fclose(passwordFile);
    printf("Password changed successfully.\n");
}

void viewPendingLeases(MYSQL* db) {
    const char* query = "SELECT la.LeaseApplicationId, la.StudentId, s.Name, la.PropertyId, p.PropertyAddress, "
                        "p.City, la.UnitId, u.FloorNumber, la.StartDate, la.EndDate, la.ApplicationStatus "
                        "FROM LeaseApplication la "
                        "JOIN Student s ON la.StudentId = s.StudentId "
                        "JOIN Property p ON la.PropertyId = p.PropertyId "
                        "JOIN Unit u ON la.UnitId = u.UnitId "
                        "WHERE la.ApplicationStatus = 'Pending';";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch pending leases. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    printf("\nPending Lease Applications:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Lease ID: %s, Student ID: %s (%s), Property ID: %s (%s, %s), Unit ID: %s (Floor: %s), "
               "Start Date: %s, End Date: %s, Status: %s\n",
               row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9], row[10]);
    }

    mysql_free_result(result);

    int leaseId;
    char action;
    char rejectionReason[255];

    printf("\nEnter Lease ID to manage (or 0 to return to menu): ");
    scanf("%d", &leaseId);

    if (leaseId != 0) {
        printf("Approve or Reject this lease? (A/R): ");
        scanf(" %c", &action);

        if (action == 'A' || action == 'a') {
            char approveQuery[512];
            sprintf(approveQuery, "UPDATE LeaseApplication SET ApplicationStatus = 'Approved' WHERE LeaseApplicationId = %d;", leaseId);

            char updateUnitQuery[256];
            sprintf(updateUnitQuery, "UPDATE Unit SET Status = 'Leased' WHERE UnitId = (SELECT UnitId FROM LeaseApplication WHERE LeaseApplicationId = %d);", leaseId);

            if (mysql_query(db, approveQuery) == 0 && mysql_query(db, updateUnitQuery) == 0) {
                printf("Lease ID %d has been approved successfully.\n", leaseId);
            } else {
                printf("Error: Unable to approve lease. %s\n", mysql_error(db));
            }
        } else if (action == 'R' || action == 'r') {
            printf("Enter the reason for rejection: ");
            scanf(" %[^\n]", rejectionReason);

            char rejectQuery[256];
            sprintf(rejectQuery, "UPDATE LeaseApplication SET ApplicationStatus = 'Rejected' WHERE LeaseApplicationId = %d;", leaseId);

            char rejectionReasonQuery[512];
            sprintf(rejectionReasonQuery,
                    "INSERT INTO LeaseRejectionReason (LeaseApplicationId, RejectionReason) VALUES (%d, '%s');",
                    leaseId, rejectionReason);

            if (mysql_query(db, rejectQuery) == 0 && mysql_query(db, rejectionReasonQuery) == 0) {
                printf("Lease ID %d has been rejected successfully with reason: %s\n", leaseId, rejectionReason);
            } else {
                printf("Error: Unable to reject lease or record the reason. %s\n", mysql_error(db));
            }
        } else {
            printf("Invalid action. Returning to menu.\n");
        }
    }
}


void viewAndManageMaintenanceRequests(MYSQL* db) {
    const char* query =
        "SELECT mr.RequestId, p.PropertyAddress, s.Name, mr.Description, mr.RequestDate "
        "FROM MaintenanceRequest mr "
        "JOIN Property p ON mr.PropertyId = p.PropertyId "
        "JOIN Student s ON mr.StudentId = s.StudentId "
        "WHERE mr.RequestId NOT IN (SELECT RequestId FROM StaffAssignment);";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch maintenance requests. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    printf("\nUnassigned Maintenance Requests:\n");
    MYSQL_ROW row;
    int requestsFound = 0;
    while ((row = mysql_fetch_row(result)) != NULL) {
        requestsFound = 1;
        printf("Request ID: %s, Address: %s, Lease Owner: %s, Issue: %s, Date: %s\n",
            row[0], row[1], row[2], row[3], row[4]);
    }

    if (!requestsFound) {
        printf("No unassigned maintenance requests found.\n");
        mysql_free_result(result);
        return;
    }

    mysql_free_result(result);

    int requestId;
    printf("\nEnter Request ID to assign to a staff member (or 0 to return to menu): ");
    scanf("%d", &requestId);

    if (requestId == 0) {
        printf("Returning to the menu.\n");
        return;
    }

    char checkRequestQuery[256];
    sprintf(checkRequestQuery, "SELECT COUNT(*) FROM MaintenanceRequest WHERE RequestId = %d;", requestId);

    if (mysql_query(db, checkRequestQuery) != 0) {
        printf("Error: Unable to verify request ID. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    row = mysql_fetch_row(result);
    int requestExists = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (!requestExists) {
        printf("Error: Request ID %d does not exist.\n", requestId);
        return;
    }

    const char* staffQuery = "SELECT StaffId, CONCAT(FirstName, ' ', LastName) AS Name FROM Staff;";

    if (mysql_query(db, staffQuery) != 0) {
        printf("Error: Unable to fetch staff list. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* staffResult = mysql_store_result(db);
    if (staffResult == NULL) {
        printf("Error: Unable to store staff result. %s\n", mysql_error(db));
        return;
    }

    printf("\nAvailable Staff Members:\n");
    while ((row = mysql_fetch_row(staffResult)) != NULL) {
        printf("Staff ID: %s, Name: %s\n", row[0], row[1]);
    }

    mysql_free_result(staffResult);

    int staffId;
    printf("\nEnter Staff ID to assign to Request ID %d: ", requestId);
    scanf("%d", &staffId);

    char checkStaffQuery[256];
    sprintf(checkStaffQuery, "SELECT COUNT(*) FROM Staff WHERE StaffId = %d;", staffId);

    if (mysql_query(db, checkStaffQuery) != 0) {
        printf("Error: Unable to verify staff ID. %s\n", mysql_error(db));
        return;
    }

    result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    row = mysql_fetch_row(result);
    int staffExists = row ? atoi(row[0]) : 0;
    mysql_free_result(result);

    if (!staffExists) {
        printf("Error: Staff ID %d does not exist.\n", staffId);
        return;
    }

    char assignQuery[256];
    sprintf(assignQuery,
        "INSERT INTO StaffAssignment (RequestId, StaffId, AssignmentDate) "
        "VALUES (%d, %d, CURDATE());",
        requestId, staffId);

    if (mysql_query(db, assignQuery) != 0) {
        printf("Error: Unable to assign staff. %s\n", mysql_error(db));
    }
    else {
        printf("Request ID %d has been assigned to Staff ID %d successfully.\n", requestId, staffId);
    }
}




void removeUnits(MYSQL* db) {
    int propertyId;
    printf("\nEnter the Property ID to manage its units (or 0 to return to the menu): ");
    scanf("%d", &propertyId);

    if (propertyId == 0) {
        printf("Returning to the menu.\n");
        return;
    }

    char query[512];
    sprintf(query,
        "SELECT u.UnitId, u.FloorNumber, u.Status, "
        "(CASE WHEN EXISTS (SELECT 1 FROM LeaseApplication la WHERE la.UnitId = u.UnitId AND la.ApplicationStatus = 'Approved') "
        "THEN 'Leased' ELSE 'Available' END) AS LeaseStatus "
        "FROM Unit u "
        "WHERE u.PropertyId = %d;",
        propertyId);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch units for the property. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    printf("\nUnits for Property ID %d:\n", propertyId);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Unit ID: %s, Floor: %s, Status: %s, Lease Status: %s\n",
            row[0], row[1], row[2], row[3]);
    }
    mysql_free_result(result);

    int unitId;
    printf("\nEnter the Unit ID to remove (or 0 to return to the menu): ");
    scanf("%d", &unitId);

    if (unitId != 0) {

        char checkLeaseQuery[256];
        sprintf(checkLeaseQuery,
            "SELECT COUNT(*) FROM LeaseApplication WHERE UnitId = %d AND ApplicationStatus = 'Approved';",
            unitId);

        if (mysql_query(db, checkLeaseQuery) != 0) {
            printf("Error: Unable to verify lease status. %s\n", mysql_error(db));
            return;
        }

        MYSQL_RES* leaseResult = mysql_store_result(db);
        if (leaseResult == NULL) {
            printf("Error: Unable to store lease result. %s\n", mysql_error(db));
            return;
        }

        MYSQL_ROW leaseRow = mysql_fetch_row(leaseResult);
        int isLeased = leaseRow ? atoi(leaseRow[0]) : 0;
        mysql_free_result(leaseResult);

        if (isLeased) {
            printf("Error: Unit ID %d is currently leased and cannot be removed.\n", unitId);
            return;
        }

        char deleteQuery[256];
        sprintf(deleteQuery, "DELETE FROM Unit WHERE UnitId = %d;", unitId);

        if (mysql_query(db, deleteQuery) == 0) {
            printf("Unit ID %d has been removed successfully.\n", unitId);
        }
        else {
            printf("Error: Unable to remove the unit. %s\n", mysql_error(db));
        }
    }
    else {
        printf("Returning to the menu.\n");
    }
}


void addNewProperty(MYSQL* db) {
    char propertyAddress[255];
    char city[100];
    char postalCode[20];
    char propertyType[50];
    double leasePrice, utilityPrice;
    int numberOfUnits;

    printf("\nEnter New Property Details:\n");

    printf("Property Address: ");
    scanf(" %[^\n]", propertyAddress);

    printf("City: ");
    scanf(" %[^\n]", city);

    printf("Postal Code: ");
    scanf(" %[^\n]", postalCode);

    printf("Property Type (e.g., Apartment, Condo): ");
    scanf(" %[^\n]", propertyType);

    printf("Lease Price: ");
    scanf("%lf", &leasePrice);

    printf("Utility Price: ");
    scanf("%lf", &utilityPrice);

    char query[1024];
    sprintf(query,
        "INSERT INTO Property (PropertyAddress, City, PostalCode, PropertyType, LeasePrice, UtilityPrice) "
        "VALUES ('%s', '%s', '%s', '%s', %.2f, %.2f);",
        propertyAddress, city, postalCode, propertyType, leasePrice, utilityPrice);

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to add the new property. %s\n", mysql_error(db));
        return;
    }

    int propertyId = (int)mysql_insert_id(db);
    printf("New property added successfully. Property ID: %d\n", propertyId);

    printf("Enter the number of units for this property: ");
    scanf("%d", &numberOfUnits);

    for (int i = 0; i < numberOfUnits; i++) {
        int floorNumber;
        printf("Enter floor number for Unit %d: ", i + 1);
        scanf("%d", &floorNumber);

        char unitQuery[512];
        sprintf(unitQuery,
            "INSERT INTO Unit (FloorNumber, PropertyId, Status) "
            "VALUES (%d, %d, 'Available');",
            floorNumber, propertyId);

        if (mysql_query(db, unitQuery) != 0) {
            printf("Error: Unable to add unit %d. %s\n", i + 1, mysql_error(db));
            return;
        }
        else {
            printf("Unit %d added successfully.\n", i + 1);
        }
    }
}

void viewAndManageUpdateRequests(MYSQL* db) {
    const char* query = "SELECT RequestId, StudentId, NewName, NewEmail, NewContactNumber, NewAddress, NewGender, NewInstitute, RequestDate, Status "
        "FROM UpdateRequests WHERE Status = 'Pending';";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch update requests. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    printf("\nPending Update Requests:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Request ID: %s\n", row[0]);
        printf("Student ID: %s\n", row[1]);
        printf("New Name: %s\n", row[2]);
        printf("New Email: %s\n", row[3]);
        printf("New Contact: %s\n", row[4]);
        printf("New Address: %s\n", row[5]);
        printf("New Gender: %s\n", row[6]);
        printf("New Institute: %s\n", row[7]);
        printf("Date: %s\n\n", row[8]);
    }

    mysql_free_result(result);

    int requestId;
    char action;
    printf("Enter Request ID to manage (or 0 to return to menu): ");
    scanf("%d", &requestId);

    if (requestId != 0) {
        printf("Approve or Reject this request? (A/R): ");
        scanf(" %c", &action);

        if (action == 'A' || action == 'a') {

            char fetchQuery[512];
            sprintf(fetchQuery, "SELECT StudentId, NewName, NewEmail, NewContactNumber, NewAddress, NewGender, NewInstitute FROM UpdateRequests WHERE RequestId = %d;", requestId);

            if (mysql_query(db, fetchQuery) != 0) {
                printf("Error: Unable to fetch request details. %s\n", mysql_error(db));
                return;
            }

            MYSQL_RES* fetchResult = mysql_store_result(db);
            if (fetchResult == NULL) {
                printf("Error: Unable to store result. %s\n", mysql_error(db));
                return;
            }

            MYSQL_ROW fetchRow = mysql_fetch_row(fetchResult);
            if (fetchRow == NULL) {
                printf("Error: Request ID %d not found.\n", requestId);
                mysql_free_result(fetchResult);
                return;
            }

            int studentId = atoi(fetchRow[0]);
            char newName[255], newEmail[255], newContact[20], newAddress[255], newGender[50], newInstitute[255];
            strncpy(newName, fetchRow[1], sizeof(newName) - 1);
            strncpy(newEmail, fetchRow[2], sizeof(newEmail) - 1);
            strncpy(newContact, fetchRow[3], sizeof(newContact) - 1);
            strncpy(newAddress, fetchRow[4], sizeof(newAddress) - 1);
            strncpy(newGender, fetchRow[5], sizeof(newGender) - 1);
            strncpy(newInstitute, fetchRow[6], sizeof(newInstitute) - 1);

            mysql_free_result(fetchResult);

            char updateStudentQuery[1024];
            sprintf(updateStudentQuery,
                "UPDATE Student SET Name = '%s', Email = '%s', ContactNumber = %s, Address = '%s', Gender = '%s', Institute = '%s' WHERE StudentId = %d;",
                newName, newEmail, newContact, newAddress, newGender, newInstitute, studentId);

            if (mysql_query(db, updateStudentQuery) != 0) {
                printf("Error: Unable to update student details. %s\n", mysql_error(db));
                return;
            }

            char updateRequestQuery[256];
            sprintf(updateRequestQuery, "UPDATE UpdateRequests SET Status = 'Approved' WHERE RequestId = %d;", requestId);

            if (mysql_query(db, updateRequestQuery) != 0) {
                printf("Error: Unable to update request status. %s\n", mysql_error(db));
                return;
            }

            printf("Request ID %d has been approved and student details updated successfully.\n", requestId);
        }
        else if (action == 'R' || action == 'r') {

            char rejectQuery[256];
            sprintf(rejectQuery, "UPDATE UpdateRequests SET Status = 'Rejected' WHERE RequestId = %d;", requestId);

            if (mysql_query(db, rejectQuery) != 0) {
                printf("Error: Unable to update request status. %s\n", mysql_error(db));
            }
            else {
                printf("Request ID %d has been rejected.\n", requestId);
            }
        }
        else {
            printf("Invalid action. Returning to menu.\n");
        }
    }
}

void viewAllLeases(MYSQL* db) {
    const char* query =
        "SELECT la.LeaseApplicationId, s.StudentId, s.Name AS StudentName, s.Email AS StudentEmail, "
        "p.PropertyAddress, u.UnitId, u.FloorNumber, la.StartDate, la.EndDate, la.ApplicationStatus "
        "FROM LeaseApplication la "
        "JOIN Student s ON la.StudentId = s.StudentId "
        "JOIN Property p ON la.PropertyId = p.PropertyId "
        "JOIN Unit u ON la.UnitId = u.UnitId "
        "ORDER BY la.StartDate;";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch all leases. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    if (mysql_num_rows(result) == 0) {
        printf("No leases found in the system.\n");
        mysql_free_result(result);
        return;
    }

    printf("\nAll Leases:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Lease ID: %s \nStudent ID: %s \nStudent: %s (%s) \nProperty: %s \nUnit ID: %s \nFloor: %s \n"
            "Start Date: %s | End Date: %s \nStatus: %s\n\n",
            row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9]);
    }

    mysql_free_result(result);
}


void viewAllRegisteredStudents(MYSQL* db) {
    const char* query =
        "SELECT s.StudentId, s.Name, s.Email, s.ContactNumber, s.Address, "
        "CASE WHEN EXISTS (SELECT 1 FROM LeaseApplication la "
        "WHERE la.StudentId = s.StudentId AND la.ApplicationStatus = 'Approved') "
        "THEN 'Yes' ELSE 'No' END AS HasActiveLease "
        "FROM Student s "
        "ORDER BY s.StudentId;";

    if (mysql_query(db, query) != 0) {
        printf("Error: Unable to fetch registered students. %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (result == NULL) {
        printf("Error: Unable to store result. %s\n", mysql_error(db));
        return;
    }

    if (mysql_num_rows(result) == 0) {
        printf("No registered students found in the system.\n");
        mysql_free_result(result);
        return;
    }

    printf("\nRegistered Students:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("Student ID: %s \nName: %s \nEmail: %s \nContact: %s \nAddress: %s  "
            "Has Active Lease: %s\n\n",
            row[0], row[1], row[2], row[3], row[4], row[5]);
    }

    mysql_free_result(result);
}

void removeStudent(MYSQL* db) {
    int studentId;
    printf("\nEnter Student ID to remove: ");
    if (scanf("%d", &studentId) != 1 || studentId <= 0) {
        printf("Invalid input. Please enter a valid Student ID.\n");
        return;
    }

    if (mysql_query(db, "START TRANSACTION;") != 0) {
        printf("Error starting transaction: %s\n", mysql_error(db));
        return;
    }

    char leaseCheckQuery[512];
    snprintf(leaseCheckQuery, sizeof(leaseCheckQuery),
        "SELECT LeaseApplicationId, EndDate "
        "FROM LeaseApplication "
        "WHERE StudentId = %d AND DATEDIFF(EndDate, CURDATE()) <= 2 AND DATEDIFF(EndDate, CURDATE()) >= 0;",
        studentId);

    if (mysql_query(db, leaseCheckQuery) != 0) {
        printf("Error checking lease: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    MYSQL_RES* leaseResult = mysql_store_result(db);
    if (!leaseResult) {
        printf("Error retrieving lease result: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    MYSQL_ROW leaseRow = mysql_fetch_row(leaseResult);
    if (!leaseRow) {
        printf("Student ID %d does not have a lease ending within 2 days.\n", studentId);
        mysql_free_result(leaseResult);
        mysql_query(db, "ROLLBACK;");
        return;
    }

    int leaseId = atoi(leaseRow[0]);
    mysql_free_result(leaseResult);

    char checkQuery[256];
    snprintf(checkQuery, sizeof(checkQuery), "SELECT * FROM Student WHERE StudentId = %d;", studentId);

    if (mysql_query(db, checkQuery) != 0) {
        printf("Error executing query: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (!result) {
        printf("Error retrieving result: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("Student ID %d does not exist.\n", studentId);
        mysql_free_result(result);
        mysql_query(db, "ROLLBACK;");
        return;
    }

    char name[255], email[255], contact[20], address[255], gender[10], institute[255];
    snprintf(name, sizeof(name), "%s", row[1]);
    snprintf(email, sizeof(email), "%s", row[2]);
    snprintf(contact, sizeof(contact), "%s", row[3]);
    snprintf(address, sizeof(address), "%s", row[4]);
    snprintf(gender, sizeof(gender), "%s", row[5]);
    snprintf(institute, sizeof(institute), "%s", row[6]);

    mysql_free_result(result);

    char insertQuery[1024];
    snprintf(insertQuery, sizeof(insertQuery),
        "INSERT INTO PreviousOwners (StudentId, Name, Email, ContactNumber, Address, Gender, Institute, RemovedDate) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', CURDATE());",
        studentId, name, email, contact, address, gender, institute);

    if (mysql_query(db, insertQuery) != 0) {
        printf("Error inserting into PreviousOwners: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    char deleteLeaseQuery[256];
    snprintf(deleteLeaseQuery, sizeof(deleteLeaseQuery), "DELETE FROM LeaseApplication WHERE LeaseApplicationId = %d;", leaseId);

    if (mysql_query(db, deleteLeaseQuery) != 0) {
        printf("Error deleting lease: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    if (mysql_query(db, "COMMIT;") != 0) {
        printf("Error committing transaction: %s\n", mysql_error(db));
        mysql_query(db, "ROLLBACK;");
        return;
    }

    printf("Student ID %d successfully removed from the lease and added to PreviousOwners.\n", studentId);
}

void displayPreviousOwners(MYSQL* db) {

    const char* query = "SELECT * FROM PreviousOwners;";

    if (mysql_query(db, query) != 0) {
        printf("Error retrieving previous owners: %s\n", mysql_error(db));
        return;
    }

    MYSQL_RES* result = mysql_store_result(db);
    if (!result) {
        printf("Error storing result: %s\n", mysql_error(db));
        return;
    }

    int numFields = mysql_num_fields(result);
    MYSQL_ROW row;

    printf("\n%-5s %-10s %-25s %-30s %-15s %-30s %-10s %-25s %-15s\n",
        "ID", "StudentId", "Name", "Email", "ContactNumber",
        "Address", "Gender", "Institute", "RemovedDate");
    printf("------------------------------------------------------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-5s %-10s %-25s %-30s %-15s %-30s %-10s %-25s %-15s\n",
            row[0], row[1], row[2], row[3], row[4],
            row[5], row[6], row[7], row[8]);
    }

    mysql_free_result(result);
}
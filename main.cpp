#include <iostream>
#include "sqlite3.h"
#include <map>
#include <list>
#include <limits>

using namespace std;

struct User {
    string admin_password;

    string name;
    string email;
    string password;
    string userId;
    string passportId;
    string payment;

    string employeeId;
    string employeeName;
    string workspace;

    string plane_id;
    string planeName;

    string flight_id;
    string arrival_place;
    string destination_time;
    string destination_place;
    string arrival_time;

    map<string, list<string>> flight;
    map<string, list<string>> users;
    map<string, list<string>> crew;
    map<string, list<string>> tracks;

    map<string, list<string>> check;

    void displayOptions();

    void registerCustomer();
    void checkIn();
    void bookTicket();

    void crew_management();
    void flight_maintenance();
    void maintenance_tracking();
};

void User::displayOptions() {
    cout << "Welcome to the Airplane Management System" << endl;
    int choice3;
    cout << "1. User or 2. Admin" << endl;
    cin >> choice3;
    if (choice3 == 1) {
        cout << "Do you want to (1) register, (2) check in a ticket, or (3) book a ticket?: " << endl;
        int choice;
        cin >> choice;
        switch (choice) {
            case 1:
                registerCustomer();
                break;
            case 2:
                checkIn();
                break;
            case 3:
                bookTicket();
                break;
            default:
                cout << "Invalid choice" << endl;
        }
    } else if (choice3 == 2) {
        cout << "Enter admin password: ";
        cin >> admin_password;
        if (admin_password == "pass123") {
            int choose;
            cout << "Crew management press 1: " << endl;
            cout << "Maintenance tracking press 2: " << endl;
            cout << "Flight management press 3: " << endl;
            cin >> choose;
            switch (choose) {
                case 1:
                    crew_management();
                    break;
                case 2:
                    maintenance_tracking();
                    break;
                case 3:
                    flight_maintenance();
                    break;
                default:
                    cout << "Unknown choice: ";
            }
        } else {
            cout << "INVALID CHOICE" << endl;
        }
    }
}

void User::registerCustomer() {
    cout << "Enter your name: ";
    cin >> name;
    cout << "Enter your email address: ";
    cin >> email;
    cout << "Enter your password: ";
    cin >> password;
    cout << "Enter your userID: ";
    cin >> userId;
    users[userId] = {name, email, password};

    cout << "If you need transportation outside of Ethiopia, we will require your passport." << endl;
    cout << "Press 1 if you have a passport." << endl;
    cout << "Press 2 if you do not have a passport." << endl;

    int choice;
    cin >> choice;
    string passportId = "";

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice" << endl;
        return;
    }

    if (choice == 1) {
        cout << "Enter your passport id: ";
        cin >> passportId;
        users[userId].push_back(passportId);
    }

    cout << "Registration successful!" << endl;

    sqlite3* db;
    char* zErrMsg = nullptr;
    int rc;
    string sql;

    rc = sqlite3_open("user.db", &db);
    if (rc) {
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Enable foreign key support
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "Can't enable foreign key support: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }

    // Create the REGISTRATION_user table if it doesn't exist
    sql = "CREATE TABLE IF NOT EXISTS REGISTRATION_user("
          "USERID TEXT PRIMARY KEY,"
          "PASSWORD CHAR(50) NOT NULL,"
          "NAME TEXT NOT NULL,"
          "EMAIL CHAR(50) UNIQUE,"
          "PASSPORTID CHAR(50) UNIQUE);";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }

    // Insert the user into the REGISTRATION_user table
    sql = "INSERT INTO REGISTRATION_user (USERID, PASSWORD, NAME, EMAIL, PASSPORTID) "
          "VALUES ('" + userId + "', '" + password + "', '" + name + "', '" + email + "', '" + passportId + "');";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "Registration failed: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void User::checkIn() {
    int userId;  // Ensure userId is an integer
    cout << "Enter your userID: ";
    cin >> userId;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid userID" << endl;
        return;
    }

    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;
    bool found = false;

    rc = sqlite3_open("user.db", &db);
    if (rc != SQLITE_OK) {
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Prepare the SQL query with a placeholder
    string sql = "SELECT USERID FROM REGISTRATION_user WHERE USERID = '" + std::to_string(userId) + "';";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        found = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (found) {
        cout << "You are already registered in the Airplane Management System" << endl;
    } else {
        cout << "You are not registered in the Airplane Management System." << endl;
        cout << "Please register in the Airplane Management System" << endl;

        // Call the registration function
        registerCustomer();

        // Check again if the user is now registered
        rc = sqlite3_open("user.db", &db);
        if (rc != SQLITE_OK) {
            cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            cout << "You are now registered in the Airplane Management System" << endl;
        } else {
            cout << "Registration failed." << endl;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}

void User::bookTicket() {
    string inputEmail;
    cout << "Please enter your email address: ";
    cin >> inputEmail;

    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;
    bool found = false;

    rc = sqlite3_open("user.db", &db);
    if (rc) {
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string sql = "SELECT USERID, NAME, PASSPORTID FROM REGISTRATION_user WHERE EMAIL = '" + inputEmail + "';";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        found = true;
        userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        passportId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    }

    sqlite3_finalize(stmt);

    if (found) {
        int choice;
        cout << "If you want to use local transportation press 1" << endl;
        cout << "If you want to use global transportation press 2" << endl;
        cin >> choice;
        if (choice == 2) {
            string inputPassportId;
            cout << "Enter passport number or id: ";
            cin >> inputPassportId;
            if (inputPassportId != passportId) {
                cout << "Invalid passport ID." << endl;
                sqlite3_close(db);
                return;
            }
        } else if (choice != 1) {
            cout << "Invalid choice" << endl;
            sqlite3_close(db);
            return;
        }

        cout << "Enter arrival place: ";
        cin.ignore();
        getline(cin, arrival_place);
        cout << "Enter destination place: ";
        getline(cin, destination_place);

        sql = "SELECT FLIGHT_ID, ARRIVAL_TIME, DESTINATION_TIME FROM FLIGHT_TABLE WHERE ARRIVAL_PLACE = '" + arrival_place + "' AND DESTINATION_PLACE = '" + destination_place + "';";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        bool flight_found = false;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            string flight_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string arrival_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string destination_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            cout << "Flight ID: " << flight_id << " Arrival Time: " << arrival_time << " Destination Time: " << destination_time << endl;
            flight_found = true;
        }

        if (!flight_found) {
            cout << "No flights available from " << arrival_place << " to " << destination_place << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }

        sqlite3_finalize(stmt);

        cout << "Enter flight ID to book: ";
        string chosen_flight_id;
        cin >> chosen_flight_id;

        cout << "Enter payment: ";
        cin.ignore();
        getline(cin, payment);

        // Check if the booking already exists
        sql = "SELECT COUNT(*) FROM BOOKING WHERE USERID = ? AND NAME = ?";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
            cout << "A booking with this USERID and NAME already exists." << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }

        sqlite3_finalize(stmt);

        // Insert the new booking
        sql = "INSERT INTO BOOKING (USERID, NAME, PAYMENT, FLIGHT_ID) VALUES (?, ?, ?, ?)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, payment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, chosen_flight_id.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cout << "SQL error: " << sqlite3_errmsg(db) << endl;
        } else {
            cout << "Booking successfully created!" << endl;
        }

        sqlite3_finalize(stmt);
    } else {
        cout << "You are not registered in the Airplane Management System." << endl;
        cout << "Please register in the Airplane Management System" << endl;
    }

    sqlite3_close(db);
}

void User::crew_management() {
    char cont;
    do {
        int choose;
        cout << "\nAdmin menu:\n";
        cout << "1. Add employee" << endl;
        cout << "2. Delete employee" << endl;
        cin >> choose;
        cin.ignore();

        if (choose == 1) {
            string employeeId, employeeName, workspace;

            cout << "Enter employee ID: ";
            cin >> employeeId;
            cin.ignore();
            cout << "Enter employee name: ";
            getline(cin, employeeName);
            cout << "Enter employee workspace: ";
            cin >> workspace;

            // Add to local map
            crew[employeeId] = {employeeName, workspace};

            // Add to database
            sqlite3* db;
            int rc;
            char* zErrMsg = nullptr;
            string sql;

            rc = sqlite3_open("user.db", &db);
            if (rc != SQLITE_OK) {
                cout << "Database connection failed: " << sqlite3_errmsg(db) << endl;
                return;
            }

            rc = sqlite3_exec(db, "PRAGMA foreign_keys=ON;", nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "Can't enable foreign key support in crew table: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sql = "CREATE TABLE IF NOT EXISTS CREW_TABLE("
                  "EMPLOYEE_ID TEXT PRIMARY KEY NOT NULL, "
                  "EMPLOY_NAME TEXT NOT NULL, "
                  "WORK_SPACE TEXT NOT NULL);";

            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "SQL error: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sql = "INSERT INTO CREW_TABLE (EMPLOYEE_ID, EMPLOY_NAME, WORK_SPACE) VALUES ('"
                  + employeeId + "', '" + employeeName + "', '" + workspace + "');";

            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "SQL error: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sqlite3_close(db);

            // Display the added employee information
            cout << "Employee added successfully:" << endl;
            cout << "\nEmployee ID\tEmployee Name\tWorkspace" << endl;
            cout << employeeId << "\t\t" << employeeName << "\t\t" << workspace << endl;

        } else if (choose == 2) {
            string employeeId;
            cout << "Enter employee ID to delete: ";
            cin >> employeeId;
            cin.ignore();

            if (crew.find(employeeId) != crew.end()) {
                crew.erase(employeeId);

                // Delete from database
                sqlite3* db;
                int rc;
                char* zErrMsg = nullptr;
                string sql;

                rc = sqlite3_open("user.db", &db);
                if (rc != SQLITE_OK) {
                    cout << "Database connection failed: " << sqlite3_errmsg(db) << endl;
                    return;
                }

                sql = "DELETE FROM CREW_TABLE WHERE EMPLOYEE_ID = '" + employeeId + "';";
                rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cout << "SQL error: " << zErrMsg << endl;
                    sqlite3_free(zErrMsg);
                }

                sqlite3_close(db);
                cout << "Employee with ID " << employeeId << " has been successfully deleted." << endl;
            } else {
                cout << "No employee with ID " << employeeId << " found in the database." << endl;
            }

        } else {
            cout << "Invalid choice. Please try again." << endl;
        }

        cout << "If you want to return to the admin menu, enter Y/n: ";
        cin >> cont;
    } while (cont == 'y' || cont == 'Y');
}

void User::maintenance_tracking() {
    char choice;
    do {
        int ch1;
        cout << "\nAdmin menu:\n";
        cout << "1. Add plane" << endl;
        cout << "2. Delete plane " << endl;
        cin >> ch1;
        cin.ignore();

        if (ch1 == 1) {
            int plane_id;
            string planeName;
            cout << "Enter plane id: ";
            while (!(cin >> plane_id)) {
                cerr << "Invalid plane ID" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter plane id: ";
            }
            cin.ignore(); // Clear the newline character left in the buffer by cin

            cout << "Enter plane name: ";
            getline(cin, planeName);

            // Add to local map
            tracks[to_string(plane_id)] = {planeName};

            // Add to database
            sqlite3* db;
            int rc;
            char* zErrMsg = nullptr;
            string sql;

            rc = sqlite3_open("user.db", &db);
            if (rc != SQLITE_OK) {
                cout << "Database connection failed: " << sqlite3_errmsg(db) << endl;
                return;
            }

            rc = sqlite3_exec(db, "PRAGMA foreign_keys=ON;", nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "Can't enable foreign key support in track table: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sql = "CREATE TABLE IF NOT EXISTS TRACK_TABLE("
                  "PLANE_ID TEXT PRIMARY KEY NOT NULL,"
                  "PLANE_NAME TEXT NOT NULL);";

            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "SQL error: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sql = "INSERT INTO TRACK_TABLE (PLANE_ID, PLANE_NAME) VALUES ('"
                  + to_string(plane_id) + "', '" + planeName + "');";

            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "SQL error: " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
            }

            sqlite3_close(db);

            // Display the added maintenance plan information
            cout << "Plane added successfully:" << endl;
            cout << "\nPlane ID\tPlane Name" << endl;
            cout << plane_id << "\t\t" << planeName << endl;

        } else if (ch1 == 2) {
            int plane_id;
            cout << "Enter plane id to delete: ";
            cin >> plane_id;
            cin.ignore();

            auto it = tracks.find(to_string(plane_id));
            if (it != tracks.end()) {
                tracks.erase(it);

                // Delete from database
                sqlite3* db;
                int rc;
                char* zErrMsg = nullptr;
                string sql;

                rc = sqlite3_open("user.db", &db);
                if (rc != SQLITE_OK) {
                    cout << "Database connection failed: " << sqlite3_errmsg(db) << endl;
                    return;
                }

                sql = "DELETE FROM TRACK_TABLE WHERE PLANE_ID = '" + to_string(plane_id) + "';";
                rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cout << "SQL error: " << zErrMsg << endl;
                    sqlite3_free(zErrMsg);
                }

                sqlite3_close(db);
                cout << "Plane with ID " << plane_id << " has been successfully deleted." << endl;
            } else {
                cout << "No plane with ID " << plane_id << " found in the database." << endl;
            }
        } else {
            cout << "Invalid choice." << endl;
        }

        cout << "If you want to return to the admin menu, enter Y/n: ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');
}

void User::flight_maintenance() {
    char choose;
    do {
        int choose1;
        cout << "\nAdmin menu:\n";
        cout << "1. Add flight" << endl;
        cout << "2. Delete flight" << endl;
        cin >> choose1;
        cin.ignore();

        sqlite3* db;
        sqlite3_stmt* stmt;
        int rc = sqlite3_open("user.db", &db);
        if (rc != SQLITE_OK) {
            cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
            return;
        }

        if (choose1 == 1) {
            string flight_id, arrival_time, arrival_place, destination_time, destination_place;
            int plane_id;

            cout << "Enter flight id: ";
            getline(cin, flight_id);
            cout << "Arrival time: ";
            getline(cin, arrival_time);
            cout << "Arrival place: ";
            getline(cin, arrival_place);
            cout << "Destination time: ";
            getline(cin, destination_time);
            cout << "Destination place: ";
            getline(cin, destination_place);
            cout << "Enter plane id: ";
            cin >> plane_id;
            cin.ignore();

            string sql = "SELECT PLANE_ID FROM TRACK_TABLE WHERE PLANE_ID = '" + std::to_string(plane_id) + "';";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW) {
                cerr << "Invalid plane id. Please enter a correct plane id." << endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return;
            }

            sqlite3_finalize(stmt);

            sql = "CREATE TABLE IF NOT EXISTS FLIGHT_TABLE ("
                  "FLIGHT_ID TEXT PRIMARY KEY NOT NULL, "
                  "PLANE_ID TEXT NOT NULL, "
                  "ARRIVAL_TIME TEXT NOT NULL, "
                  "ARRIVAL_PLACE TEXT NOT NULL, "
                  "DESTINATION_TIME TEXT NOT NULL, "
                  "DESTINATION_PLACE TEXT NOT NULL, "
                  "FOREIGN KEY(PLANE_ID) REFERENCES TRACK_TABLE(PLANE_ID)"
                  ");";
            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK) {
                cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            sql = "INSERT OR REPLACE INTO FLIGHT_TABLE (FLIGHT_ID, PLANE_ID, ARRIVAL_TIME, ARRIVAL_PLACE, DESTINATION_TIME, DESTINATION_PLACE) "
                  "VALUES (?, ?, ?, ?, ?, ?);";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }
            sqlite3_bind_text(stmt, 1, flight_id.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, to_string(plane_id).c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, arrival_time.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, arrival_place.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, destination_time.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 6, destination_place.c_str(), -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                cerr << "Insertion failed: " << sqlite3_errmsg(db) << endl;
            } else {
                cout << "Flight added successfully with Plane ID: " << plane_id << endl;
            }

            sqlite3_finalize(stmt);
            sqlite3_close(db);
        } else if (choose1 == 2) {
            string flight_id;
            cout << "Enter flight id: ";
            getline(cin, flight_id);

            string sql = "SELECT FLIGHT_ID FROM FLIGHT_TABLE WHERE FLIGHT_ID = ?;";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            sqlite3_bind_text(stmt, 1, flight_id.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_ROW) {
                cerr << "There is no such flight id." << endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return;
            }

            sqlite3_finalize(stmt);

            sql = "DELETE FROM FLIGHT_TABLE WHERE FLIGHT_ID = ?;";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                return;
            }

            sqlite3_bind_text(stmt, 1, flight_id.c_str(), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                cerr << "Deletion failed: " << sqlite3_errmsg(db) << endl;
            } else {
                cout << "Flight deleted successfully." << endl;
            }

            sqlite3_finalize(stmt);
            sqlite3_close(db);
        } else {
            cout << "Invalid choice." << endl;
        }

        cout << "If you want to return to the admin menu, enter Y/n: ";
        cin >> choose;
    } while (choose == 'y' || choose == 'Y');
}

int main() {
    User user;
    char choice = 'y';
    while (choice == 'y' || choice == 'Y') {
        user.displayOptions();
        cout << "Do you want to continue? y/n: ";
        cin >> choice;
        cin.ignore();
    }
    return 0;
}

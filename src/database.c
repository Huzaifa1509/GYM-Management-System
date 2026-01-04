#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

// ============================================
// Global Database Handle
// ============================================

static sqlite3 *db = NULL;

// ============================================
// Database Initialization
// ============================================

// Initialize database and create tables
int db_init() {
    int rc = sqlite3_open("database/gym.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    const char *sql_users = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "role TEXT NOT NULL,"
        "verified INTEGER DEFAULT 0);"
    ;

    const char *sql_members = 
        "CREATE TABLE IF NOT EXISTS Members ("
        "member_id INTEGER PRIMARY KEY,"
        "plan_id INTEGER,"
        "trainer_id INTEGER,"
        "time_slot TEXT,"
        "status TEXT,"
        "FOREIGN KEY(member_id) REFERENCES Users(user_id));"
    ;

    const char *sql_trainers = 
        "CREATE TABLE IF NOT EXISTS Trainers ("
        "trainer_id INTEGER PRIMARY KEY,"
        "specialization TEXT,"
        "status TEXT,"
        "FOREIGN KEY(trainer_id) REFERENCES Users(user_id));"
    ;

    const char *sql_plans = 
        "CREATE TABLE IF NOT EXISTS Plans ("
        "plan_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "price REAL,"
        "time_slot TEXT);"
    ;

    const char *sql_attendance = 
        "CREATE TABLE IF NOT EXISTS Attendance ("
        "attendance_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "member_id INTEGER,"
        "date TEXT,"
        "status TEXT,"
        "FOREIGN KEY(member_id) REFERENCES Members(member_id));"
    ;

    char *errMsg = 0;
    
    // Execute table creation
    if (sqlite3_exec(db, sql_users, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Users): %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    if (sqlite3_exec(db, sql_members, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Members): %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    if (sqlite3_exec(db, sql_trainers, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Trainers): %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    if (sqlite3_exec(db, sql_plans, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Plans): %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    if (sqlite3_exec(db, sql_attendance, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Attendance): %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    // Seed default plans
    const char *sql_seed_plans = 
        "INSERT OR IGNORE INTO Plans (plan_id, name, price, time_slot) VALUES "
        "(1, 'Basic (Morning)', 30.0, 'Morning'),"
        "(2, 'Standard (Evening)', 50.0, 'Evening'),"
        "(3, 'Premium (Anytime)', 80.0, 'Full Day');";
        
    if (sqlite3_exec(db, sql_seed_plans, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error (Seed Plans): %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    // Seed default admin account
    const char *sql_seed_admin = 
        "INSERT OR IGNORE INTO Users (user_id, name, email, password, role, verified) VALUES "
        "(1, 'Admin', 'admin@gym.com', 'admin123', 'Admin', 1);";

    if (sqlite3_exec(db, sql_seed_admin, 0, 0, &errMsg) != SQLITE_OK) {
         fprintf(stderr, "SQL error (Seed Admin): %s\n", errMsg);
         sqlite3_free(errMsg);
    }

    return 0;
}

// Get database handle
sqlite3* db_get_handle() {
    return db;
}

// Close database connection
void db_close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

// ============================================
// User Management Functions
// ============================================

// Create a new user in the database
int db_create_user(User *user) {
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO Users (name, email, password, role, verified) VALUES ('%s', '%s', '%s', '%s', %d);",
        user->name, user->email, user->password, user->role, user->verified);
    
    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Error creating user: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    user->user_id = (int)sqlite3_last_insert_rowid(db);
    return 0;
}

// Get user by email address
int db_get_user_by_email(const char *email, User *user) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM Users WHERE email='%s';", email);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        return 1;
    }
    
    int result = 1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user->user_id = sqlite3_column_int(stmt, 0);
        snprintf(user->name, sizeof(user->name), "%s", sqlite3_column_text(stmt, 1));
        snprintf(user->email, sizeof(user->email), "%s", sqlite3_column_text(stmt, 2));
        snprintf(user->password, sizeof(user->password), "%s", sqlite3_column_text(stmt, 3));
        snprintf(user->role, sizeof(user->role), "%s", sqlite3_column_text(stmt, 4));
        user->verified = sqlite3_column_int(stmt, 5);
        result = 0;
    }
    
    sqlite3_finalize(stmt);
    return result;
}

// Mark user as verified
int db_verify_user(const char *email) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE Users SET verified=1 WHERE email='%s';", email);
    
    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Error verifying user: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    return 0;
}

// Authenticate user login
int db_login_user(const char *email, const char *password, User *user) {
    if (db_get_user_by_email(email, user) != 0) {
        return 1; // User not found
    }
    if (strcmp(user->password, password) == 0) {
        return 0; // Success
    }
    return 2; // Wrong password
}

// ============================================
// Member Management Functions
// ============================================

// Get member information by user ID
int db_get_member(int user_id, Member *member) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM Members WHERE member_id=%d;", user_id);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int result = 1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        member->member_id = sqlite3_column_int(stmt, 0);
        member->plan_id = sqlite3_column_int(stmt, 1);
        member->trainer_id = sqlite3_column_int(stmt, 2);
        snprintf(member->time_slot, sizeof(member->time_slot), "%s", sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "");
        snprintf(member->status, sizeof(member->status), "%s", sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "");
        result = 0;
    }
    sqlite3_finalize(stmt);
    return result;
}

// Create a new member record
int db_create_member(int user_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT OR IGNORE INTO Members (member_id) VALUES (%d);", user_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// Update member's plan and time slot
int db_update_member_plan(int member_id, int plan_id, const char *time_slot) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE Members SET plan_id=%d, time_slot='%s' WHERE member_id=%d;", plan_id, time_slot, member_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// Assign a trainer to a member
int db_assign_trainer(int member_id, int trainer_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE Members SET trainer_id=%d WHERE member_id=%d;", trainer_id, member_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// ============================================
// Trainer Management Functions
// ============================================

// Create a new trainer record
int db_create_trainer(int user_id, const char *specialization) {
    char sql[512];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO Trainers (trainer_id, specialization, status) VALUES (%d, '%s', 'PENDING_APPROVAL');",
        user_id, specialization);
    
    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Error creating trainer: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }
    return 0;
}

// ============================================
// Data Retrieval Functions
// ============================================

// Get all available plans
int db_get_plans(Plan *plans, int *count) {
    const char *sql = "SELECT * FROM Plans;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < *count) {
        plans[i].plan_id = sqlite3_column_int(stmt, 0);
        snprintf(plans[i].name, sizeof(plans[i].name), "%s", sqlite3_column_text(stmt, 1));
        plans[i].price = sqlite3_column_double(stmt, 2);
        snprintf(plans[i].time_slot, sizeof(plans[i].time_slot), "%s", sqlite3_column_text(stmt, 3));
        i++;
    }
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

// Get available trainers for a time slot
int db_get_available_trainers(const char *time_slot, Trainer *trainers, int *count) {
    // For simplicity, return all approved trainers. Real logic would check schedule.
    // Also need to join with Users to get names, but Trainer struct only has ID/Spec.
    // Let's assume we just get IDs for now.
    const char *sql = "SELECT * FROM Trainers WHERE status='APPROVED';";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < *count) {
        trainers[i].trainer_id = sqlite3_column_int(stmt, 0);
        snprintf(trainers[i].specialization, sizeof(trainers[i].specialization), "%s", sqlite3_column_text(stmt, 1));
        snprintf(trainers[i].status, sizeof(trainers[i].status), "%s", sqlite3_column_text(stmt, 2));
        i++;
    }
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

// ============================================
// Admin Functions
// ============================================

// Get all pending trainer applications
int db_get_pending_trainers(TrainerDetail *trainers, int *count) {
    const char *sql = "SELECT t.trainer_id, u.name, u.email, t.specialization, t.status FROM Trainers t JOIN Users u ON t.trainer_id = u.user_id WHERE t.status='PENDING_APPROVAL';";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < *count) {
        trainers[i].trainer_id = sqlite3_column_int(stmt, 0);
        snprintf(trainers[i].name, sizeof(trainers[i].name), "%s", sqlite3_column_text(stmt, 1));
        snprintf(trainers[i].email, sizeof(trainers[i].email), "%s", sqlite3_column_text(stmt, 2));
        snprintf(trainers[i].specialization, sizeof(trainers[i].specialization), "%s", sqlite3_column_text(stmt, 3));
        snprintf(trainers[i].status, sizeof(trainers[i].status), "%s", sqlite3_column_text(stmt, 4));
        i++;
    }
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

// Approve a trainer application
int db_approve_trainer(int trainer_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE Trainers SET status='APPROVED' WHERE trainer_id=%d;", trainer_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// Reject a trainer application (deletes user)
int db_reject_trainer(int trainer_id) {
    // Delete from Trainers and Users
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM Trainers WHERE trainer_id=%d;", trainer_id);
    sqlite3_exec(db, sql, 0, 0, 0);
    snprintf(sql, sizeof(sql), "DELETE FROM Users WHERE user_id=%d;", trainer_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// Get all members with details
int db_get_all_members_detail(MemberDetail *members, int *count) {
    const char *sql = "SELECT m.member_id, u.name, u.email, p.name, m.status FROM Members m JOIN Users u ON m.member_id = u.user_id LEFT JOIN Plans p ON m.plan_id = p.plan_id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < *count) {
        members[i].member_id = sqlite3_column_int(stmt, 0);
        snprintf(members[i].name, sizeof(members[i].name), "%s", sqlite3_column_text(stmt, 1));
        snprintf(members[i].email, sizeof(members[i].email), "%s", sqlite3_column_text(stmt, 2));
        snprintf(members[i].plan_name, sizeof(members[i].plan_name), "%s", sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "None");
        snprintf(members[i].status, sizeof(members[i].status), "%s", sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "");
        i++;
    }
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

// Get all trainers with details
int db_get_all_trainers_detail(TrainerDetail *trainers, int *count) {
    const char *sql = "SELECT t.trainer_id, u.name, u.email, t.specialization, t.status FROM Trainers t JOIN Users u ON t.trainer_id = u.user_id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 1;
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < *count) {
        trainers[i].trainer_id = sqlite3_column_int(stmt, 0);
        snprintf(trainers[i].name, sizeof(trainers[i].name), "%s", sqlite3_column_text(stmt, 1));
        snprintf(trainers[i].email, sizeof(trainers[i].email), "%s", sqlite3_column_text(stmt, 2));
        snprintf(trainers[i].specialization, sizeof(trainers[i].specialization), "%s", sqlite3_column_text(stmt, 3));
        snprintf(trainers[i].status, sizeof(trainers[i].status), "%s", sqlite3_column_text(stmt, 4));
        i++;
    }
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

// Delete a member
int db_delete_member(int member_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM Members WHERE member_id=%d;", member_id);
    sqlite3_exec(db, sql, 0, 0, 0);
    snprintf(sql, sizeof(sql), "DELETE FROM Users WHERE user_id=%d;", member_id);
    return sqlite3_exec(db, sql, 0, 0, 0);
}

// Delete a trainer
int db_delete_trainer(int trainer_id) {
    return db_reject_trainer(trainer_id);
}

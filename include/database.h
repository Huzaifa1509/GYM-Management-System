#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "models.h"

int db_init();
sqlite3* db_get_handle();
void db_close();

// User Management
int db_create_user(User *user);
int db_get_user_by_email(const char *email, User *user);
int db_verify_user(const char *email);
int db_login_user(const char *email, const char *password, User *user);

// Member Management
int db_get_member(int user_id, Member *member);
int db_create_member(int user_id);
int db_update_member_plan(int member_id, int plan_id, const char *time_slot);
int db_assign_trainer(int member_id, int trainer_id);

// Trainer Management
int db_create_trainer(int user_id, const char *specialization);

// Data Retrieval
int db_get_plans(Plan *plans, int *count); // Assumes caller allocates enough or we use dynamic array
int db_get_available_trainers(const char *time_slot, Trainer *trainers, int *count);

// Admin Functions
int db_get_pending_trainers(TrainerDetail *trainers, int *count);
int db_approve_trainer(int trainer_id);
int db_reject_trainer(int trainer_id); // Deletes user
int db_get_all_members_detail(MemberDetail *members, int *count);
int db_get_all_trainers_detail(TrainerDetail *trainers, int *count);
int db_delete_member(int member_id);
int db_delete_trainer(int trainer_id);

#endif

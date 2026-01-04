#ifndef MODELS_H
#define MODELS_H

typedef struct {
    int user_id;
    char name[100];
    char email[100];
    char password[100];
    char role[20]; // either  "Admin" ya to "Member" ya to "Trainer"
    int verified;
} User;

typedef struct {
    int member_id;
    int plan_id;
    int trainer_id;
    char time_slot[50];
    char status[50];
} Member;

typedef struct {
    int trainer_id;
    char specialization[100];
    char status[50]; // "PENDING_APPROVAL" ya to "APPROVED"
} Trainer;

typedef struct {
    int plan_id;
    char name[100];
    double price;
    char time_slot[50];
} Plan;

typedef struct {
    int member_id;
    char name[100];
    char email[100];
    char plan_name[100];
    char status[50];
} MemberDetail;

typedef struct {
    int trainer_id;
    char name[100];
    char email[100];
    char specialization[100];
    char status[50];
} TrainerDetail;

#endif

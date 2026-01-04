#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "login.h"
#include "database.h"
#include "models.h"
#include "member.h"
#include "admin.h"

// Widgets
static GtkWidget *window;
static GtkWidget *stack;
static GtkWidget *login_grid, *register_grid, *verify_grid;

// Login Widgets
static GtkWidget *login_email_entry, *login_pass_entry;

// Register Widgets
static GtkWidget *reg_name_entry, *reg_email_entry, *reg_pass_entry, *reg_role_combo;

// Verify Widgets
static GtkWidget *verify_code_entry;
static char current_verifying_email[100];

// Forward declarations
void on_login_clicked(GtkButton *button, gpointer user_data);
void on_register_link_clicked(GtkButton *button, gpointer user_data);
void on_register_submit_clicked(GtkButton *button, gpointer user_data);
void on_verify_submit_clicked(GtkButton *button, gpointer user_data);
void on_back_to_login_clicked(GtkButton *button, gpointer user_data);

void show_message(const char *msg) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Handle login button click
void on_login_clicked(GtkButton *button, gpointer user_data) {
    const char *email = gtk_entry_get_text(GTK_ENTRY(login_email_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(login_pass_entry));
    
    User user;
    int res = db_login_user(email, password, &user);
    
    if (res == 0) {
        // Check if account is verified
        if (user.verified == 0) {
            show_message("Account not verified! Please verify.");
            strcpy(current_verifying_email, email);
            gtk_stack_set_visible_child(GTK_STACK(stack), verify_grid);
        } else {
            char buf[256];
            snprintf(buf, sizeof(buf), "Welcome %s!", user.name);
            show_message(buf);
            
            // Hide login window
            gtk_widget_hide(window);
            
            // Route to appropriate dashboard
            if (strcmp(user.role, "Member") == 0) {
                show_member_dashboard(&user);
            } else if (strcmp(user.role, "Admin") == 0) {
                show_admin_dashboard(&user);
            } else {
                show_message("Trainer Dashboard not implemented yet.");
                gtk_widget_show(window);
            }
        }
    } else if (res == 1) {
        show_message("User not found.");
    } else {
        show_message("Wrong password.");
    }
}

void on_register_link_clicked(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child(GTK_STACK(stack), register_grid);
}

// Handle registration submission
void on_register_submit_clicked(GtkButton *button, gpointer user_data) {
    User user;
    strncpy(user.name, gtk_entry_get_text(GTK_ENTRY(reg_name_entry)), sizeof(user.name));
    strncpy(user.email, gtk_entry_get_text(GTK_ENTRY(reg_email_entry)), sizeof(user.email));
    strncpy(user.password, gtk_entry_get_text(GTK_ENTRY(reg_pass_entry)), sizeof(user.password));
    user.verified = 0;
    
    // Get selected role from dropdown
    user.role[0] = '\0';
    const char *active_id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(reg_role_combo));
    if (active_id) {
        strncpy(user.role, active_id, sizeof(user.role));
    } else {
        show_message("Please select a role.");
        return;
    }

    // Create user in database
    if (db_create_user(&user) == 0) {
        // If registering as trainer, create trainer record
        if (strcmp(user.role, "Trainer") == 0) {
            // For now, use a default specialization. In a real app, you'd prompt for this.
            db_create_trainer(user.user_id, "General Fitness");
        } else if (strcmp(user.role, "Member") == 0) {
            // Create member record
            db_create_member(user.user_id);
        }
        
        show_message("Registration Successful! Please verify with code 1234.");
        strcpy(current_verifying_email, user.email);
        gtk_stack_set_visible_child(GTK_STACK(stack), verify_grid);
    } else {
        show_message("Registration Failed. Email might be taken.");
    }
}

// Handle email verification
void on_verify_submit_clicked(GtkButton *button, gpointer user_data) {
    const char *code = gtk_entry_get_text(GTK_ENTRY(verify_code_entry));
    
    // Check verification code (simulated - always use 1234)
    if (strcmp(code, "1234") == 0) {
        db_verify_user(current_verifying_email);
        show_message("Verification Successful! Please Login.");
        gtk_stack_set_visible_child(GTK_STACK(stack), login_grid);
    } else {
        show_message("Invalid Code. Use 1234.");
    }
}

void on_back_to_login_clicked(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child(GTK_STACK(stack), login_grid);
}

// ============================================
// UI Creation Functions
// ============================================

// Create login screen
GtkWidget* create_login_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *lbl_email = gtk_label_new("Email:");
    login_email_entry = gtk_entry_new();
    GtkWidget *lbl_pass = gtk_label_new("Password:");
    login_pass_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(login_pass_entry), FALSE);
    
    GtkWidget *btn_login = gtk_button_new_with_label("Login");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_login_clicked), NULL);
    
    GtkWidget *btn_reg = gtk_button_new_with_label("Register");
    g_signal_connect(btn_reg, "clicked", G_CALLBACK(on_register_link_clicked), NULL);

    gtk_grid_attach(GTK_GRID(grid), lbl_email, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login_email_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), lbl_pass, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login_pass_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_login, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_reg, 0, 3, 2, 1);

    return grid;
}

// Create registration screen
GtkWidget* create_register_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    reg_name_entry = gtk_entry_new();
    reg_email_entry = gtk_entry_new();
    reg_pass_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(reg_pass_entry), FALSE);
    
    reg_role_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(reg_role_combo), "Member", "Member");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(reg_role_combo), "Trainer", "Trainer");
    gtk_combo_box_set_active(GTK_COMBO_BOX(reg_role_combo), 0);

    GtkWidget *btn_submit = gtk_button_new_with_label("Register");
    g_signal_connect(btn_submit, "clicked", G_CALLBACK(on_register_submit_clicked), NULL);
    
    GtkWidget *btn_back = gtk_button_new_with_label("Back to Login");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_to_login_clicked), NULL);

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Name:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reg_name_entry, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Email:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reg_email_entry, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Password:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reg_pass_entry, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Role:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reg_role_combo, 1, row++, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_submit, 0, row++, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, row++, 2, 1);

    return grid;
}

// Create verification screen
GtkWidget* create_verify_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    verify_code_entry = gtk_entry_new();
    GtkWidget *btn_verify = gtk_button_new_with_label("Verify");
    g_signal_connect(btn_verify, "clicked", G_CALLBACK(on_verify_submit_clicked), NULL);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Enter Verification Code (1234):"), 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), verify_code_entry, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_verify, 0, 2, 2, 1);

    return grid;
}

// Return to login screen (for logout)
void return_to_login() {
    // Clear entry fields
    gtk_entry_set_text(GTK_ENTRY(login_email_entry), "");
    gtk_entry_set_text(GTK_ENTRY(login_pass_entry), "");
    
    // Show login screen
    gtk_stack_set_visible_child(GTK_STACK(stack), login_grid);
    gtk_widget_show(window);
}

// Initialize and show the main login window
void show_login_window() {
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Gym Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create Stack
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    login_grid = create_login_grid();
    register_grid = create_register_grid();
    verify_grid = create_verify_grid();

    gtk_stack_add_named(GTK_STACK(stack), login_grid, "login");
    gtk_stack_add_named(GTK_STACK(stack), register_grid, "register");
    gtk_stack_add_named(GTK_STACK(stack), verify_grid, "verify");

    gtk_container_add(GTK_CONTAINER(window), stack);
    gtk_widget_show_all(window);
}

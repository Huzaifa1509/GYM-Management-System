#include <gtk/gtk.h>
#include <stdio.h>
#include "member.h"
#include "database.h"
#include "login.h"

// ============================================
// Global State
// ============================================

static GtkWidget *window;
static GtkWidget *stack;
static User current_user;
static Member current_member;

// UI Grids
static GtkWidget *plan_grid;
static GtkWidget *time_grid;
static GtkWidget *trainer_grid;
static GtkWidget *dashboard_grid;

// User Selections
static int selected_plan_id = 0;
static char selected_time_slot[50];

// Forward declaration
void refresh_dashboard();

// ============================================
// Event Handlers
// ============================================

// Handle plan selection
void on_plan_selected(GtkButton *button, gpointer data) {
    selected_plan_id = GPOINTER_TO_INT(data);
    gtk_stack_set_visible_child(GTK_STACK(stack), time_grid);
}

// Handle time slot selection
void on_time_selected(GtkButton *button, gpointer data) {
    const char *slot = (const char *)data;
    snprintf(selected_time_slot, sizeof(selected_time_slot), "%s", slot);
    
    // Save plan and time slot to database
    db_update_member_plan(current_member.member_id, selected_plan_id, selected_time_slot);
    
    // Refresh member data
    db_get_member(current_user.user_id, &current_member);
    
    // Move to trainer selection
    gtk_stack_set_visible_child(GTK_STACK(stack), trainer_grid);
}

// Handle trainer selection
void on_trainer_selected(GtkButton *button, gpointer data) {
    int trainer_id = GPOINTER_TO_INT(data);
    db_assign_trainer(current_member.member_id, trainer_id);
    
    // Refresh member data
    db_get_member(current_user.user_id, &current_member);
    
    // Show dashboard
    refresh_dashboard();
    gtk_stack_set_visible_child(GTK_STACK(stack), dashboard_grid);
}

// Handle logout
static void on_logout_clicked(GtkButton *button, gpointer data) {
    gtk_widget_destroy(window);
    return_to_login();
}

// ============================================
// UI Creation Functions
// ============================================

// Create plan selection screen
GtkWidget* create_plan_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *lbl = gtk_label_new("Select a Plan:");
    gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 2, 1);

    Plan plans[10];
    int count = 10;
    db_get_plans(plans, &count);

    for (int i = 0; i < count; i++) {
        char label[100];
        snprintf(label, sizeof(label), "%s - $%.2f", plans[i].name, plans[i].price);
        GtkWidget *btn = gtk_button_new_with_label(label);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_plan_selected), GINT_TO_POINTER(plans[i].plan_id));
        gtk_grid_attach(GTK_GRID(grid), btn, 0, i + 1, 2, 1);
    }

    return grid;
}

// Create time slot selection screen
GtkWidget* create_time_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *lbl = gtk_label_new("Select Time Slot:");
    gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 1, 1);

    const char *slots[] = {"Morning (6-10)", "Evening (5-9)", "Full Day"};
    for (int i = 0; i < 3; i++) {
        GtkWidget *btn = gtk_button_new_with_label(slots[i]);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_time_selected), (gpointer)slots[i]);
        gtk_grid_attach(GTK_GRID(grid), btn, 0, i + 1, 1, 1);
    }

    return grid;
}

// Create trainer selection screen
GtkWidget* create_trainer_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *lbl = gtk_label_new("Select a Trainer:");
    gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 1, 1);

    Trainer trainers[10];
    int count = 10;
    db_get_available_trainers(selected_time_slot, trainers, &count);

    if (count == 0) {
        gtk_grid_attach(GTK_GRID(grid), gtk_label_new("No trainers available."), 0, 1, 1, 1);
    }

    for (int i = 0; i < count; i++) {
        char label[100];
        snprintf(label, sizeof(label), "Trainer ID: %d (%s)", trainers[i].trainer_id, trainers[i].specialization);
        GtkWidget *btn = gtk_button_new_with_label(label);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_trainer_selected), GINT_TO_POINTER(trainers[i].trainer_id));
        gtk_grid_attach(GTK_GRID(grid), btn, 0, i + 1, 1, 1);
    }

    return grid;
}

// Create member dashboard screen
GtkWidget* create_dashboard_grid() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    
    // Content populated in refresh_dashboard()
    return grid;
}

// Update dashboard with current member information
void refresh_dashboard() {
    // Clear existing widgets
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(dashboard_grid));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    char buf[256];
    snprintf(buf, sizeof(buf), "Welcome %s!", current_user.name);
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new(buf), 0, 0, 2, 1);
    
    snprintf(buf, sizeof(buf), "Plan ID: %d | Time: %s", current_member.plan_id, current_member.time_slot);
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new(buf), 0, 1, 2, 1);
    
    snprintf(buf, sizeof(buf), "Trainer ID: %d", current_member.trainer_id);
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new(buf), 0, 2, 2, 1);

    // Weekly Workout Plan
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new("<b>Weekly Workout:</b>"), 0, 3, 2, 1);
    gtk_label_set_use_markup(GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(dashboard_grid), 0, 3)), TRUE);
    
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new("Mon: Chest"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new("Tue: Back"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(dashboard_grid), gtk_label_new("Wed: Legs"), 0, 6, 1, 1);
    
    // Attendance
    GtkWidget *btn_checkin = gtk_button_new_with_label("Check-In (Attendance)");
    // g_signal_connect(btn_checkin, "clicked", ...); // TODO
    gtk_grid_attach(GTK_GRID(dashboard_grid), btn_checkin, 0, 7, 2, 1);
    
    // Logout button
    GtkWidget *btn_logout = gtk_button_new_with_label("Logout");
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_grid_attach(GTK_GRID(dashboard_grid), btn_logout, 0, 8, 2, 1);
    
    gtk_widget_show_all(dashboard_grid);
}

// Initialize and show member dashboard
void show_member_dashboard(User *user) {
    current_user = *user;
    
    // Ensure member record exists
    if (db_get_member(user->user_id, &current_member) != 0) {
        db_create_member(user->user_id);
        db_get_member(user->user_id, &current_member);
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Member Dashboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(on_logout_clicked), NULL);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    plan_grid = create_plan_grid();
    time_grid = create_time_grid();
    trainer_grid = create_trainer_grid();
    dashboard_grid = create_dashboard_grid();
    
    gtk_stack_add_named(GTK_STACK(stack), plan_grid, "plan");
    gtk_stack_add_named(GTK_STACK(stack), time_grid, "time");
    gtk_stack_add_named(GTK_STACK(stack), trainer_grid, "trainer");
    gtk_stack_add_named(GTK_STACK(stack), dashboard_grid, "dashboard");

    // Determine initial view
    if (current_member.plan_id == 0) {
        gtk_stack_set_visible_child(GTK_STACK(stack), plan_grid);
    } else if (strlen(current_member.time_slot) == 0) {
        gtk_stack_set_visible_child(GTK_STACK(stack), time_grid);
    } else if (current_member.trainer_id == 0) {
        // Refresh trainer grid to ensure it uses selected time slot if we jump here
        // But for simplicity, let's assume if time is set, we might need to select trainer.
        // If trainer is 0, show trainer selection.
        snprintf(selected_time_slot, sizeof(selected_time_slot), "%s", current_member.time_slot);
        // Re-create trainer grid to filter by time slot
        gtk_container_remove(GTK_CONTAINER(stack), trainer_grid);
        trainer_grid = create_trainer_grid();
        gtk_stack_add_named(GTK_STACK(stack), trainer_grid, "trainer");
        gtk_stack_set_visible_child(GTK_STACK(stack), trainer_grid);
    } else {
        refresh_dashboard();
        gtk_stack_set_visible_child(GTK_STACK(stack), dashboard_grid);
    }

    gtk_container_add(GTK_CONTAINER(window), stack);
    gtk_widget_show_all(window);
}

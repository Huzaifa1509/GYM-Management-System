#include <gtk/gtk.h>
#include <stdio.h>
#include "admin.h"
#include "database.h"
#include "login.h"

// ============================================
// Global State
// ============================================

static GtkWidget *window;
static GtkWidget *notebook;
static GtkWidget *trainers_list;
static GtkWidget *members_list;
static GtkWidget *pending_trainers_list;

// ============================================
// Helper Functions
// ============================================

// Add a column to tree view
void add_column(GtkWidget *treeview, const char *title, int columnId) {
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(title, renderer, "text", columnId, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

// ============================================
// Data Refresh Functions
// ============================================

// Refresh pending trainers list
void refresh_pending_trainers() {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(pending_trainers_list)));
    gtk_list_store_clear(store);
    
    TrainerDetail trainers[20];
    int count = 20;
    db_get_pending_trainers(trainers, &count);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, trainers[i].trainer_id, 1, trainers[i].name, 2, trainers[i].specialization, -1);
    }
}

// Refresh members list
void refresh_members() {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(members_list)));
    gtk_list_store_clear(store);
    
    MemberDetail members[50];
    int count = 50;
    db_get_all_members_detail(members, &count);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, members[i].member_id, 1, members[i].name, 2, members[i].plan_name, 3, members[i].status, -1);
    }
}

// Refresh trainers list
void refresh_trainers() {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(trainers_list)));
    gtk_list_store_clear(store);
    
    TrainerDetail trainers[20];
    int count = 20;
    db_get_all_trainers_detail(trainers, &count);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, trainers[i].trainer_id, 1, trainers[i].name, 2, trainers[i].specialization, 3, trainers[i].status, -1);
    }
}

// ============================================
// Event Handlers
// ============================================

// Approve a pending trainer
void on_approve_trainer(GtkButton *button, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pending_trainers_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        db_approve_trainer(id);
        refresh_pending_trainers();
        refresh_trainers();
    }
}

// Reject a pending trainer
void on_reject_trainer(GtkButton *button, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pending_trainers_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        db_reject_trainer(id);
        refresh_pending_trainers();
    }
}

// Delete a member
void on_delete_member(GtkButton *button, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(members_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        db_delete_member(id);
        refresh_members();
    }
}

// Fire a trainer
void on_fire_trainer(GtkButton *button, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(trainers_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        db_delete_trainer(id);
        refresh_trainers();
        refresh_pending_trainers();
    }
}

// Handle logout
static void on_logout_clicked(GtkButton *button, gpointer data) {
    gtk_widget_destroy(window);
    return_to_login();
}

// ============================================
// UI Creation Functions
// ============================================

// Create pending trainers tab
GtkWidget* create_pending_trainers_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    pending_trainers_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    add_column(pending_trainers_list, "ID", 0);
    add_column(pending_trainers_list, "Name", 1);
    add_column(pending_trainers_list, "Specialization", 2);
    
    gtk_box_pack_start(GTK_BOX(vbox), pending_trainers_list, TRUE, TRUE, 0);
    
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *btn_approve = gtk_button_new_with_label("Approve");
    g_signal_connect(btn_approve, "clicked", G_CALLBACK(on_approve_trainer), NULL);
    GtkWidget *btn_reject = gtk_button_new_with_label("Reject");
    g_signal_connect(btn_reject, "clicked", G_CALLBACK(on_reject_trainer), NULL);
    
    gtk_box_pack_start(GTK_BOX(hbox), btn_approve, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn_reject, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    
    refresh_pending_trainers();
    return vbox;
}

// Create members management tab
GtkWidget* create_members_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    members_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    add_column(members_list, "ID", 0);
    add_column(members_list, "Name", 1);
    add_column(members_list, "Plan", 2);
    add_column(members_list, "Status", 3);
    
    gtk_box_pack_start(GTK_BOX(vbox), members_list, TRUE, TRUE, 0);
    
    GtkWidget *btn_delete = gtk_button_new_with_label("Cancel Membership");
    g_signal_connect(btn_delete, "clicked", G_CALLBACK(on_delete_member), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), btn_delete, FALSE, FALSE, 0);
    
    refresh_members();
    return vbox;
}

// Create trainers management tab
GtkWidget* create_trainers_tab() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    trainers_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    add_column(trainers_list, "ID", 0);
    add_column(trainers_list, "Name", 1);
    add_column(trainers_list, "Specialization", 2);
    add_column(trainers_list, "Status", 3);
    
    gtk_box_pack_start(GTK_BOX(vbox), trainers_list, TRUE, TRUE, 0);
    
    GtkWidget *btn_fire = gtk_button_new_with_label("Fire Trainer");
    g_signal_connect(btn_fire, "clicked", G_CALLBACK(on_fire_trainer), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), btn_fire, FALSE, FALSE, 0);
    
    refresh_trainers();
    return vbox;
}

// Initialize and show admin dashboard
void show_admin_dashboard(User *user) {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Admin Dashboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(on_logout_clicked), NULL);
    
    // Create main container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    notebook = gtk_notebook_new();
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_pending_trainers_tab(), gtk_label_new("Pending Trainers"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_members_tab(), gtk_label_new("Members"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_trainers_tab(), gtk_label_new("All Trainers"));
    
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    
    // Add logout button
    GtkWidget *btn_logout = gtk_button_new_with_label("Logout");
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), btn_logout, FALSE, FALSE, 5);
    
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
}

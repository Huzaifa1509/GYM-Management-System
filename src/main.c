#include <gtk/gtk.h>
#include <stdio.h>
#include "login.h"
#include "database.h"

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Initialize Database
    if (db_init() != 0) {
        fprintf(stderr, "Failed to initialize database.\n");
        return 1;
    }

    // Show Login Window
    show_login_window();

    // Start Main Loop
    gtk_main();

    return 0;
}

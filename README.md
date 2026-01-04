# 🏋️ GYM Management System

A simple, clean, and minimalistic Gym Management System built with C, GTK+ 3, and SQLite.

## ✨ Features

- **Member Dashboard** - Select plans, time slots, and trainers
- **Admin Panel** - Manage members and trainers
- **Trainer Registration** - Apply and get approved by admin
- **Secure Authentication** - Login with email verification

## 📋 Prerequisites

Before you begin, make sure you have these installed:

### Windows (MSYS2/MinGW)
```bash
# Install GTK+ 3 and SQLite3
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-gcc make
```

### Linux (Ubuntu/Debian)
```bash
# Install GTK+ 3 and SQLite3
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev libsqlite3-dev
```

### macOS
```bash
# Install GTK+ 3 and SQLite3
brew install gtk+3 sqlite3 pkg-config
```

## 🚀 How to Run (Baby Steps)

### Step 1: Open Terminal
- **Windows**: Open MSYS2 MinGW terminal
- **Linux/Mac**: Open your terminal application

### Step 2: Navigate to Project Folder
```bash
cd "e:/GYM Management System"
```

### Step 3: Build the Project
```bash
make
```
This will compile all the source files and create the executable.

### Step 4: Run the Application
```bash
make run
```
Or directly:
```bash
./bin/gym_system
```

### Step 5: Login
Use these default credentials:
- **Email**: `admin@gym.com`
- **Password**: `admin123`

## 🔑 Default Credentials

| Role | Email | Password | Verification Code |
|------|-------|----------|-------------------|
| Admin | admin@gym.com | admin123 | N/A (pre-verified) |
| New Users | (your email) | (your password) | 1234 |

## 📁 Project Structure

```
GYM Management System/
├── src/              # Source code files
│   ├── main.c        # Application entry point
│   ├── login.c       # Login and registration
│   ├── member.c      # Member dashboard
│   ├── admin.c       # Admin panel
│   └── database.c    # Database operations
├── include/          # Header files
│   ├── login.h
│   ├── member.h
│   ├── admin.h
│   ├── database.h
│   └── models.h      # Data structures
├── bin/              # Compiled executable (created on build)
├── obj/              # Object files (created on build)
├── database/         # SQLite database (created on first run)
├── Makefile          # Build configuration
└── README.md         # This file
```

## 🎯 Available Plans

1. **Basic (Morning)** - $30/month - Morning sessions (6-10 AM)
2. **Standard (Evening)** - $50/month - Evening sessions (5-9 PM)
3. **Premium (Anytime)** - $80/month - Full day access

## 🛠️ Makefile Commands

```bash
make          # Build the application
make run      # Build and run the application
make clean    # Remove build artifacts
make help     # Show available commands
```

## 🐛 Troubleshooting

### "Command not found: make"
Install build tools for your platform (see Prerequisites above).

### "Package gtk+-3.0 was not found"
Install GTK+ 3 development libraries (see Prerequisites above).

### "Can't open database"
The database folder will be created automatically on first run. Make sure you have write permissions in the project directory.

### Application doesn't start
1. Make sure all prerequisites are installed
2. Try running `make clean` then `make` again
3. Check that you're in the correct directory

## 📝 How to Use

### For Members:
1. Register with your email
2. Verify with code `1234`
3. Login with your credentials
4. Select a membership plan
5. Choose your preferred time slot
6. Pick a trainer
7. View your dashboard

### For Trainers:
1. Register and select "Trainer" role
2. Verify with code `1234`
3. Wait for admin approval
4. Login after approval

### For Admin:
1. Login with admin credentials
2. Approve/reject trainer applications
3. Manage members and trainers
4. View all system data

## 💡 Tips

- The verification code is always `1234` (simulated email verification)
- Admin account is pre-created for testing
- Database is automatically initialized on first run
- All data is stored in `database/gym.db`

## 🎨 Design Philosophy

This project follows a **minimalistic and clean** design approach:
- Simple, intuitive user interface
- Clear code organization with comments
- Easy-to-understand structure
- Straightforward workflows

---

**Enjoy your gym management! 💪**

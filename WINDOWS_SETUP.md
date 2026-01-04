# 🏋️ GYM Management System - Windows Setup Complete! ✅

## ✨ Setup Status

All dependencies have been installed:
- ✅ MSYS2 (Unix environment for Windows)
- ✅ GCC Compiler
- ✅ Make Build Tool
- ✅ GTK+ 3 (GUI Library)
- ✅ SQLite3 (Database)
- ✅ pkg-config

## 🚀 How to Run

### Option 1: Double-click the batch file (EASIEST)
Simply double-click `run.bat` in this folder!

### Option 2: From PowerShell
```powershell
C:\msys64\msys2_shell.cmd -mingw64 -defterm -no-start -c "cd 'E:/GYM Management System' && make && ./bin/gym_system"
```

### Option 3: Open MSYS2 MinGW 64-bit terminal
1. Search for "MSYS2 MinGW 64-bit" in Start Menu
2. Run these commands:
```bash
cd "E:/GYM Management System"
make
./bin/gym_system
```

## 🔑 Default Login Credentials

**Admin Account:**
- Email: `admin@gym.com`
- Password: `admin123`

**New User Registration:**
- Verification Code: `1234` (always)

## 📁 Project Files

- `run.bat` - Quick launcher for Windows
- `bin/gym_system.exe` - The compiled application
- `database/gym.db` - Database file (created on first run)

## 🛠️ Build Commands

If you want to rebuild manually:
```bash
# Clean build artifacts
make clean

# Build the project
make

# Build and run
make run
```

## ⚠️ Important Notes

1. **MSYS2 is required** - Don't delete `C:\msys64` folder
2. **Database is auto-created** - No manual setup needed
3. **Windows Firewall** - May ask for permission on first run
4. **GTK Theme** - Uses system theme or defaults to a basic theme

## 🎯 Features Available

- Member Dashboard
- Admin Panel
- Trainer Management
- Plan Selection (Basic, Standard, Premium)
- Time Slot Booking
- Database Management

## 💡 Tips

- The application GUI will open in a separate window
- If it doesn't show up, check your taskbar
- Database is stored in `database/gym.db`
- All changes are saved automatically

---

**Ready to manage your gym! 💪**

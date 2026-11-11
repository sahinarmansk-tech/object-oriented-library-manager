# Object-Oriented Library Management System

A console-based Library Management System developed in C++ using Object-Oriented Programming (OOP), file handling, data validation, and persistent storage.

## Project Overview

This application helps manage a library's books, registered members, and borrowing records. It allows users to add books, register members, issue and return books, search records, and check book availability.

All successful changes are saved to a local data file and loaded automatically when the application starts.

## Features

### Book Management
- Add new books to the library.
- Search books by ID, title, author, or category.
- Display all registered books.
- Check book availability and total copies.

### Member Management
- Register new library members.
- View all registered members.
- Search for a member using their Member ID.
- Display member details, including name and email.
- View the number of active loans for each member.
- View active borrowed-book details when searching for a member.

### Loan Management
- Issue books to registered members.
- Return borrowed books.
- Generate unique loan IDs.
- Track issue dates and due dates.
- Prevent issuing unavailable book copies.
- Display all active loans.

### Data Persistence
- Automatically save successful changes.
- Load saved records when the application starts.
- Validate records before accepting saved data.
- Use temporary files and backup handling during data saving.

## Menu Options

| Option | Function |
|---|---|
| 1 | Add a book |
| 2 | Register a member |
| 3 | Issue a book |
| 4 | Return a book |
| 5 | View all members | 
| 6 | Search member by ID | 
| 7 | Search books |
| 8 | Check book availability |
| 9 | List all books |
| 10 | List active loans |
| 11 | Save and exit |

## Technologies Used

- **Programming Language:** C++
- **Concepts:** Classes, objects, encapsulation, collections, and file handling
- **Storage:** Local text file
- **Development Environment:** OnlineGDB or a C++ compiler supporting C++17

## Project Structure

The main project is organised into separate header and source files:

```text
object-oriented-library-manager/
│
├── include/
│   ├── Book.h
│   ├── Member.h
│   ├── Loan.h
│   └── LibraryManager.h
│
├── src/
│   ├── Book.cpp
│   ├── Member.cpp
│   ├── Loan.cpp
│   ├── LibraryManager.cpp
│   └── main.cpp
│
├── data/
│   └── library_data.txt
│
├── sample-data/
│   ├── library-records-sample.csv
│   └── sample-session.txt
│
├── docs/
│   ├── class-diagram.png
│   ├── class-diagram.svg
│   ├── class-diagram.dot
│   └── class-diagram.mmd
│
├── Makefile
├── .gitignore
└── README.md
```

## Class Design

The application uses four main classes:

- **Book:** Stores book information, including ID, title, author, category, publication year, and number of copies.
- **Member:** Stores registered member information, including ID, name, and email.
- **Loan:** Maintains borrowing records, including book ID, member ID, issue date, due date, and return status.
- **LibraryManager:** Coordinates books, members, loans, application operations, and data persistence.

See the `docs/` folder for the class diagram.

## How to Run

### Using OnlineGDB

1. Open [OnlineGDB](https://www.onlinegdb.com/online_c++_compiler).
2. Upload the C++ source and header files.
3. Upload `library_data.txt` into the project root.
4. Make sure the database filename in `main.cpp` matches the uploaded file location.
5. Select C++17 if the compiler settings allow it.
6. Click **Run**.

For the flattened OnlineGDB project, the data file is expected at the root level.

### Using a Local C++ Compiler

If the project folders are preserved, compile using a C++17-compatible compiler. For example:

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o library_manager
```

Run the application:

```bash
./library_manager
```

On Windows, run:

```bash
library_manager.exe
```

## Sample Data

The `sample-data/` folder contains the supplied CSV sample and a sample application session.

The live application records are stored in `data/library_data.txt` in the main project layout.

The sample dataset includes six book records and two borrowed statuses. The original CSV did not provide member names, email addresses, loan IDs, or issue dates. Therefore, the seed file uses illustrative demo member details and an illustrative issue date.

Replace demo details with appropriate test data before using the application for real library records.

**Important:** Keep a backup of `library_data.txt` before experimenting with important data.

## Testing

The following application features were tested:

- Listing all books.
- Searching for a book by ID.
- Checking book availability.
- Registering a member.
- Issuing an available book.
- Updating availability after issuing.
- Returning a borrowed book.
- Updating availability after returning.
- Saving and reloading records after restarting the application.

The new member-management features should also be tested:

- Viewing all registered members.
- Searching for an existing member by ID.
- Searching for a non-existent member ID.
- Checking active loan counts and borrowed-book details.

## Learning Outcomes

This project demonstrates practical use of:

- Object-Oriented Programming in C++.
- Classes and encapsulation.
- Header and implementation file separation.
- Vectors and collections.
- Searching and record management.
- File input and output.
- Input validation and error handling.
- Data persistence.

## Author

Name: Sk Sahin Arman
Developed as a C++ internship project.

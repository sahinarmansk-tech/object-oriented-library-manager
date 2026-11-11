#pragma once

#include "Book.h"
#include "Loan.h"
#include "Member.h"

#include <string>
#include <vector>

class LibraryManager {
public:
    explicit LibraryManager(std::string dataFile);

    // Returns false if an existing data file is malformed or cannot be read.
    bool loadData();
    void run();

private:
    std::string dataFile_;
    std::vector<Book> books_;
    std::vector<Member> members_;
    std::vector<Loan> loans_;

    void printMenu() const;
    void addBook();
    void registerMember();
    void issueBook();
    void returnBook();
    void searchBooks() const;
    void showAvailability() const;
    void listBooks() const;
    void listActiveLoans() const;
    void listMembers() const;
    void searchMember() const;

    bool saveData() const;
    std::string makeId(char prefix, const std::vector<std::string>& existingIds) const;
    int activeLoanCount(const std::string& bookId) const;
    const Book* findBook(const std::string& id) const;
    const Member* findMember(const std::string& id) const;
    Loan* findActiveLoan(const std::string& loanId);
};

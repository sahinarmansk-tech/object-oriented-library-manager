#include "LibraryManager.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>

namespace {
const char* const DATA_HEADER = "LIBRARY_MANAGER_DATA_V1";

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool readLine(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    if (!std::getline(std::cin, value)) return false;
    value = trim(value);
    return true;
}

bool readInteger(const std::string& prompt, int& value, int minimum, int maximum) {
    while (true) {
        std::string line;
        if (!readLine(prompt, line)) return false;
        std::istringstream input(line);
        int parsed = 0;
        char extra = '\0';
        if ((input >> parsed) && !(input >> extra) && parsed >= minimum && parsed <= maximum) {
            value = parsed;
            return true;
        }
        std::cout << "Invalid input. Enter a whole number from " << minimum
                  << " to " << maximum << ".\n";
    }
}

bool validEmail(const std::string& email) {
    const auto at = email.find('@');
    return at != std::string::npos && at > 0 &&
           email.find('.', at) != std::string::npos &&
           email.find(' ') == std::string::npos;
}

bool isLeapYear(int year) {
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

bool validDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));
    if (year < 1 || month < 1 || month > 12) return false;
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maximumDay = daysInMonth[month - 1];
    if (month == 2 && isLeapYear(year)) maximumDay = 29;
    return day >= 1 && day <= maximumDay;
}

bool dateNotBefore(const std::string& first, const std::string& second) {
    // Valid ISO dates sort lexicographically in chronological order.
    return first <= second;
}

bool noExtraTokens(std::istringstream& stream) {
    std::string extra;
    return !(stream >> extra);
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

void printBookRow(const Book& book, int checkedOut) {
    std::cout << std::left << std::setw(7) << book.getId()
              << std::setw(34) << book.getTitle().substr(0, 32)
              << std::setw(27) << book.getAuthor().substr(0, 25)
              << std::setw(18) << book.getCategory().substr(0, 16)
              << std::right << std::setw(7) << book.getPublicationYear()
              << std::setw(9) << book.getTotalCopies() - checkedOut << "/"
              << book.getTotalCopies() << '\n';
}
} // namespace

LibraryManager::LibraryManager(std::string dataFile) : dataFile_(std::move(dataFile)) {}

bool LibraryManager::loadData() {
    std::ifstream input(dataFile_);
    if (!input) {
        if (!std::filesystem::exists(dataFile_)) {
            std::cout << "No saved library file found. Starting with an empty library.\n";
            return true;
        }
        std::cerr << "Error: could not open " << dataFile_ << " for reading.\n";
        return false;
    }

    std::string header;
    if (!std::getline(input, header) || header != DATA_HEADER) {
        std::cerr << "Error: unsupported or damaged library data file header.\n";
        return false;
    }

    std::vector<Book> loadedBooks;
    std::vector<Member> loadedMembers;
    std::vector<Loan> loadedLoans;
    std::string line;
    int lineNumber = 1;

    while (std::getline(input, line)) {
        ++lineNumber;
        if (trim(line).empty() || trim(line).front() == '#') continue;
        std::istringstream record(line);
        std::string type;
        record >> type;

        if (type == "BOOK") {
            std::string id, title, author, category;
            int year = 0, copies = 0;
            if (!(record >> std::quoted(id) >> std::quoted(title) >> std::quoted(author)
                         >> std::quoted(category) >> year >> copies) || !noExtraTokens(record) ||
                id.empty() || title.empty() || author.empty() || category.empty() ||
                year < 0 || year > 2100 || copies < 1) {
                std::cerr << "Error: invalid BOOK record on line " << lineNumber << ".\n";
                return false;
            }
            loadedBooks.emplace_back(id, title, author, category, year, copies);
        } else if (type == "MEMBER") {
            std::string id, name, email;
            if (!(record >> std::quoted(id) >> std::quoted(name) >> std::quoted(email)) ||
                !noExtraTokens(record) || id.empty() || name.empty() || !validEmail(email)) {
                std::cerr << "Error: invalid MEMBER record on line " << lineNumber << ".\n";
                return false;
            }
            loadedMembers.emplace_back(id, name, email);
        } else if (type == "LOAN") {
            std::string id, bookId, memberId, issueDate, dueDate, returnDate;
            int returned = -1;
            if (!(record >> std::quoted(id) >> std::quoted(bookId) >> std::quoted(memberId)
                         >> std::quoted(issueDate) >> std::quoted(dueDate) >> returned
                         >> std::quoted(returnDate)) || !noExtraTokens(record) ||
                id.empty() || bookId.empty() || memberId.empty() ||
                !validDate(issueDate) || !validDate(dueDate) ||
                !dateNotBefore(issueDate, dueDate) || (returned != 0 && returned != 1) ||
                (returned == 1 && (!validDate(returnDate) || !dateNotBefore(issueDate, returnDate))) ||
                (returned == 0 && !returnDate.empty())) {
                std::cerr << "Error: invalid LOAN record on line " << lineNumber << ".\n";
                return false;
            }
            loadedLoans.emplace_back(id, bookId, memberId, issueDate, dueDate,
                                     returned == 1, returnDate);
        } else {
            std::cerr << "Error: unknown record type on line " << lineNumber << ".\n";
            return false;
        }
    }

    std::set<std::string> bookIds, memberIds, loanIds;
    for (const auto& book : loadedBooks) {
        if (!bookIds.insert(book.getId()).second) {
            std::cerr << "Error: duplicate book ID " << book.getId() << ".\n";
            return false;
        }
    }
    for (const auto& member : loadedMembers) {
        if (!memberIds.insert(member.getId()).second) {
            std::cerr << "Error: duplicate member ID " << member.getId() << ".\n";
            return false;
        }
    }
    std::vector<int> activeByBook(loadedBooks.size(), 0);
    std::set<std::pair<std::string, std::string>> activePairs;
    for (const auto& loan : loadedLoans) {
        if (!loanIds.insert(loan.getId()).second || !bookIds.count(loan.getBookId()) ||
            !memberIds.count(loan.getMemberId())) {
            std::cerr << "Error: duplicate loan ID or missing book/member reference.\n";
            return false;
        }
        if (!loan.isReturned()) {
            if (!activePairs.insert({loan.getBookId(), loan.getMemberId()}).second) {
                std::cerr << "Error: duplicate active loan for the same book and member.\n";
                return false;
            }
            for (std::size_t i = 0; i < loadedBooks.size(); ++i) {
                if (loadedBooks[i].getId() == loan.getBookId()) ++activeByBook[i];
            }
        }
    }
    for (std::size_t i = 0; i < loadedBooks.size(); ++i) {
        if (activeByBook[i] > loadedBooks[i].getTotalCopies()) {
            std::cerr << "Error: active loans exceed copies for book " << loadedBooks[i].getId() << ".\n";
            return false;
        }
    }

    books_ = std::move(loadedBooks);
    members_ = std::move(loadedMembers);
    loans_ = std::move(loadedLoans);
    std::cout << "Loaded " << books_.size() << " book(s), " << members_.size()
              << " member(s), and " << loans_.size() << " loan record(s).\n";
    return true;
}

void LibraryManager::run() {
    bool keepRunning = true;
    while (keepRunning) {
        printMenu();
        int choice = 0;

        if (!readInteger("Select an option (1-11): ", choice, 1, 11)) {
            std::cout << "\nInput closed. Saving library data...\n";
            break;
        }

        switch (choice) {
            case 1: addBook(); break;
            case 2: registerMember(); break;
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: listMembers(); break;
            case 6: searchMember(); break;
            case 7: searchBooks(); break;
            case 8: showAvailability(); break;
            case 9: listBooks(); break;
            case 10: listActiveLoans(); break;
            case 11: keepRunning = false; break;
            default: break;
        }

        if (!keepRunning) break;
        std::cout << '\n';
    }

    if (saveData()) std::cout << "Library data saved. Goodbye!\n";
    else std::cerr << "Warning: could not save library data. Check file permissions and disk space.\n";
}

void LibraryManager::printMenu() const {
    std::cout << "\n========================================\n"
              << "        OBJECT-ORIENTED LIBRARY         \n"
              << "========================================\n"
              << "1. Add a book\n"
              << "2. Register a member\n"
              << "3. Issue a book\n"
              << "4. Return a book\n"
              << "5. View all members\n"
              << "6. Search member by ID\n"
              << "7. Search books\n"
              << "8. Check book availability\n"
              << "9. List all books\n"
              << "10. List active loans\n"
              << "11. Save and exit\n";
}

void LibraryManager::addBook() {
    std::string title, author, category;
    int year = 0, copies = 0;
    if (!readLine("Book title: ", title) || !readLine("Author: ", author) ||
        !readLine("Category: ", category)) return;
    if (title.empty() || author.empty() || category.empty()) {
        std::cout << "Title, author, and category cannot be blank. Book not added.\n";
        return;
    }
    if (!readInteger("Publication year (0-2100): ", year, 0, 2100) ||
        !readInteger("Number of copies (1-999): ", copies, 1, 999)) return;
    std::vector<std::string> ids;
    for (const auto& book : books_) ids.push_back(book.getId());
    const std::string id = makeId('B', ids);
    books_.emplace_back(id, title, author, category, year, copies);
    if (!saveData()) {
        books_.pop_back();
        std::cout << "Could not save changes; the book was not added.\n";
        return;
    }
    std::cout << "Book added successfully. Book ID: " << id << '\n';
}

void LibraryManager::registerMember() {
    std::string name, email;
    if (!readLine("Member name: ", name) || !readLine("Email address: ", email)) return;
    if (name.empty() || !validEmail(email)) {
        std::cout << "Please enter a name and a valid-looking email address. Member not added.\n";
        return;
    }
    const std::string normalizedEmail = lower(email);
    for (const auto& member : members_) {
        if (lower(member.getEmail()) == normalizedEmail) {
            std::cout << "That email is already registered. Member not added.\n";
            return;
        }
    }
    std::vector<std::string> ids;
    for (const auto& member : members_) ids.push_back(member.getId());
    const std::string id = makeId('M', ids);
    members_.emplace_back(id, name, email);
    if (!saveData()) {
        members_.pop_back();
        std::cout << "Could not save changes; the member was not registered.\n";
        return;
    }
    std::cout << "Member registered successfully. Member ID: " << id << '\n';
}

void LibraryManager::issueBook() {
    std::string bookId, memberId, issueDate, dueDate;
    if (!readLine("Book ID: ", bookId) || !readLine("Member ID: ", memberId)) return;
    const Book* book = findBook(bookId);
    const Member* member = findMember(memberId);
    if (!book) { std::cout << "Book ID not found.\n"; return; }
    if (!member) { std::cout << "Member ID not found. Register the member first.\n"; return; }
    if (activeLoanCount(bookId) >= book->getTotalCopies()) {
        std::cout << "No copies are currently available.\n";
        return;
    }
    for (const auto& loan : loans_) {
        if (!loan.isReturned() && loan.getBookId() == bookId && loan.getMemberId() == memberId) {
            std::cout << "This member already has an active loan for this book.\n";
            return;
        }
    }
    if (!readLine("Issue date (YYYY-MM-DD): ", issueDate) ||
        !readLine("Due date (YYYY-MM-DD): ", dueDate)) return;
    if (!validDate(issueDate) || !validDate(dueDate) || !dateNotBefore(issueDate, dueDate)) {
        std::cout << "Invalid dates. Use real YYYY-MM-DD dates; due date cannot precede issue date.\n";
        return;
    }
    std::vector<std::string> ids;
    for (const auto& loan : loans_) ids.push_back(loan.getId());
    const std::string loanId = makeId('L', ids);
    loans_.emplace_back(loanId, bookId, memberId, issueDate, dueDate);
    if (!saveData()) {
        loans_.pop_back();
        std::cout << "Could not save changes; the book was not issued.\n";
        return;
    }
    std::cout << "Book issued successfully. Loan ID: " << loanId << '\n'
              << "Book: " << book->getTitle() << " | Member: " << member->getName() << '\n';
}

void LibraryManager::returnBook() {
    std::string loanId, returnDate;
    if (!readLine("Loan ID: ", loanId)) return;
    Loan* loan = findActiveLoan(loanId);
    if (!loan) {
        std::cout << "No active loan found with that ID.\n";
        return;
    }
    if (!readLine("Return date (YYYY-MM-DD): ", returnDate)) return;
    if (!validDate(returnDate) || !dateNotBefore(loan->getIssueDate(), returnDate)) {
        std::cout << "Invalid return date. It must be a real date on or after the issue date.\n";
        return;
    }
    const Loan previous = *loan;
    loan->markReturned(returnDate);
    if (!saveData()) {
        *loan = previous;
        std::cout << "Could not save changes; the return was not recorded.\n";
        return;
    }
    std::cout << "Book return recorded successfully.\n";
}

void LibraryManager::listMembers() const {
    if (members_.empty()) {
        std::cout << "No registered members found.\n";
        return;
    }

    std::cout << "\nREGISTERED MEMBERS\n";
    std::cout << std::left
              << std::setw(10) << "ID"
              << std::setw(25) << "NAME"
              << std::setw(32) << "EMAIL"
              << "ACTIVE LOANS\n";

    for (const auto& member : members_) {
        int borrowed = static_cast<int>(
            std::count_if(loans_.begin(), loans_.end(),
                [&](const Loan& loan) {
                    return !loan.isReturned() &&
                           loan.getMemberId() == member.getId();
                })
        );

        std::cout << std::left
                  << std::setw(10) << member.getId()
                  << std::setw(25) << member.getName().substr(0, 23)
                  << std::setw(32) << member.getEmail().substr(0, 30)
                  << borrowed << '\n';
    }

    std::cout << members_.size() << " registered member(s).\n";
}

void LibraryManager::searchMember() const {
    std::string id;

    if (!readLine("Enter Member ID: ", id)) return;

    const Member* member = findMember(id);

    if (!member) {
        std::cout << "Member ID not found.\n";
        return;
    }

    std::cout << "\nMEMBER FOUND!\n"
              << "Member ID: " << member->getId() << '\n'
              << "Name: " << member->getName() << '\n'
              << "Email: " << member->getEmail() << '\n';

    int activeCount = 0;

    std::cout << "\nACTIVE BORROWED BOOKS\n";

    for (const auto& loan : loans_) {
        if (loan.isReturned() || loan.getMemberId() != id) {
            continue;
        }

        const Book* book = findBook(loan.getBookId());

        std::cout << "Loan ID: " << loan.getId()
                  << " | Book: "
                  << (book ? book->getTitle() : "[missing book]")
                  << " | Due date: " << loan.getDueDate() << '\n';

        ++activeCount;
    }

    if (activeCount == 0) {
        std::cout << "This member has no active borrowed books.\n";
    } else {
        std::cout << "Total active loans: " << activeCount << '\n';
    }
}

void LibraryManager::searchBooks() const {
    std::string query;
    if (!readLine("Search by ID, title, author, or category: ", query)) return;
    if (query.empty()) { std::cout << "Search text cannot be blank.\n"; return; }
    int matches = 0;
    std::cout << "\n" << std::left << std::setw(7) << "ID" << std::setw(34) << "TITLE"
              << std::setw(27) << "AUTHOR" << std::setw(18) << "CATEGORY"
              << std::right << std::setw(7) << "YEAR" << std::setw(9) << "AVAILABLE\n";
    for (const auto& book : books_) {
        if (book.matches(query)) {
            printBookRow(book, activeLoanCount(book.getId()));
            ++matches;
        }
    }
    if (matches == 0) std::cout << "No matching books found.\n";
    else std::cout << matches << " matching book record(s).\n";
}

void LibraryManager::showAvailability() const {
    std::string id;
    if (!readLine("Book ID: ", id)) return;
    const Book* book = findBook(id);
    if (!book) { std::cout << "Book ID not found.\n"; return; }
    const int borrowed = activeLoanCount(id);
    std::cout << book->getTitle() << " — " << book->getAuthor() << '\n'
              << "Total copies: " << book->getTotalCopies() << '\n'
              << "On loan: " << borrowed << '\n'
              << "Available now: " << book->getTotalCopies() - borrowed << '\n';
}

void LibraryManager::listBooks() const {
    if (books_.empty()) { std::cout << "The catalogue is empty.\n"; return; }
    std::cout << "\n" << std::left << std::setw(7) << "ID" << std::setw(34) << "TITLE"
              << std::setw(27) << "AUTHOR" << std::setw(18) << "CATEGORY"
              << std::right << std::setw(7) << "YEAR" << std::setw(9) << "AVAILABLE\n";
    for (const auto& book : books_) printBookRow(book, activeLoanCount(book.getId()));
}

void LibraryManager::listActiveLoans() const {
    int count = 0;
    std::cout << "\nACTIVE LOANS\n";
    for (const auto& loan : loans_) {
        if (loan.isReturned()) continue;
        const Book* book = findBook(loan.getBookId());
        const Member* member = findMember(loan.getMemberId());
        std::cout << "Loan: " << loan.getId()
                  << " | Book: " << (book ? book->getTitle() : "[missing book]")
                  << " | Member: " << (member ? member->getName() : "[missing member]")
                  << " | Issued: " << loan.getIssueDate()
                  << " | Due: " << loan.getDueDate() << '\n';
        ++count;
    }
    if (count == 0) std::cout << "There are no active loans.\n";
    else std::cout << count << " active loan(s).\n";
}

bool LibraryManager::saveData() const {
    namespace fs = std::filesystem;
    const fs::path target(dataFile_);
    std::error_code error;
    if (!target.parent_path().empty()) fs::create_directories(target.parent_path(), error);
    if (error) return false;

    const fs::path temporary = target.string() + ".tmp";
    const fs::path backup = target.string() + ".bak";
    {
        std::ofstream output(temporary, std::ios::trunc);
        if (!output) return false;
        output << DATA_HEADER << '\n';
        for (const auto& book : books_) {
            output << "BOOK " << std::quoted(book.getId()) << ' ' << std::quoted(book.getTitle())
                   << ' ' << std::quoted(book.getAuthor()) << ' ' << std::quoted(book.getCategory())
                   << ' ' << book.getPublicationYear() << ' ' << book.getTotalCopies() << '\n';
        }
        for (const auto& member : members_) {
            output << "MEMBER " << std::quoted(member.getId()) << ' '
                   << std::quoted(member.getName()) << ' ' << std::quoted(member.getEmail()) << '\n';
        }
        for (const auto& loan : loans_) {
            output << "LOAN " << std::quoted(loan.getId()) << ' ' << std::quoted(loan.getBookId())
                   << ' ' << std::quoted(loan.getMemberId()) << ' ' << std::quoted(loan.getIssueDate())
                   << ' ' << std::quoted(loan.getDueDate()) << ' ' << (loan.isReturned() ? 1 : 0)
                   << ' ' << std::quoted(loan.getReturnDate()) << '\n';
        }
        output.flush();
        if (!output) {
            output.close();
            fs::remove(temporary, error);
            return false;
        }
    }

    error.clear();
    const bool targetExists = fs::exists(target, error);
    if (error) { fs::remove(temporary, error); return false; }
    bool movedOriginal = false;
    if (targetExists) {
        fs::remove(backup, error);
        error.clear();
        fs::rename(target, backup, error);
        if (error) { fs::remove(temporary, error); return false; }
        movedOriginal = true;
    }
    error.clear();
    fs::rename(temporary, target, error);
    if (error) {
        if (movedOriginal) {
            std::error_code restoreError;
            fs::rename(backup, target, restoreError);
        }
        fs::remove(temporary, error);
        return false;
    }
    if (movedOriginal) fs::remove(backup, error);
    return true;
}

std::string LibraryManager::makeId(char prefix, const std::vector<std::string>& existingIds) const {
    std::set<std::string> existing(existingIds.begin(), existingIds.end());
    for (int number = 1; number < 1000000; ++number) {
        std::ostringstream candidate;
        candidate << prefix << std::setw(3) << std::setfill('0') << number;
        if (!existing.count(candidate.str())) return candidate.str();
    }
    return std::string(1, prefix) + "_ID_LIMIT_REACHED";
}

int LibraryManager::activeLoanCount(const std::string& bookId) const {
    return static_cast<int>(std::count_if(loans_.begin(), loans_.end(), [&](const Loan& loan) {
        return !loan.isReturned() && loan.getBookId() == bookId;
    }));
}

const Book* LibraryManager::findBook(const std::string& id) const {
    const auto found = std::find_if(books_.begin(), books_.end(), [&](const Book& book) {
        return book.getId() == id;
    });
    return found == books_.end() ? nullptr : &*found;
}

const Member* LibraryManager::findMember(const std::string& id) const {
    const auto found = std::find_if(members_.begin(), members_.end(), [&](const Member& member) {
        return member.getId() == id;
    });
    return found == members_.end() ? nullptr : &*found;
}

Loan* LibraryManager::findActiveLoan(const std::string& loanId) {
    const auto found = std::find_if(loans_.begin(), loans_.end(), [&](const Loan& loan) {
        return loan.getId() == loanId && !loan.isReturned();
    });
    return found == loans_.end() ? nullptr : &*found;
}

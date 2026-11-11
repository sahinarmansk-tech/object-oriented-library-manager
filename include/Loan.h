#pragma once

#include <string>

class Loan {
public:
    Loan() = default;
    Loan(std::string id, std::string bookId, std::string memberId,
         std::string issueDate, std::string dueDate,
         bool returned = false, std::string returnDate = {});

    const std::string& getId() const;
    const std::string& getBookId() const;
    const std::string& getMemberId() const;
    const std::string& getIssueDate() const;
    const std::string& getDueDate() const;
    const std::string& getReturnDate() const;
    bool isReturned() const;

    void markReturned(const std::string& returnDate);

private:
    std::string id_;
    std::string bookId_;
    std::string memberId_;
    std::string issueDate_;
    std::string dueDate_;
    bool returned_ = false;
    std::string returnDate_;
};

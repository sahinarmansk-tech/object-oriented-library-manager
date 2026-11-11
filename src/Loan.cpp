#include "Loan.h"

#include <utility>

Loan::Loan(std::string id, std::string bookId, std::string memberId,
           std::string issueDate, std::string dueDate,
           bool returned, std::string returnDate)
    : id_(std::move(id)), bookId_(std::move(bookId)), memberId_(std::move(memberId)),
      issueDate_(std::move(issueDate)), dueDate_(std::move(dueDate)),
      returned_(returned), returnDate_(std::move(returnDate)) {}

const std::string& Loan::getId() const { return id_; }
const std::string& Loan::getBookId() const { return bookId_; }
const std::string& Loan::getMemberId() const { return memberId_; }
const std::string& Loan::getIssueDate() const { return issueDate_; }
const std::string& Loan::getDueDate() const { return dueDate_; }
const std::string& Loan::getReturnDate() const { return returnDate_; }
bool Loan::isReturned() const { return returned_; }

void Loan::markReturned(const std::string& returnDate) {
    returned_ = true;
    returnDate_ = returnDate;
}

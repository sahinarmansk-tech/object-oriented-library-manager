#include "Book.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace {
std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}
} // namespace

Book::Book(std::string id, std::string title, std::string author,
           std::string category, int publicationYear, int totalCopies)
    : id_(std::move(id)), title_(std::move(title)), author_(std::move(author)),
      category_(std::move(category)), publicationYear_(publicationYear),
      totalCopies_(totalCopies) {}

const std::string& Book::getId() const { return id_; }
const std::string& Book::getTitle() const { return title_; }
const std::string& Book::getAuthor() const { return author_; }
const std::string& Book::getCategory() const { return category_; }
int Book::getPublicationYear() const { return publicationYear_; }
int Book::getTotalCopies() const { return totalCopies_; }

bool Book::matches(const std::string& query) const {
    const std::string needle = toLower(query);
    return toLower(id_).find(needle) != std::string::npos ||
           toLower(title_).find(needle) != std::string::npos ||
           toLower(author_).find(needle) != std::string::npos ||
           toLower(category_).find(needle) != std::string::npos;
}

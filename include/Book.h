#pragma once

#include <string>

class Book {
public:
    Book() = default;
    Book(std::string id, std::string title, std::string author,
         std::string category, int publicationYear, int totalCopies);

    const std::string& getId() const;
    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getCategory() const;
    int getPublicationYear() const;
    int getTotalCopies() const;

    bool matches(const std::string& query) const;

private:
    std::string id_;
    std::string title_;
    std::string author_;
    std::string category_;
    int publicationYear_ = 0;
    int totalCopies_ = 1;
};

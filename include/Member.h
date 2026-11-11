#pragma once

#include <string>

class Member {
public:
    Member() = default;
    Member(std::string id, std::string name, std::string email);

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getEmail() const;

private:
    std::string id_;
    std::string name_;
    std::string email_;
};

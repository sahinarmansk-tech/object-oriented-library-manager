#include "Member.h"

#include <utility>

Member::Member(std::string id, std::string name, std::string email)
    : id_(std::move(id)), name_(std::move(name)), email_(std::move(email)) {}

const std::string& Member::getId() const { return id_; }
const std::string& Member::getName() const { return name_; }
const std::string& Member::getEmail() const { return email_; }

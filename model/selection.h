#pragma once

#include <unordered_set>
#include <string>
#include <QString>

class Selection {
    std::unordered_set<std::string> data_{};
public:
    static Selection& self() noexcept {
        static auto obj = Selection();
        return obj;
    }
    Selection(Selection const&) = delete;
    Selection& operator=(Selection const&) = delete;
    Selection(Selection&&) = delete;
    Selection& operator=(Selection&&) = delete;
    ~Selection() = default;

    void insert(QString const& path) noexcept {
        data_.insert(path.toStdString());
    }
    void erase(QString const& path) noexcept {
        data_.erase(path.toStdString());
    }
    void clear() noexcept {
        data_.clear();
    }

private:
    Selection() = default;
};

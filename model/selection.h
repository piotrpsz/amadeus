#pragma once

#include "../shared/event.hh"
#include "../shared/event_controller.hh"
#include <set>
#include <string>
#include <QString>

class Selection : QObject {
    Q_OBJECT
    std::set<std::string> data_{};
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
        EventController::self().send(event::SelectionChanged);
    }
    void erase(QString const& path) noexcept {
        data_.erase(path.toStdString());
        EventController::self().send(event::SelectionChanged);
    }
    bool contains(QString const& path) noexcept {
        return data_.contains(path.toStdString());
    }
    bool empty() const noexcept {
        return data_.empty();
    }
    size_t size() const noexcept {
        return data_.size();
    }
    void clear() noexcept {
        data_.clear();
        EventController::self().send(event::SelectionChanged);
    }

    using iterator = std::set<std::string>::iterator;
    using const_iterator = std::set<std::string>::const_iterator;
    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator cbegin() { return data_.cbegin(); }
    const_iterator cend() { return data_.cend(); }

private:
    Selection() = default;
};
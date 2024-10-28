#pragma once

#include "../shared/event.hh"
#include "../shared/event_controller.hh"
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

private:
    Selection() = default;
};

#pragma once

#include <QString>
#include <pwd.h>
#include <filesystem>
#include <fmt/core.h>
namespace fs = std::filesystem;

class QWidget;

class tool {
public:
    static inline QString const ORGANIZATION{"Piotr Pszczółkowski"};
    static inline QString const DOMAIN{"beesoft.org"};
    static inline QString const VERSION{"0.1.0"};
    static inline QString const PROGRAM{"Amadeus"};
    static inline QString const SUBNAME{"music for you"};


    static void resize(QWidget*, int width, int height);

    static QString app_complete_name() {
        return PROGRAM + " - " + SUBNAME;
    }

    static std::string home_dir() {
        if (struct passwd* pw = getpwuid(getuid()))
            return pw->pw_dir;
        return {};
    }

    static bool create_dirs(std::string const& path) noexcept {
        if (fs::exists(path))
            return true;

        std::error_code ec{};
        if (fs::create_directories(path, ec))
            return true;

        fmt::print(stderr, "{}\n", ec.message());
        return {};
    }
};

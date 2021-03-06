/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemfiles/Logger.hpp"

#include <cassert>
#include <iostream>

using namespace chemfiles;

// Singleton instance
Logger Logger::instance_{};

Logger::Logger() : level_(LogLevel::WARNING), backend_(STDERR), logfile_() {}

void Logger::write_message(LogLevel level, std::string message) {
    if (backend_ == SILENT) {
        return;
    }

    if (backend_ != CALLBACK && backend_ != SILENT) {
        switch (level) {
        case LogLevel::ERROR:
            message = "Chemfiles error: " + message;
            break;
        case LogLevel::WARNING:
            message = "Chemfiles warning: " + message;
            break;
        case LogLevel::INFO:
            message = "Chemfiles info: " + message;
            break;
        case LogLevel::DEBUG:
            message = "Chemfiles debug: " + message;
            break;
        }
    }

    switch (backend_) {
    case SILENT:
        break;
    case STDOUT:
        std::cout << message << std::endl;
        break;
    case STDERR:
        std::cerr << message << std::endl;
        break;
    case FILE:
        assert(logfile_.is_open());
        logfile_ << message << std::endl;
        break;
    case CALLBACK:
        assert(callback_ != nullptr);
        callback_(level, message);
        break;
    }
}

void Logger::set_level(LogLevel level) {
    instance_.level_ = level;
}

void Logger::to_stdout() {
    instance_.close();
    instance_.backend_ = STDOUT;
}

void Logger::to_stderr() {
    instance_.close();
    instance_.backend_ = STDERR;
}

void Logger::silent() {
    instance_.close();
    instance_.backend_ = SILENT;
}

void Logger::to_file(const std::string& filename) {
    instance_.close();
    instance_.backend_ = FILE;
    instance_.logfile_.open(filename, std::ofstream::out);
}

void Logger::callback(logging_cb_t function) {
    instance_.close();
    instance_.backend_ = CALLBACK;
    instance_.callback_ = function;
}

void Logger::close() {
    if (backend_ == FILE) {
        logfile_.close();
    }
}

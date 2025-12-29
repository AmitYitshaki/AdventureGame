#pragma once
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include "Screen.h" 

class GameException : public std::exception
{
private:
    std::string message;
    std::string context;
    std::string fullMessage;

public:
    GameException(const std::string& msg, const std::string& ctx)
        : message(msg), context(ctx)
    {
        fullMessage = "[Error in " + context + "]: " + message;
    }

    const char* what() const noexcept override {
        return fullMessage.c_str();
    }

    // === Static Validators (The Guards) ===

    static void ensureFileOpen(const std::ifstream& file, const std::string& filename, const std::string& ctx) {
        if (!file.is_open()) {
            throw GameException("Failed to open file: " + filename, ctx);
        }
    }

    static void ensureStreamGood(const std::ios& stream, const std::string& errorMsg, const std::string& ctx) {
        if (stream.fail()) {
            throw GameException("Data parsing failed: " + errorMsg, ctx);
        }
    }

    static void ensureNotNull(const void* ptr, const std::string& varName, const std::string& ctx) {
        if (ptr == nullptr) {
            throw GameException("Unexpected Null Pointer: " + varName, ctx);
        }
    }

    static void ensureInBounds(int x, int y, const std::string& ctx) {
        if (x < 0 || x >= Screen::WIDTH || y < 0 || y >= Screen::HEIGHT) {
            throw GameException("Coordinates out of bounds (" + std::to_string(x) + "," + std::to_string(y) + ")", ctx);
        }
    }
};
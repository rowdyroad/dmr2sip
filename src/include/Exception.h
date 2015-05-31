#pragma once
#include <exception>
#include <string>

namespace Commutator {

    class Exception : public std::exception
    {
        private:
            size_t code_;
            std::string message_;
        public:
            Exception(size_t code, const std::string& message) noexcept
                : code_(code)
                , message_(message)
            {}

            template<template<class TT = std::string> class T>
            Exception(size_t code, const T<std::string>& list) noexcept
                : code_(code)
            {
                for (auto& message : list) {
                    message_ += message;
                }
            }

            const char* what() const noexcept
            {
                return message_.c_str();
            }
    };

}
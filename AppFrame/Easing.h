#pragma once
//!
//! @file Easing.h
//!
//! @brief イージングクラス
//!
#include <string>
#include <unordered_map>
#include <functional>

using EasingMode = std::function<double(double, double, double, double)>;
using EasingMap = std::unordered_map<std::string, EasingMode>;

class Easing {
public:
    // イージング関数のラムダを取得する
    static EasingMode GetMode(const std::string& mode) {
        return _modes.at(mode);
    }
private:
    static const EasingMap _modes;
};

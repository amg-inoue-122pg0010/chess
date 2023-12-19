#pragma once
//!
//! @file Easing.h
//!
//! @brief �C�[�W���O�N���X
//!
#include <string>
#include <unordered_map>
#include <functional>

using EasingMode = std::function<double(double, double, double, double)>;
using EasingMap = std::unordered_map<std::string, EasingMode>;

class Easing {
public:
    // �C�[�W���O�֐��̃����_���擾����
    static EasingMode GetMode(const std::string& mode) {
        return _modes.at(mode);
    }
private:
    static const EasingMap _modes;
};

//!
//! @file Easing.cpp
//!
//! @brief イージングクラス
//!
#include "Easing.h"
#include "Math.h"

//!	@brief (4-5.)イージング関数ラムダ式テーブル
//!
//!	@param t 経過時間
//!	@param b 開始値
//!	@param c 終了値
//!	@param d 持続時間
//!	@return  計算結果
//!
const EasingMap Easing::_modes = {
  { "Linear",
    [](double t, double b, double c, double d) {
      return (c - b) * t / d + b;
    }
  },
  { "InQuad",
    [](double t, double b, double c, double d) {
      t /= d;
      return (c - b) * t * t + b;
    }
  },
  { "OutQuad",
    [](double t, double b, double c, double d) {
      t /= d;
      return -(c - b) * t * (t - 2) + b;
    }
  },
  { "InOutQuad",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return (c - b) / 2.0 * t * t + b;
      }
      t--;
      return -(c - b) / 2.0 * (t * (t - 2) - 1) + b;
    }
  },
  { "InCubic",
    [](double t, double b, double c, double d) {
      t /= d;
      return (c - b) * t * t * t + b;
    }
  },
  { "OutCubic",
    [](double t, double b, double c, double d) {
      t /= d;
      t--;
      return (c - b) * (t * t * t + 1) + b;
    }
  },
  { "InOutCubic",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return (c - b) / 2.0 * t * t * t + b;
      }
      t -= 2;
      return (c - b) / 2.0 * (t * t * t + 2) + b;
    }
  },
  { "InQuart",
    [](double t, double b, double c, double d) {
      t /= d;
      return (c - b) * t * t * t * t + b;
    }
  },
  { "OutQuart",
    [](double t, double b, double c, double d) {
      t /= d;
      t--;
      return -(c - b) * (t * t * t * t - 1) + b;
    }
  },
  { "InOutQuart",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return (c - b) / 2.0 * t * t * t * t + b;
      }
      t -= 2;
      return -(c - b) / 2.0 * (t * t * t * t - 2) + b;
    }
  },
  { "InQuint",
    [](double t, double b, double c, double d) {
      t /= d;
      return (c - b) * t * t * t * t * t + b;
    }
  },
  { "OutQuint",
    [](double t, double b, double c, double d) {
      t /= d;
      t--;
      return (c - b) * (t * t * t * t * t + 1) + b;
    }
  },
  { "InOutQuint",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return (c - b) / 2.0 * t * t * t * t * t + b;
      }
      t -= 2;
      return (c - b) / 2.0 * (t * t * t * t * t + 2) + b;
    }
  },
  { "InSine",
    [](double t, double b, double c, double d) {
      return -(c - b) * std::cos(t / d * (Math::PiOver2)) + c + b;
    }
  },
  { "OutSine",
    [](double t, double b, double c, double d) {
      return (c - b) * std::sin(t / d * Math::PiOver2) + b;
    }
  },
  { "InOutSine",
    [](double t, double b, double c, double d) {
      return -(c - b) / 2.0 * (std::cos(Math::Pi * t / d) - 1) + b;
    }
  },
  { "InExpo",
    [](double t, double b, double c, double d) {
      return (c - b) * std::pow(2.0, 10 * (t / d - 1)) + b;
    }
  },
  { "OutExpo",
    [](double t, double b, double c, double d) {
      return (c - b) * (-std::pow(2.0, -10 * t / d) + 1) + b;
    }
  },
  { "InOutExpo",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return (c - b) / 2.0 * std::pow(2.0, 10 * (t - 1)) + b;
      }
      t--;
      return (c - b) / 2.0 * (-std::pow(2.0, -10 * t) + 2) + b;
    }
  },
  { "InCirc",
    [](double t, double b, double c, double d) {
      t /= d;
      return -(c - b) * (std::sqrt(1 - t * t) - 1) + b;
    }
  },
  { "OutCirc",
    [](double t, double b, double c, double d) {
      t /= d;
      t--;
      return (c - b) * std::sqrt(1 - t * t) + b;
    }
  },
  { "InOutCirc",
    [](double t, double b, double c, double d) {
      t /= d / 2.0;
      if (t < 1) {
        return -(c - b) / 2.0 * (std::sqrt(1 - t * t) - 1) + b;
      }
      t -= 2;
      return (c - b) / 2.0 * (std::sqrt(1 - t * t) + 1) + b;
    }
  }
};

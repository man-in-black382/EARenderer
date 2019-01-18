//
//  Color.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Color.hpp"

#include <glm/detail/func_exponential.hpp>

namespace EARenderer {

#pragma mark - Singletons

    const Color &Color::White() {
        static Color white(1.0, 1.0);
        return white;
    }

    const Color &Color::Black() {
        static Color black(0.0, 1.0);
        return black;
    }

    const Color &Color::Gray() {
        static Color gray(0.3, 1.0);
        return gray;
    }

    const Color &Color::Red() {
        static Color red(1.0, 0.0, 0.0, 1.0);
        return red;
    }

    const Color &Color::Blue() {
        static Color blue(0.0, 0.0, 1.0, 1.0);
        return blue;
    }

    const Color &Color::Green() {
        static Color green(0.0, 1.0, 0.0, 1.0);
        return green;
    }

#pragma mark - Lifecycle

    Color::Color(float red, float green, float blue, float alpha, Space space)
            : mR(red), mG(green), mB(blue), mA(alpha), mSpace(space) {}

    Color::Color(float red, float green, float blue, Space space) : Color(red, green, blue, 1.0, space) {}

    Color::Color(float white, float alpha, Space space) : Color(white, white, white, alpha, space) {}

    Color::Color(float white, Color::Space space) : Color(white, white, white, 1.0, space) {}

#pragma mark - Getters

    float Color::r() const {
        return mR;
    }

    float Color::g() const {
        return mG;
    }

    float Color::b() const {
        return mB;
    }

    float Color::a() const {
        return mA;
    }

    glm::vec3 Color::rgb() const {
        return {mR, mG, mB};
    }

    glm::vec4 Color::rgba() const {
        return {mR, mG, mB, mA};
    }

    Color::Space Color::space() const {
        return mSpace;
    }

    Color Color::toLinear() const {
        switch (mSpace) {
            case Color::Space::sRGB: {
                float power = 2.2;
                return {std::pow(mR, power), std::pow(mG, power), std::pow(mB, power), mA};
            }

            case Color::Space::YCoCg: {
                float t = mR - mB;
                float g = mR + mB;
                float b = t - mG;
                float r = t + mG;
                return {r, g, b, mA};
            }

            case Color::Space::Linear: {
                return *this;
            }
        }
    }

    Color Color::convertedTo(Color::Space space) const {
        Color linear = toLinear();

        switch (space) {
            case Space::Linear: {
                return linear;
            }

            case Space::sRGB: {
                float power = 1.0 / 2.2;
                return {std::pow(linear.mR, power), std::pow(linear.mG, power), std::pow(linear.mB, power), mA, Space::sRGB};
            }

            case Space::YCoCg: {
                float Co = (linear.mR - linear.mB) / 2.0;
                float t = linear.mB + Co;
                float Cg = (linear.mG - t) / 2.0;
                float Y = t + Cg;
                return {Y, Co, Cg, mA, Space::YCoCg};
            }
        }
    }

    Color Color::scaled(float scale) const {
        return { mR * scale, mG * scale, mB * scale, mA };
    }

}

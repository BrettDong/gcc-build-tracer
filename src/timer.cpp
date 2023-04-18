/*
* GCC Build Tracer
* Copyright (C) 2023 Binrui Dong
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timer.hpp"

#include <chrono>

namespace {

std::uint64_t compilation_epoch;

}

namespace tracer::timer {

std::uint64_t now() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
}

std::uint64_t to_us(std::uint64_t timestamp_diff) {
    const auto duration = std::chrono::steady_clock::duration{timestamp_diff};
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

void reset_epoch() {
    compilation_epoch = now();
}

std::uint64_t epoch() {
    return compilation_epoch;
}

} // namespace tracer::timer

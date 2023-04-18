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

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace tracer {

struct event {
    std::string name;
    std::string category;
    std::string phase;
    std::uint64_t timestamp;
};

class event_recorder {
  public:
    void finalize_session();
    void add_function(const char *name, std::uint64_t start, std::uint64_t end);

    void add_event(event &&e);
    void save_to_file();

    event_recorder();
    ~event_recorder();

  private:
    class impl;
    std::unique_ptr<impl> pimpl;
};

event_recorder &events();

} // namespace tracer

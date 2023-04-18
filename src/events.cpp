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

#include "events.hpp"
#include "timer.hpp"

#include <vector>

#include <gcc-plugin.h>
#include <json.h>

namespace tracer {

event_recorder &events() {
    static event_recorder instance;
    return instance;
}

class event_recorder::impl {
  public:
    std::vector<event> events;

    void finalize_session() {
        events.emplace_back(event{
            .name = "Session", .category = "Session", .phase = "B", .timestamp = timer::epoch()});
        events.emplace_back(event{
            .name = "Session", .category = "Session", .phase = "E", .timestamp = timer::now()});
    }

    void add_function(const char *name, std::uint64_t start, std::uint64_t end) {
        events.emplace_back(
            event{.name = name, .category = "Function", .phase = "B", .timestamp = start});
        events.emplace_back(
            event{.name = name, .category = "Function", .phase = "E", .timestamp = end});
    }

    void add_event(event &&e) {
        events.emplace_back(std::move(e));
    }

    void write_to_file() {
        auto json = new json::object();
        auto json_events = new json::array();
        auto pid = getpid();
        auto tid = gettid();
        for (const auto &e : events) {
            auto json_event = new json::object();
            json_event->set("name", new json::string(e.name.c_str()));
            json_event->set("cat", new json::string(e.category.c_str()));
            json_event->set("ph", new json::string(e.phase.c_str()));
            json_event->set("ts", new json::integer_number(static_cast<long>(
                                      timer::to_us(e.timestamp - timer::epoch()))));
            json_event->set("pid", new json::integer_number(pid));
            json_event->set("tid", new json::integer_number(tid));
            json_events->append(json_event);
        }
        json->set("traceEvents", json_events);
        FILE *f = fopen("/tmp/gcc-trace.json", "w");
        json->dump(f);
        fclose(f);
        delete json;
    }
};

event_recorder::event_recorder() {
    pimpl = std::make_unique<impl>();
}

event_recorder::~event_recorder() = default;

void event_recorder::finalize_session() {
    pimpl->finalize_session();
}

void event_recorder::add_function(const char *name, std::uint64_t start, std::uint64_t end) {
    pimpl->add_function(name, start, end);
}

void event_recorder::add_event(event &&e) {
    pimpl->add_event(std::move(e));
}

void event_recorder::save_to_file() {
    pimpl->write_to_file();
}

} // namespace tracer

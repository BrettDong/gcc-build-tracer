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

#include <gcc-plugin.h>
#include <plugin-version.h>

// needs to be included after gcc-plugin.h
#include <cp/cp-tree.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "events.hpp"
#include "timer.hpp"

namespace tracer {

std::unordered_map<std::string, std::uint64_t> start_parse_fn_timestamps;

void start_parse_fn_cb(void *gcc_data, void *user_data) {
    const auto now = timer::now();
    const auto decl = static_cast<tree>(gcc_data);
    const auto name = decl_as_string(decl, 0);
    if (start_parse_fn_timestamps.contains(name)) {
        std::cerr << "Error: " << name << " started twice" << std::endl;
        events().add_event(
            event{.name = name, .category = "FnStart", .phase = "i", .timestamp = now});
        return;
    }
    start_parse_fn_timestamps[name] = now;
}

void finish_parse_fn_cb(void *gcc_data, void *user_data) {
    const auto now = timer::now();
    const auto decl = static_cast<tree>(gcc_data);
    const auto name = decl_as_string(decl, 0);
    auto it = start_parse_fn_timestamps.find(name);
    if (it == start_parse_fn_timestamps.end()) {
        std::cerr << "Error: " << name << " finished but no start event received" << std::endl;
        events().add_event(
            event{.name = name, .category = "FnFinish", .phase = "i", .timestamp = now});
        return;
    }
    events().add_function(name, it->second, now);
    start_parse_fn_timestamps.erase(it);
}

void include_file_cb(void *gcc_data, void *user_data) {
    const auto now = timer::now();
    const auto path = static_cast<const char *>(gcc_data);
    events().add_event(event{.name = path, .category = "Include", .phase = "I", .timestamp = now});
}

void passes_start_cb(void *gcc_data, void *user_data) {
    const auto now = timer::now();
    events().add_event(
        event{.name = "PassStart", .category = "Passes", .phase = "I", .timestamp = now});
}

void passes_end_cb(void *gcc_data, void *user_data) {
    const auto now = timer::now();
    events().add_event(
        event{.name = "PassEnd", .category = "Passes", .phase = "I", .timestamp = now});
}

void plugin_finish_cb(void *gcc_data, void *user_data) {
    events().finalize_session();
    events().save_to_file();
}

} // namespace tracer

[[maybe_unused]] int plugin_is_GPL_compatible = 1;

int plugin_init(plugin_name_args *plugin_info, plugin_gcc_version *version) {
    if (!plugin_default_version_check(version, &gcc_version)) {
        return 1;
    }
    std::cerr << "Build Profiler plugin loaded" << std::endl;
    register_callback(plugin_info->base_name, PLUGIN_START_PARSE_FUNCTION,
                      tracer::start_parse_fn_cb, nullptr);
    register_callback(plugin_info->base_name, PLUGIN_FINISH_PARSE_FUNCTION,
                      tracer::finish_parse_fn_cb, nullptr);
    register_callback(plugin_info->base_name, PLUGIN_INCLUDE_FILE, tracer::include_file_cb,
                      nullptr);
    register_callback(plugin_info->base_name, PLUGIN_ALL_PASSES_START, tracer::passes_start_cb,
                      nullptr);
    register_callback(plugin_info->base_name, PLUGIN_ALL_PASSES_END, tracer::passes_end_cb,
                      nullptr);
    register_callback(plugin_info->base_name, PLUGIN_FINISH, tracer::plugin_finish_cb, nullptr);
    tracer::timer::reset_epoch();
    return 0;
}

/**
 * @file tp_args.hpp
 * @author Tadeusz Puźniakowski (anonymous@unknown.com)
 * @brief Simple header only arguments parsing library.
 * @version 0.1
 * @date 2022-04-21
 *
 * @copyright Copyright (c) 2022
 *
 * This library allows for simple parsing arguments in the format: -argname argvalue.
 *
 * @code {.c++ }
 * auto help = arg(argc, argv, "help", false);
 * auto x = arg(argc, argv,"x",10.0, "the x value");
 * @endcode
 *
 */
#ifndef __TP_ARGS_HPP____
#define __TP_ARGS_HPP____

#include <map>
#include <string>
#include <any>
#include <vector>
#include <stdexcept>

/**
 * @brief It is in the namespace tp::args and provides just two methods - parse_arguments, and arg
 *
 */

namespace tp::args {
    std::map<std::string, std::string> known_args;
    std::map<std::string, std::pair<double, double>> known_ranges;

    template<typename T>
    T arg(int argc, char **argv, std::string name, T default_value, std::string description = "", double min_val = 0.0,
          double max_val = 0.0) {
        using namespace std;
        any ret = default_value;
        for (int i = 0; i < argc; i++) {
            if ((std::string(argv[i]).substr(1) == name) && (argv[i][0] == '-')) {
                std::string argument = (i < (argc - 1)) ? argv[i + 1] : "1";
                if (std::is_same_v<T, bool>) {
                    ret = (argument == "true") || (argument == "1") || (argument == "yes") ||
                          ((argument.size() > 0) && (argument[0] == '-'));
                } else {
                    if (std::is_same_v<T, int>) ret = stoi(argument);
                    else if (std::is_same_v<T, double>) ret = stod(argument);
                    else if (std::is_same_v<T, unsigned>) ret = (unsigned) stoul(argument);
                    else if (std::is_same_v<T, unsigned long>) ret = stoul(argument);
                    else if (std::is_same_v<T, char>) ret = argument.at(0);
                    else ret = argument;
                }
                if (!(min_val == max_val)) {
                    if (stod(argument) < min_val)
                        throw std::invalid_argument(
                                "The argument -" + name + " must be above or equal " + std::to_string(min_val));
                    if (stod(argument) > max_val)
                        throw std::invalid_argument(
                                "The argument -" + name + " must be below or equal " + std::to_string(max_val));
                }
            }
        }
        known_args[name] = description;
        if (!(min_val == max_val)) known_ranges[name] = {min_val, max_val};
        return std::any_cast<T>(ret);
    };

    auto args_info = [](auto &o) {
        using namespace std;
#ifdef __TP_ARGS_MAX_ARG_NAME_LENGTH
        const int max_arg_name_length = __TP_ARGS_MAX_ARG_NAME_LENGTH;
#else
        const int max_arg_name_length = 12;
#endif
        for (auto [k, v]: known_args) {
            o << " -" << k << string(max((max_arg_name_length - (int) k.size()), 1), ' ')
              << v
              << ((known_ranges.count(k) == 0) ? "" : " (" + std::to_string(known_ranges[k].first) + "-" +
                                                      std::to_string(known_ranges[k].second) + ")") << "\n";
        }
    };
}


#endif
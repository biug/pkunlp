#pragma once

#ifdef _WIN32
    #include <Windows.h>
    #ifdef LIBGRASS_EXPORTS
    #define LIBGRASS_API __declspec(dllexport)
    #else
    #define LIBGRASS_API __declspec(dllimport)
    #endif
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    // Windows Header Files:
    #include <windows.h>
#else
    #ifdef LIBGRASS_EXPORTS
        #define LIBGRASS_API __attribute__((visibility("default")))
    #else
        #define LIBGRASS_API
    #endif
    #define CP_UTF8 65001
    #define CP_ACP 0
    typedef wchar_t WCHAR;
#endif

#include <string>
#include <vector>
#include <sstream>

// segment
LIBGRASS_API void train_segmentor(const std::string & train_file, const std::string & feature_file, const std::string & dict_file, int times, int encoding = CP_UTF8);

LIBGRASS_API void create_segmentor(const std::string & feature_file = "segfeat_PKU", const std::string & dict_file = "dic_PKU");

LIBGRASS_API void delete_segmentor();

LIBGRASS_API void seg_file(const std::string & input_file, const std::string & output_file, int encoding = CP_UTF8);

LIBGRASS_API std::vector<std::string> seg_string(const std::string & input, int encoding = CP_UTF8);

// postagging
LIBGRASS_API void train_postagger(const std::string & train_file, const std::string & feature_file, int times, int encoding = CP_UTF8);

LIBGRASS_API void create_postagger(const std::string & feature_file = "posfeat_PKU");

LIBGRASS_API void delete_postagger();

LIBGRASS_API void tag_file(const std::string & input_file, const std::string & output_file, int encoding = CP_UTF8);

LIBGRASS_API std::vector<std::pair<std::string, std::string>> tag_sentence(const std::vector<std::string> & input, int encoding = CP_UTF8);

// syntax tree
LIBGRASS_API void train_syntax_parser(const std::string & input_file, const std::string & feature_file, int round);

LIBGRASS_API void create_syntax_parser(const std::string & feature_file);

LIBGRASS_API void delete_syntax_parser();

LIBGRASS_API void syntax_parse_file(const std::string & input_file, const std::string & output_file, int encoding = CP_UTF8);

LIBGRASS_API std::string syntax_parse_string(const std::string & input, int encoding = CP_UTF8);

// semantic graph
LIBGRASS_API void train_semantic_parser(const std::string & input_file, const std::string & feature_file, int round);

LIBGRASS_API void create_semantic_parser(const std::string & semantic_feature_file, const std::string & tree_feature_file);

LIBGRASS_API void delete_semantic_parser();

LIBGRASS_API void semantic_parse_file(const std::string & input_file, const std::string & output_file, int encoding = CP_UTF8);

LIBGRASS_API std::string semantic_parse_string(const std::string & input, int encoding = CP_UTF8);

// helper
LIBGRASS_API void sentence_per_line(const std::string & input_file, const std::string & output_file, int encoding = CP_UTF8);
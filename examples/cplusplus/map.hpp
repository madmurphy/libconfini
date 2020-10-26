/*

This is a simple example of how to construct a C++ class that relies on a
`std::unordered_map` object to store the data dispatched by **libconfini**.
This framework exploits most of the library's features, but not all. Missing
features include:

- It is possible to parse only a path or an INI string (but not a `FILE`
  handle)
- Comments and disabled entries are ignored
- Number parsing is not implemented

The `IniMap` class can be easily expanded further to include the features
listed above. The `std::unordered_map` is publicly available as
`IniMap::dictionary`.

Here the list of all its public members:

- `IniMap::mapPath()`
- `IniMap::mapINIString()`
- `IniMap::hasKey()`
- `IniMap::getSource()`
- `IniMap::getBool()`
- `IniMap::isntFalse()`
- `IniMap::getString()`
- `IniMap::getStringArray()`
- `IniMap::format()`
- `IniMap::dictionary`

*/

#include <string>
#include <unordered_map>
#include <confini.h>

#define __INI_MAP_DEFAULT_FORMAT__ { \
    /* .delimiter_symbol = */ INI_EQUALS, \
    /* .case_sensitive = */ false, \
    \
    /*   We ignore comments and disabled entries...  */ \
    /* .semicolon_marker = */ INI_IGNORE, \
    /* .hash_marker = */ INI_IGNORE, \
    \
    /* .section_paths = */ INI_ABSOLUTE_AND_RELATIVE, \
    /* .multiline_nodes = */ INI_MULTILINE_EVERYWHERE, \
    /* .no_single_quotes = */ false, \
    /* .no_double_quotes = */ false, \
    /* .no_spaces_in_names = */ false, \
    /* .implicit_is_not_empty = */ false, \
    /* .do_not_collapse_values = */ false, \
    /* .preserve_empty_quotes = */ false, \
    /* .disabled_after_space = */ false, \
    /* .disabled_can_be_implicit = */ false \
  }

struct IniMap {

  public:

    int mapPath (
      const std::string path,
      const IniFormat format = __INI_MAP_DEFAULT_FORMAT__,
      const IniDispHandler callback = _push_dispatch_
    );

    int mapINIString (
      const std::string path,
      const IniFormat format = __INI_MAP_DEFAULT_FORMAT__,
      const IniDispHandler callback = _push_dispatch_
    );

    bool hasKey (
      const std::string key
    );

    bool getBool (
      const std::string key,
      bool * const is_bool = NULL
    );

    bool isntFalse (
      const std::string key,
      bool * const is_bool = NULL
    );

    std::string getString (
      const std::string key
    );

    char * const * getStringArray (
      const std::string key,
      size_t * const dest_arrlen,
      char delimiter
    );

    std::string getSource (
      const std::string key
    );

    IniFormat format () { return this->_format_; }

    std::unordered_map<std::string,
      std::string> dictionary;


  private:

    IniFormat _format_;

    static int _push_dispatch_ (
      IniDispatch * const disp,
      void * const v_dictionary
    );

};

#undef __INI_MAP_DEFAULT_FORMAT__

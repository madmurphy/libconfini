/*

All methods except `IniMap::getSource()` are independent from each other and
can be deleted. Feel free to adapt this exaxmple to your needs.

*/

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstring>
#include <confini.h>
#include "map.hpp"

using namespace std;


static inline void chrarr_tolower (char * const str) {
    for (register char * chrptr = str; *chrptr; chrptr++) {
        *chrptr = *chrptr > 0x40 && *chrptr < 0x5b ? *chrptr | 0x60 : *chrptr;
    }
}

/*

In this implementation the dot is a metacharacter and is used as delimiter
between path parts. Therefore all dots appearing in key names, or (within
quotes) in section paths, will be replaced with the following character. You
may change it with any other character (except the dot itself).

*/
#define DOT_REPLACEMENT '-'


/*  Parse a path pointing to an INI file  */
int IniMap::mapPath (const string path, const IniFormat format, const IniDispHandler callback) {
    this->dictionary.clear();
    this->_format_ = format;
    return load_ini_path(path.c_str(), format, NULL, callback, &dictionary);
}


/*  Parse a string containing an INI file  */
int IniMap::mapINIString (const string ini_string, const IniFormat format, const IniDispHandler callback) {
    this->dictionary.clear();
    this->_format_ = format;
    size_t len = ini_string.length();
    char * tmp = new char[len + 1];
    memcpy(tmp, ini_string.c_str(), len + 1);
    int retval = strip_ini_cache(tmp, len, format, NULL, callback, &dictionary);
    delete tmp;
    return retval;
}


/*  Check if a key exists  */
bool IniMap::hasKey (const string key) {
    return dictionary.count(key) ? true : false;
}


/*  Get the INI source string of a value  */
string IniMap::getSource (const string key) {
    if (!dictionary.count(key)) {
        /*  You can change this to `return ""` if you prefer...  */
        throw runtime_error("Key not present.");
    }
    return dictionary.at(key);
}


/*  Get a value as `bool`  */
bool IniMap::getBool (const string key, bool * const is_bool) {
    unsigned int intbool = static_cast<unsigned int>(ini_get_bool_i(this->getSource(key).c_str(), 2, this->_format_));
    if (is_bool) {
        *is_bool = !(intbool & 2);
    }
    return static_cast<bool>(intbool & 1);
}


/*  Like `IniMap::getBool`, but defaults to `true`  */
bool IniMap::isntFalse (const string key, bool * const is_bool) {
    unsigned int intbool = static_cast<unsigned int>(ini_get_bool_i(this->getSource(key).c_str(), 3, this->_format_));
    if (is_bool) {
        *is_bool = !(intbool & 2);
    }
    return static_cast<bool>(intbool & 1);
}


/*  Get a value parsed as a std::string  */
string IniMap::getString (const string key) {
    const string srcstr = this->getSource(key);
    size_t len = srcstr.length();
    char * tmp = new char[len + 1];
    memcpy(tmp, srcstr.c_str(), len + 1);
    len = ini_string_parse(tmp, this->_format_);
    string myString(tmp, len);
    delete tmp;
    return myString;
}


/*  Get a value parsed as an array of C strings (`char **`)  */
/*  Remember to `delete` the array returned after using it!  */
char * const * IniMap::getStringArray (
    const string key,
    size_t * const dest_arrlen,
    const char delimiter
) {

    const string srcstr = this->getSource(key);
    *dest_arrlen = ini_array_get_length(srcstr.c_str(), delimiter, this->_format_);

    if (!*dest_arrlen) {

        return NULL;

    }

    char ** const newarr = reinterpret_cast<char **>(
        (void *) operator new(*dest_arrlen * sizeof(char *) + srcstr.length() + 1)
    );
    char * remnant = reinterpret_cast<char *>((char **) newarr + *dest_arrlen);
    memcpy(remnant, srcstr.c_str(), srcstr.length() + 1);

    for (size_t idx = 0; idx < *dest_arrlen; idx++) {

        newarr[idx] = ini_array_release(&remnant, delimiter, this->_format_);
        ini_string_parse(newarr[idx], this->_format_);

    }

    return (char * const *) newarr;

}


/*  Private `IniDispHandler`  */
int IniMap::_push_dispatch_ (IniDispatch * const disp, void * const v_dictionary) {

    #define thismap (reinterpret_cast<unordered_map<string, string> *>(v_dictionary))

    if (disp->type != INI_KEY) {

        return 0;

    }

    size_t idx;
    char * newptr1, * newptr2, * oldptr1, * oldptr2;
    disp->d_len = ini_unquote(disp->data, disp->format);

    /*  remove quoted dots from parent  */
    if (disp->at_len) {

        /*  has parent  */
        newptr1 = newptr2 = new char[disp->at_len + 1];
        *((const char **) &oldptr2) = disp->append_to;

        while ((oldptr1 = oldptr2)) {

            idx = ini_array_shift((const char **) &oldptr2, '.', disp->format);
            newptr1[idx] = '\0';

            while (idx > 0) {

                --idx;
                newptr1[idx] = oldptr1[idx] == '.' ? DOT_REPLACEMENT : oldptr1[idx];

            }

            newptr1 += ini_unquote(newptr1, disp->format);
            *newptr1++ = '.';

        }

        idx = newptr1 - newptr2;
        newptr1 = new char[idx + disp->d_len + 1];
        memcpy(newptr1, newptr2, idx);
        delete newptr2;
        newptr2 = newptr1 + idx;

    } else {

        /*  parent is root  */
        newptr1 = newptr2 = new char[disp->d_len + 1];

    }

    /*  remove dots from key name  */
    idx = disp->d_len + 1;

    do {

        --idx;
        newptr2[idx] = disp->data[idx] == '.' ? DOT_REPLACEMENT : disp->data[idx];

    } while (idx > 0);

    if (!disp->format.case_sensitive) {

        chrarr_tolower(newptr1);

    }

    string key = string(newptr1, newptr2 - newptr1 + disp->d_len);
    delete newptr1;

    /*  check for duplicate keys  */
    if (thismap->count(key)) {

        cerr << "`" << key << "` will be overwritten (duplicate key found)\n";
        thismap->erase(key);

    }

    thismap->insert(
        pair<string, string>(
            key,
            disp->value ? string(disp->value, disp->v_len) : ""
        )
    );

    return 0;

    #undef thismap
}



int main () {

    IniMap myINIFile;
    myINIFile.mapPath("../ini_files/typed_ini.conf");

    cout << "my_section.my_string -> " << myINIFile.getString("my_section.my_string") << "\n";
    cout << "my_section.my_number -> " << ini_get_int(myINIFile.getString("my_section.my_number").c_str()) << "\n";

    bool bool_found;
    bool my_bool = myINIFile.getBool("my_section.my_boolean", &bool_found);
    cout << "my_section.my_boolean -> " << (!bool_found ? "not found" : my_bool ? "true" : "false") << "\n";

    cout
        << "my_section.my_implicit_boolean -> "
        << (myINIFile.isntFalse("my_section.my_implicit_boolean") ? "true" : "false") << "\n";

    size_t my_arrlen;
    char * const * my_array = myINIFile.getStringArray("my_section.my_array", &my_arrlen, ',');
    cout << "my_section.my_array[" << my_arrlen << "] -> [" << my_array[0];
    for (size_t idx = 1; idx < my_arrlen; idx++) {
        cout << "|" << my_array[idx];
    }
    cout << "]\n";
    delete my_array;

    /*  You could also try...  */
    /*
    IniMap myINIString;
    myINIString.mapINIString("[test]\ntestkey = testval");
    cout << "test.testkey -> " << myINIString.getString("test.testkey") << "\n";
    */

    /*

    For using the library's original functions, you can always rely on
    `myINIFile.getSource(...).c_str()`:

        IniMap myINIFile;
        ...
        size_t array_length = ini_array_get_length(
            myINIFile.getSource("my_section.my_array").c_str(),
            ',',
            myINIFile.format()
        );

    */

    return 0;

}


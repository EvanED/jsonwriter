#ifndef STL2JSON_HH
#define STL2JSON_HH

#include <map>
#include <iostream>
#include <vector>
#include <string>

namespace stl_to_json {

    template <typename T>
    void
    serialize(std::ostream & os, std::vector<T> const & vec);

    template <typename U, typename V>
    void
    serialize(std::ostream & os, std::map<U, V> const & vec);


    void
    serialize(std::ostream & os, int i)
    {
        os << i;
    }

    void
    serialize(std::ostream & os, std::string const & str)
    {
        // Um, just assume there are no special characters for now I guess
        os << "\"" << str << "\"";
    }

    template <typename U, typename V>
    void
    serialize(std::ostream & os, std::pair<U, V> const & p)
    {
        os << "[";
        serialize(os, p.first);
        os << ",";
        serialize(os, p.second);
        os << "]";
    }

    template <typename T>
    void
    serialize(std::ostream & os, std::vector<T> const & vec)
    {
        os << "[";
        bool first = true;
        for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
            if (first) {
                first = false;
            }
            else {
                os << ", ";
            }
            serialize(os, *it);
        }
        os << "]";
    }

    template <typename U, typename V>
    void
    serialize(std::ostream & os, std::map<U, V> const & vec)
    {
        os << "{";
                      
        bool first = true;
        for (typename std::map<U, V>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
            if (first) {
                first = false;
            }
            else {
                os << ", ";
            }
            serialize(os, it->first);
            os << ": ";
            serialize(os, it->second);
        }
        os << "}";
    }
}

// Yo emacs!
// Local Variables:
//     c-file-style: "ellemtel"
//     c-basic-offset: 4
//     indent-tabs-mode: nil
// End:

#endif /* STL2JSON_HH */

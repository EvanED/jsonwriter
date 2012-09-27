#ifndef STL2JSON_HH
#define STL2JSON_HH

#include <map>
#include <iostream>
#include <vector>
#include <string>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/stringize.hpp>

#define MY_SEQUENCE_CREATOR_0(a,b) ((a,b)) MY_SEQUENCE_CREATOR_1
#define MY_SEQUENCE_CREATOR_1(a,b) ((a,b)) MY_SEQUENCE_CREATOR_0
#define MY_SEQUENCE_CREATOR_0_END
#define MY_SEQUENCE_CREATOR_1_END


#define STL_TO_JSON_INTERNAL_DEFINE_STRUCT(struct_name, fields)  \
    struct struct_name {                                \
        BOOST_PP_SEQ_FOR_EACH(                          \
            STL_TO_JSON_INTERNAL_DEFINE_FIELD,          \
            ~,                                          \
            fields)                                     \
    };

#define STL_TO_JSON_INTERNAL_DEFINE_SERIALIZE(struct_name, fields) \
    void serialize(std::ostream & os, struct_name const & s) {     \
        os << "{ ";                                                \
        BOOST_PP_SEQ_FOR_EACH(                                     \
            STL_TO_JSON_INTERNAL_OUTPUT_FIELD,                     \
            (os, s),                                               \
            fields)                                                \
        os << "} ";                                                \
    }

#define STL_TO_JSON_INTERNAL_DEFINE_FIELD(r, data, field_tuple)    \
    BOOST_PP_TUPLE_ELEM(2, 0, field_tuple)    \
    BOOST_PP_TUPLE_ELEM(2, 1, field_tuple);

#define STL_TO_JSON_INTERNAL_OUTPUT_FIELD2(os, s, fld) \
    os << BOOST_PP_STRINGIZE(fld) << ": " << s.fld << "\n";

#define STL_TO_JSON_INTERNAL_OUTPUT_FIELD(r, params, field_tuple) \
    STL_TO_JSON_INTERNAL_OUTPUT_FIELD2(                    \
        BOOST_PP_TUPLE_ELEM(2, 0, params),                 \
        BOOST_PP_TUPLE_ELEM(2, 1, params),                 \
        BOOST_PP_TUPLE_ELEM(2, 1, field_tuple))

#define STL_TO_JSON_DEFINE_STRUCT_ONLY(struct_name, fields) \
    STL_TO_JSON_INTERNAL_DEFINE_STRUCT(struct_name,         \
        BOOST_PP_CAT(MY_SEQUENCE_CREATOR_0 fields, _END))

#define STL_TO_JSON_DEFINE_SERIALIZE(struct_name, field_pairs)  \
    STL_TO_JSON_INTERNAL_DEFINE_SERIALIZE(struct_name,          \
        BOOST_PP_CAT(MY_SEQUENCE_CREATOR_0 field_pairs, _END))

#define STL_TO_JSON_DEFINE_SERIALIZED_STRUCT(struct_name, fields) \
    STL_TO_JSON_DEFINE_STRUCT_ONLY(struct_name, fields)           \
    STL_TO_JSON_DEFINE_SERIALIZE(struct_name, fields)


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



#ifndef STL2JSON_HH
#define STL2JSON_HH

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/stringize.hpp>


/// THIS IS AN INTERNAL MACRO
// The following is used for transforming a sequence like
//   (a, b)(c, d)(e, f)
// into one with double parentheses:
//   ((a, b))((c, d))((e, f))
// which is needed by the SEQ_FOR_EACH macro.
//
// See
//   http://article.gmane.org/gmane.comp.lib.boost.user/75703
//   http://article.gmane.org/gmane.comp.lib.boost.user/75702
// (primarily the first).
#define MY_SEQUENCE_CREATOR_0(a,b) ((a,b)) MY_SEQUENCE_CREATOR_1
#define MY_SEQUENCE_CREATOR_1(a,b) ((a,b)) MY_SEQUENCE_CREATOR_0
#define MY_SEQUENCE_CREATOR_0_END
#define MY_SEQUENCE_CREATOR_1_END


/// THIS IS AN INTERNAL MACRO. Use STL_TO_JSON_DEFINE_STRUCT_ONLY
/// or STL_TO_JSON_DEFINE_SERIALIZED_STRUCT instead.
///
/// Given the name of a struct and a sequence of fields,
/// expands to a definition of that structure.
///
/// The fields must be double-parenthesized, e.g.
/// ((int, x))((int, y))((double, z)).
//
// The ~ is meant as a placeholder; it is passed to
// STL_TO_JSON_INTERNAL_DEFINE_FIELD in the 'data' parameter, but is
// then unused.
#define STL_TO_JSON_INTERNAL_DEFINE_STRUCT(struct_name, fields)  \
    struct struct_name {                                \
        BOOST_PP_SEQ_FOR_EACH(                          \
            STL_TO_JSON_INTERNAL_DEFINE_FIELD,          \
            ~,                                          \
            fields)                                     \
    };


/// THIS IS AN INTERNAL MACRO. Use STL_TO_JSON_DEFINE_SERIALIZE
/// or STL_TO_JSON_DEFINE_SERIALIZED_STRUCT instead.
///
/// Given the name of a struct and a list of fields, expands to
/// corresponding definition of a serialize function.
///
/// The fields must be double-parenthesized, e.g.
/// ((int, x))((int, y))((double, z)).
#define STL_TO_JSON_INTERNAL_DEFINE_SERIALIZE(struct_name, fields) \
    void serialize(std::ostream & os, struct_name const & s) {     \
        os << "{ ";                                                \
        BOOST_PP_SEQ_FOR_EACH_I(                                   \
            STL_TO_JSON_INTERNAL_OUTPUT_FIELD,                     \
            (os, s),                                               \
            fields)                                                \
        os << "} ";                                                \
    }


/// THIS IS AN INTERNAL MACRO.
///
/// Expands to the definition of a field given in 'field_tuple'. The
/// 'field_tuple' should be a pair (type, name) which specifies the
/// given field.
///
/// The 'r' field and 'data' fields are ignored. 'r' is used to
/// improve the efficiency of the preprocessing (I think to improve
/// additional calls to iteration constructs, but we're not using any
/// more so it doesn't matter), and 'data' corresponds to the second
/// parameter to 'BOOST_PP_SEQ_FOR_EACH', which in this case is ~.
#define STL_TO_JSON_INTERNAL_DEFINE_FIELD(r, data, field_tuple) \
    BOOST_PP_TUPLE_ELEM(2, 0, field_tuple)    \
    BOOST_PP_TUPLE_ELEM(2, 1, field_tuple);


/// THIS IS AN INTERNAL MACRO.
///
/// Expands to a line that outputs a field of a structure,
/// possibly with a preceeding comma.
///
/// 'os'  specifies the stream to receive the output
/// 's'   is the name of the structure to look in
/// 'fld' is the name of the field to look at
/// 'i'   determines whether the macro should also
///       include an extra 'os << ", ";' at the start;
///       if 'i' is non-zero, it is included.
///
/// (In context, 'i' is used as a counter: it says that 'fld' is the
/// 'i'th field. Then the first field (when 'i==0') shouldn't have a
/// preceeding comma since it is the first one, but all others
/// should.)
#define STL_TO_JSON_INTERNAL_OUTPUT_FIELD2(os, s, i, fld) \
    BOOST_PP_EXPR_IF(i, os << ", ";)                   \
    os << BOOST_PP_STRINGIZE(fld) << ": ";             \
    stl_to_json::do_serialize(os, s.fld);              \
    os << "\n";


/// THIS IS AN INTERNAL MACRO
///
/// Like STL_TO_JSON_INTERNAL_OUTPUT_FIELD2, but with parameters bound
/// up: 'params' in this macro is '(os, s)', and 'field_tuple' is
/// '(_, fld)' (where the first item in that pair is the type of the
/// field and is unused).
///
/// The two stage macro is there to make the ..._FIELD2 macro more
/// readable, so it's not extracting things from the field_tuple
/// (which takes a lot of text) in the midst of other code.
#define STL_TO_JSON_INTERNAL_OUTPUT_FIELD(r, params, i, field_tuple) \
    STL_TO_JSON_INTERNAL_OUTPUT_FIELD2(                    \
        BOOST_PP_TUPLE_ELEM(2, 0, params),                 \
        BOOST_PP_TUPLE_ELEM(2, 1, params),                 \
        i,                                                 \
        BOOST_PP_TUPLE_ELEM(2, 1, field_tuple))


/// Expands to a struct definition with the given name and fields.
///
/// 'struct_name' is a normal C/C++ identifier which will be defined
/// by this macro. 'fields' is a sequence of the form
/// '(type1, name1)(type2, name2)...(typeN, nameN)' that lists the
/// fields that will be in the structure. (Note that there is no comma
/// between the fields, but there is one between the type and name.)
///
/// This isn't strictly an internal macro, but it does seem strange if
/// you'd call it directly.
#define STL_TO_JSON_DEFINE_STRUCT_ONLY(struct_name, fields) \
    STL_TO_JSON_INTERNAL_DEFINE_STRUCT(struct_name,         \
        BOOST_PP_CAT(MY_SEQUENCE_CREATOR_0 fields, _END))


/// Expands to an overload of specialize() for the given struct.
///
/// WARNING: The interface of this macro may change in the future. The
///          change would be to stop requiring the types of the
///          fields, meaning that 'field_pairs' would turn into a
///          sequence of names (like '(a)(b)(c)') instead.
///
/// The function will have the signature
///     void specialize(std::ostream & os, struct_name const &)
/// and appear in the current namespace. For a description of this
/// function, see STL_TO_JSON_DEFINE_SERIALIZED_STRUCT; this macro
/// works the same way but on an existing structure type.
///
/// 'fields' is a sequence of the form '(type1, name1)(type2,
/// name2)...(typeN, nameN)' that lists the fields that will be
/// output. It does not need to be a complete list, and the fields
/// will be output in the order they are listed. (Note that there is
/// no comma between the fields, but there is one between the type and
/// name.) The types are ignored, so you could also put a dummy
/// (non-empty) value like '~' for the type.
///
/// You probably want to use this in the same namespace as the struct
/// was defined in, because then argument-dependent lookup will find
/// it without needing to be explicitly qualified. (This is necessary
/// if you want to put objects of this struct into a list, map,
/// etc. and then serialize those, unless you take other steps to make
/// it possible.)
#define STL_TO_JSON_DEFINE_SERIALIZE(struct_name, field_pairs)  \
    STL_TO_JSON_INTERNAL_DEFINE_SERIALIZE(struct_name,          \
        BOOST_PP_CAT(MY_SEQUENCE_CREATOR_0 field_pairs, _END))


/// Defines a structure and corresponding serialize() overload.
///
/// This macro expands to a definition of a struct with the given name
/// and fields along with an overloaded serialize() definiton
/// corresponding to that struct. Both definitions go into the current
/// namespace.
///
/// 'struct_name' is the name of the structure that is being defined.
///
/// 'fields' is a sequence of the form '(type1, name1)(type2,
/// name2)...(typeN, nameN)' that lists the fields that will be
/// output. It does not need to be a complete list, and the fields
/// will be output in the order they are listed. (Note that there is
/// no comma between the fields, but there is one between the type and
/// name.)
///
/// The specialize() function will have the signature
///     void specialize(std::ostream & os, struct_name const &)
///
/// The behavior will be to print out a JSON object where the keys are
/// the field names quoted, and the values are serialized descriptions
/// of the values.
#define STL_TO_JSON_DEFINE_SERIALIZED_STRUCT(struct_name, fields) \
    STL_TO_JSON_DEFINE_STRUCT_ONLY(struct_name, fields)           \
    STL_TO_JSON_DEFINE_SERIALIZE(struct_name, fields)


namespace stl_to_json {

    /// Serialize the given vector to the given stream as a JSON
    /// array.
    ///
    /// The vector element types will be serialized by the overload
    /// that works on that type.
    template <typename T>
    void
    serialize(std::ostream & os, std::vector<T> const & vec);


    /// Serialize the given map to the given stream as a JSON object.
    ///
    /// The map's key type better serialize to a string (as that's
    /// what JSON requires), but no effort is made to ensure this.
    template <typename U, typename V>
    void
    serialize(std::ostream & os, std::map<U, V> const & vec);


    /// Serialize the given integer to the stream as a JSON number.
    ///
    /// JSON can only use decimal numbers, so every number will be
    /// serialized that way.
    void
    serialize(std::ostream & os, int i)
    {
        os << i;
    }


    /// Serialize the given number to the given stream, as a JSON
    /// number.
    ///
    /// All JSON numbers are in decimal.
    ///
    /// WARNING: I believe that JavaScript uses 64-bit floating points
    ///          for all numbers. If your JSON parser always does the
    ///          same, then a 64-bit long will have unrepresentable
    ///          values.
    ///
    ///          No effort is currently made to detect this situation,
    ///          though that'd be cool. (TODO)
    void
    serialize(std::ostream & os, unsigned long l)
    {
        os << l;
    }


    /// Serialize the given string to the given stream, as a JSON
    /// stream.
    ///
    /// FIXME: No actual escaping is currently done. JSON requires
    ///        that ", \, and control characters be escaped, and
    ///        allows the following escape sequences:
    ///
    ///        ", \, /, b [backspace], f [form feed], n, r, t,
    ///        and uXXXX [hex digits]
    void
    serialize(std::ostream & os, std::string const & str)
    {
        // Um, just assume there are no special characters for now I guess
        os << "\"" << str << "\"";
    }


    /// Serialize the given pair to the stream, as a two-element JSON
    /// array.
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


    /// This is a wrapper function, used to get ADL on the parameter.
    ///
    /// WARNING: Do not overload or specialize this function for your
    ///          type and expect things to work, especially right
    ///          now. The library doesn't currently call this itself,
    ///          so if you have a vector<T> and specialize/overload
    ///          this function for T, it won't be called. Even after
    ///          this is fixed, third-party code may not call the
    ///          right function.
    ///
    /// If you need to serialize an arbitrary type, calling this
    /// function will allow for picking up *both* the serialize()
    /// functions that this library provides (for base types and
    /// vectors, maps, etc.) as well as any serialize() function
    /// defined in the same namespace as the type.
    ///
    /// For instance, the serialize() functions that are defined by
    /// this library's DEFINE_SERIALIZE and DEFINE_SERIALIZED_STRUCT
    /// macros need to serialize each field. However, neither a
    /// qualified or unqualified call will do the trick; a qualified
    /// call (stl_to_json::serialize(...)) would prevent serializing a
    /// field whose type has a serialize overload (unless it's defined
    /// in stl_to_json, of course), but an unqualified name would
    /// prevent looking in namespace stl_to_json for primitive types
    /// and STL types. This wrapper solves the problem: the macros
    /// generate calls to do_serialize() which uses an unqualified
    /// call to serialize(). Custom types are picked up via
    /// argument-dependent lokup (ADL, or "Koenig lookup"), and base
    /// types and STL types are picked up because do_serialize() is in
    /// the same namespace.
    template<typename T>
    void
    do_serialize(std::ostream & os, T const & val)
    {
        serialize(os, val);
    }
}

// Yo emacs!
// Local Variables:
//     c-file-style: "ellemtel"
//     c-basic-offset: 4
//     indent-tabs-mode: nil
// End:

#endif /* STL2JSON_HH */



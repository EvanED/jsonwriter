libjsonwriter, a JSON output library for C++
============================================

`libjsonwriter` provides a library for easy serialization of C++
objects to a JSON format (also usable as a Python literal as well as
YAML). It provides output operations for common C++ types including
STL containers, and is *extensible*, meaning you can provide an
outputter for your own format, and is *simple to use*.

Building an installation
------------------------

This library currently consists of a single header, so there's no
building to be done at all. Just copy that header to where you want.

All examples in this README are also available in the `examples/`
directory of the distribution. Build them by changing to that
directory and running `scons`. (You'll need
[SCons](http://www.scons.org) installed, of course.) `ex1` needs a
compiler capable of compiling C++11's initializer list support; if you
are using GCC, it will automatically pass `-std=c++0x` to your
compiler.

In addition, I should probably write some tests one of these days.

== How to serialize an object ==

Serializing an object for which all component types have a `serialize`
function defined is easy:

    #include <iostream>
    #include <vector>
    #include <jsonwriter/writer.hh>

    int main() {
        std::vector<int> v = {1, 4, 9, 16};
        jsonwriter::serialize(std::cout, v);
        std::cout << "\n";
    }

Simply call `serialize`; the first parameter is an `std::ostream &` to
write to and the second parameter is the object to be printed. This
program will produce the output

    [1, 4, 9, 16]


== Provided serializers ==

The `serialize` function is overloaded for different types;
`libjsonwriter` provides overloads for the following:

* `int` [will likely change to `long long` in the future]
* `unsigned long` [will likely change to `unsigned long long`]
* `std::string` [will likely provide an overload for `basic_string`]
* `std::vector<T>`, outputted as a JSON array
* `std::map<T,U>`, outputted as a JSON object
* `std::pair<T,U>`, outputted as a two-element JSON array

*Note:* because JSON requires the keys in an object be strings
 (because in Javascript the literal {"a": 1, "b": 2}` denotes an
 object `o` with fields `o.a` and `o.b`), when serializing an
 `std::map` the keys must serialize to a string. (In other words, the
 key must be either an `std::string` or some custom type that is
 rendered as one.) No effort is currently made to ensure that this is
 true, and if it is not, then you will get invalid JSON. (A future
 version of the library may check whether the result of serializing
 each key parses as a string, and wrap it if not.)


== Extending the serializer for your own types [basic use] ==

To serialize your own type, you simply provide an overload of
`serialize` that operates on it. Like the standard ones, the first
parameter should take an `std::ostream` (by non-const reference, of
course) and the second parameter should be the type in question (by
value, reference, or const reference). To be fully usable, this
overload must appear in either the same namespace as the type you want
to serialize or in the `jsonwriter` namespace. (See two sections
below.)

*Important:* At a later date, this will be rearranged so you will need
to overload a function called `do_serialize` instead.


For instance:

    #include <iostream>
    #include <vector>
    #include <string>

    #include <jsonwriter/writer.hh>

    namespace my_library {
        // Define a simple Person class
        class Person {
            std::string name_;
        public:
            Person(std::string const & name)
                : name_(name)
            {}
            std::string get_name() const {
                return name_;
            }
        };


        // Overload the serialize function. Will output
        // something like '{ "name": "Fred Bob" }'
        void serialize(std::ostream & out,
                       Person const & p)
        {
            out << "{ \"name\": ";
	    // We could just say 'out << p.get_name();',
	    // but calling the usual 'serialize' function
            // will mean that we don't have to deal with
            // adding quotes ourselves, or people with
            // special characters (like " or \) in their
            // names (unlikely as that may be).
	    jsonwriter::serialize(out, p.get_name());
	    out << " }";
        }
    }

    using my_library::Person;

    int main() {
        std::vector<Person> vp;
        vp.push_back(Person("Bruce Wayne"));
        vp.push_back(Person("Clark Kent"));
        vp.push_back(Person("Tony Stark"));
        jsonwriter::serialize(std::cout, vp);
	std::cout << "\n";
    }

This example will output the following JSON (reformatted for
readability):

    [ { "name": "Bruce Wayne" },
      { "name": "Clark Kent"  },
      { "name": "tony Stark"  }
    ]


== Outputting structures ==

If you have a structure (or class with public fields), you can easily
define a `serialize` function for it using the macro
`JSONWRITER_DEFINE_SERIALIZE`. This takes two parameters; the first is
the name of the structure type and the second is a sequence of
fields of the form `(type1, name1)(type2, name2)...(typeN,
nameN)`. Note that there is no comma between the fields, but there is
one within each.

*Note:* This will change in the future to omit the types, to be
something like `(name1)(name2)...(nameN)`.

This macro will expand to a definition of `serialize` in the current
namespace (thus you should use this in the same namespace as the type
definition) that outputs the struct as a JSON object: the keys are the
names of the fields as strings and the values are the serialized field
values.

There is no need to list all the fields in a structure, and the struct
doesn't have to be POD.

There is one more convenience macro,
`JSONWRITER_DEFINE_SERIALIZED_STRUCT`, which defines both the
structure and `serialize` overload in one fell swoop. This is useful
if you don't have an explicit representation of the data you want to
output, and would prefer to build up essentially an AST then output
that. The use is the same as `JSONWRITER_DEFINE_SERIALIZE`. (The
signature of this will *not* change to eliminate the types, however.)

Here is an example:

    #include <iostream>
    #include <vector>
    #include <string>

    #include <jsonwriter/writer.hh>

    using std::string;

    JSONWRITER_DEFINE_SERIALIZED_STRUCT(Address,
        (std::vector<string>, lines)
        (string, city)
        (string, state)
        (string, postal_code))

    JSONWRITER_DEFINE_SERIALIZED_STRUCT(Person,
        (string,  name)
        (Address, address)
        (int,     age))

    int main() {
        Address address;
        address.lines.push_back = "1007 Mountain Drive";
        address.city = "Gotham";
        address.state = "DC";
        address.postal_code = "12345";

        Person batman;
        batman.name = "Batman";
        batman.address = address;
        batman.age = 30;

        serialize(std::cout, batman);
	std::cout << "\n";
    }

This prints (reformatted):

    {   "name": "Batman",
        "address": {
            "lines": [ "1007 Mountain Drive" ],
            "city": "Gotham",
            "state": "DC",
            "postal_code": "12345" },
        "age": 30
    }

At a later time, I may add the ability to use accessor functions
("getters") instead of just fields. If you want this feature, drop me
a line and maybe I'll add it. (Or propose a patch!)


The above is enough information for practical use. However, if you
would like a deeper understanding of how pieces fit together, continue
reading.


== A whirlwind explanation of Argument-Dependent Lookup ==

There is a key feature of C++ called argument-dependent lookup (ADL),
informally also referred to as Koenig Lookup. The process defined by
ADL describes how the compiler resolves each particluar function call
to determine the actual callee.

In particular, ADL specifies that for a function call like `foo(x,
y)`, the C++ compiler will look in the namespaces of `x`'s and `y`'s
types in addition to any of the places it would look if you just
called `foo()` (the current namespace, it's parents, and anything
explicitly `using`ed).

(This feature was introduced mostly because of operator
overloading. The function `operator<<(ostream&, int)` is defined in
the `std` namespace, but you don't have to want to say something like
`std::cout std::<< 5;` or something like that. Instead, we would
prefer that the compiler look for `operator<<` in the same namespace
as `std::cout`.)

`libjsonwriter` is able to take advantage of ADL in its functions for
outputting vectors, maps, and other containers. The reason is that we
can say something like `serialize(out, myvec[0])` and no matter what
the value type is of `myvec`, the compiler will look in that type's
namespace for that `serialize` definition.


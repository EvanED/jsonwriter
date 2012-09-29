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

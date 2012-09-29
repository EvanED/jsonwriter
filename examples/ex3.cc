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
        address.lines.push_back("1007 Mountain Drive");
        address.city = "Gotham";
        address.state = "NY";
        address.postal_code = "12345";

        Person batman;
        batman.name = "Batman";
        batman.address = address;
        batman.age = 30;

        serialize(std::cout, batman);
	std::cout << "\n";
    }

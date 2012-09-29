    #include <iostream>
    #include <vector>
    #include <jsonwriter/writer.hh>

    int main() {
        std::vector<int> v = {1, 4, 9, 16};
        jsonwriter::serialize(std::cout, v);
	std::cout << "\n";
    }

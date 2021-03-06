#ifndef WIN32

#include <iostream>

#include "catch.hpp"

#include "chemfiles.hpp"
#include "chemfiles/files/BasicFile.hpp"
using namespace chemfiles;

#define FILESDIR SRCDIR "/data/"

TEST_CASE("Read a text file", "[Files]"){
    BasicFile file(FILESDIR "xyz/helium.xyz", File::READ);
    REQUIRE(file.is_open());

    CHECK(file.nlines() == 50419);

    std::string line = file.getline();
    CHECK(line == "125");

    auto lines = file.readlines(42);
    REQUIRE(lines.size() == 42);
    CHECK(lines[0] == "Helium as a Lennard-Jone fluid");
    CHECK(lines[1] == "He 0.49053 8.41351 0.0777257");

    // Geting line count after some operations
    CHECK(file.nlines() == 50419);

    file.rewind();
    line = file.getline();
    CHECK(line == "125");

    // State is preserved by the nlines function
    file.nlines();
    line = file.getline();
    CHECK(line == "Helium as a Lennard-Jone fluid");

    // Check stream version
    file.rewind();
    file >> line;
    CHECK(line == "125");
}

TEST_CASE("Write a text file", "[Files]"){
    BasicFile file("tmp.dat", File::WRITE);
    REQUIRE(file.is_open());

    file << "Test" << "\n";

    file.writeline("Test again\n");

    std::vector<std::string> lines;
    lines.push_back("Hello\n");
    lines.push_back("world\n");
    file.writelines(lines);

    file.sync();

    std::ifstream verification("tmp.dat");
    REQUIRE(verification.is_open());

    std::string line;
    std::getline(verification, line);
    CHECK(line == "Test");

    std::getline(verification, line);
    CHECK(line == "Test again");

    std::getline(verification, line);
    CHECK(line == "Hello");
    std::getline(verification, line);
    CHECK(line == "world");

    verification.close();

    remove("tmp.dat");
}

TEST_CASE("Errors in text files", "[Files]"){
    // TODO
}

#endif

#include <streambuf>
#include <fstream>
#include <cstdio>
#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

#define XYZDIR SRCDIR "/data/xyz/"

// This file only perform basic testing of the trajectory class. All the differents
// formats are tested in the formats folder
TEST_CASE("Associate a topology and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        Trajectory file(XYZDIR "trajectory.xyz");

        SECTION("From a file"){
            file.set_topology(XYZDIR "topology.xyz.topology", "XYZ");
            auto frame = file.read();

            CHECK(frame.natoms() == 9);
            auto topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Zn"));
            CHECK(topology[1] == Atom("Cs"));
            CHECK(topology[2] == Atom("Ar"));
        }

        SECTION("Directely"){
            Topology top;
            for (size_t i=0; i<9; i++)
                top.append(Atom("Fe"));

            file.set_topology(top);
            auto frame = file.read();

            CHECK(frame.natoms() == 9);
            auto topology = frame.topology();
            CHECK(topology.natoms() == 9);
            CHECK(topology[0] == Atom("Fe"));
            CHECK(topology[1] == Atom("Fe"));
            CHECK(topology[8] == Atom("Fe"));
        }
    }

    SECTION("Writing"){
        const auto EXPECTED_CONTENT =
        "5\n"
        "Written by the chemfiles library\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n"
        "Fe 1 2 3\n";

        Trajectory file("tmp.xyz", 'w');
        Frame frame(5);
        auto positions = frame.positions();
        for(size_t i=0; i<5; i++) {
            positions[i] = vector3d(1, 2, 3);
        }

        Topology top;
        for (size_t i=0; i<5; i++)
            top.append(Atom("Fe"));

        file.set_topology(top);
        file.write(frame);
        file.sync();

        std::ifstream checking("tmp.xyz");
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        checking.close();

        CHECK(content == EXPECTED_CONTENT);
        remove("tmp.xyz");
    }
}

TEST_CASE("Associate an unit cell and a trajectory", "[Trajectory]"){
    SECTION("Reading"){
        Trajectory file(XYZDIR "trajectory.xyz");
        file.set_cell(UnitCell(25, 32, 94));
        auto frame = file.read();

        CHECK(frame.cell() == UnitCell(25, 32, 94));
    }

    SECTION("Writing"){
        const auto EXPECTED_CONTENT =
        "CRYST1    3.000    4.000    5.000  90.00  90.00  90.00 P 1           1\n"
        "HETATM    0        X   0    1.000   2.000   3.000  0.00  0.00  \n"
        "HETATM    1        X   1    1.000   2.000   3.000  0.00  0.00  \n"
        "HETATM    2        X   2    1.000   2.000   3.000  0.00  0.00  \n"
        "END\n";

        Trajectory file("tmp.pdb", 'w');
        Frame frame(3);
        auto positions = frame.positions();
        for(size_t i=0; i<3; i++) {
            positions[i] = vector3d(1, 2, 3);
        }
        file.set_cell(UnitCell(3, 4, 5));
        file.write(frame);
        file.sync();

        std::ifstream checking("tmp.pdb");
        std::string content{
            std::istreambuf_iterator<char>(checking),
            std::istreambuf_iterator<char>()
        };
        checking.close();

        CHECK(content == EXPECTED_CONTENT);
        remove("tmp.pdb");
    }
}

TEST_CASE("Specify a format parameter", "[Trajectory]"){
    Trajectory file(XYZDIR "helium.xyz.but.not.really", 'r', "XYZ");
    auto frame = file.read();
    CHECK(frame.natoms() == 125);
}

TEST_CASE("Errors", "[Trajectory]"){
    SECTION("Unknow opening mode") {
        CHECK_THROWS_AS(Trajectory("trajectory.xyz", 'z'), FileError);
    }

    SECTION("Bad opening mode") {
        // Try to read a write-only file
        Trajectory file("tmp.xyz", 'w');
        CHECK_THROWS_AS(file.read(), FileError);
        remove("tmp.pdb");

        // Try to write a read-only file
        file = Trajectory(XYZDIR "trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.write(Frame()), FileError);
    }

    SECTION("Read file past end") {
        Trajectory file(XYZDIR "trajectory.xyz", 'r');
        CHECK_THROWS_AS(file.read_step(5), FileError);

        file.read();
        file.read();
        CHECK_THROWS_AS(file.read(), FileError);
    }
}

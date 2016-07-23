External dependencies
-----------------
* googletest
* boost (filesystem)

googletest is provided as a git submoudle.
install by initiating the git submodule.
`git submodule update --init`

Boost is required to be installed seperatly

1. Download latest version from [boost.org](Http://www.boost.org/)
2. Uncompress with `tar -xzvf`
3. Compile boost by executing boostrap.sh and following the instructions
4. In the root `db` CMakeLists.txt configure the `BOOST_ROOT`property by setting it to the location of the uncompressed boost library.

Build
----- 
`mkdir build
cd build
cmake ..
make
./main/db`

Compiled with
------------
g++ 5.3.1

requires c++14 support

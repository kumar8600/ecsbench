# ecsbench

Benchmarks comparing Entity-component-systems frameworks for C++

## Dependencies

* [Git](https://git-scm.com/)
* [CMake](http://www.cmake.org/)
* [Boost C++ Libraries](http://www.boost.org/)
* [google/benchmark](https://github.com/google/benchmark) (downloaded by CMake)
* [alecthomas/entityx](https://github.com/alecthomas/entityx) (downloaded by CMake)
* [miguelmartin75/anax](https://github.com/miguelmartin75/anax) (downloaded by CMake)
* [Catchouli/coment](https://github.com/Catchouli/coment) (downloaded by CMake)

## Build and Run benchmarks

No support for Visual Studio now (because google/benchmark does not support Visual Studio).

```bash
git clone https://github.com/kumar8600/ecsbench.git
cd ecsbench
mkdir build
cd build
cmake ..
make
./ecsbench
```


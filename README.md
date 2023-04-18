 WIP

---

# GCC Build Tracer

Collect time traces of various GCC internal events during a compilation and save to Trace Event Format.

## Build

```
cmake -B build -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_C_COMPILER=gcc-12 -D CMAKE_CXX_COMPILER=g++-12
ninja -C build
sudo ninja -C build install
```

## Use

```
g++-12 -fplugin=tracer foo.cpp
```

Traces are saved to `/tmp/gcc-trace.json`.

## License

GPLv3

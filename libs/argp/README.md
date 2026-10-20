# argp

Bundled [argp-standalone](https://github.com/tom42/argp-standalone) command-line parser, distributed under the [LGPL-2.1-or-later license](LICENSES/LGPL-2.1-or-later.txt)

The same bundled parser is used on Linux, macOS, and Windows/MSYS2. A system argp package and CMake are not required

Commands run from `libs/argp`

Update the sources, configuration template, and licenses using Git:

```sh
make update
```

Build:

```sh
make debug
```

Libraries are created at `.builds/debug/libs/libargp.a` and `.builds/debug/libs/libargp.so`. The public header is `src/argp.h`

Generate only `.builds/debug/libs/obj/argp/config.h`, without building the library:

```sh
make .builds/debug/libs/obj/argp/config.h
```

Before compilation, `configure.c` generates `config.h` from the upstream `config.h.in`. Small compiler probes detect available headers, functions, variables, and types. Probe programs are linked but never executed. The generated header and compiler diagnostics are kept in the build mode's `libs/obj/argp` directory

`CC`, `CPPFLAGS`, `CFLAGS`, and `LDFLAGS` select the target toolchain. `CC_FOR_BUILD` and `CFLAGS_FOR_BUILD` select the native compiler and flags for the generator. Changed target flags trigger configuration again. On macOS, function checks respect the deployment target supplied through compiler flags or `MACOSX_DEPLOYMENT_TARGET`

The bundled sources retain their upstream formatting. `make format` formats only the local generator

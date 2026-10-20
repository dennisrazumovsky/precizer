# inih

Bundled [inih](https://github.com/benhoyt/inih) INI parser in C, distributed under the [BSD-3-Clause license](LICENSE.txt)

Commands run from `libs/inih`

Update sources and license from upstream `master` using Git:

```sh
make update
```

Build:

```sh
make debug
```

Libraries are created at `.builds/debug/libs/libinih.a` and `.builds/debug/libs/libinih.so`. The public header is `src/ini.h`

The library and precizer use the same parser settings from [`src/inih_config.h`](../../src/inih_config.h) in the application source directory. This header is required to build the library

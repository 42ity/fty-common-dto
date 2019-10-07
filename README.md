# fty-common-dto
fty-common-dto:

* Centralize all objects between agents. 
* Object centralization used by fty-common-message bus.

## How to build

To build fty-common-dto project run:

```bash
./autogen.sh
./configure
make
make check # to run self-test
```
## How to use the dependency in your project

In the project.xml, add following lines:

```bash
<use project = "fty-common-dto" libname = "libfty_common_dto" header = "fty_common_dto.h"
        repository = "https://github.com/42ity/fty-common-dto.git"
        release = "master"
        test = "fty_common_dto_selftest" />
```

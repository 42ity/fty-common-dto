# fty-common-dto
fty-common-dto:

* Centralize the most common use and exchanged between agents. 
* Object centralization used which can be used by fty-common-message bus.

## How to build

To build fty-common-dto project run:

```bash
./autogen.sh
./configure
make
make check # to run self-test
```
## How to use the dependency in your project

* If you want to include only some specific headers:

In the project.xml, add following lines:

```bash
<use project = "fty-common-dto" libname = "libfty_common_dto" header = "fty_common_dto_empty.h"
        repository = "https://github.com/42ity/fty-common-dto.git" />

```
If you use any protbuf generated file you need also to add protobuf:

```bash
<use project = "protobuf" header = "google/protobuf/stubs/common.h"
        repository = "https://github.com/42ity/protobuf.git"/>
```

* If you want to include all headers:

In the project.xml, add following lines:

```bash
<use project = "fty-common-dto" libname = "libfty_common_dto" header = "fty_common_dto.h"
        repository = "https://github.com/42ity/fty-common-dto.git" />

<use project = "protobuf" header = "google/protobuf/stubs/common.h"
        repository = "https://github.com/42ity/protobuf.git"/>
```

Regenerate protobuf files with protoc (3.11.0):

```bash

protoc --cpp_out=. src/srr.proto 
files src/srr.pb.cc and src/srr.pb.cc are generate, move them like mv src/srr.pb.cc src/srr_pb.cc mv src/srr.pb.h include/srr_pb.h
Replace in src/srr_pb.cc the header #include "src/srr.pb.h" by #include "srr_pb.h"

```

protoc --cpp_out=. src/srr.proto

## Objects offered
- srr_pb objects and fty_srr_dto -> DTO for srr including serialisation for UI, comparaison and SrrRequestProcessor to help the client to implement SRR.
- message_bus_config_pb -> Configuration for message bus.
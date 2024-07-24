# fty-common-dto
fty-common-dto:

* Centralize the most common use and exchanged between agents.
* Object centralization used which can be used by fty-common-message bus.

## How to build

To build fty-common-dto project and run tests:

```bash
cmake -B build -DBUILDTESTING=On
cd build/
make
ctest
```

## Regenerate protobuf files with protoc (>= 3.11.0):

```bash
protoc --cpp_out=. src/srr.proto
files src/srr.pb.cc and src/srr.pb.cc are generate, move them like mv src/srr.pb.cc src/srr_pb.cc mv src/srr.pb.h include/srr_pb.h
Replace in src/srr_pb.cc the header #include "src/srr.pb.h" by #include "srr_pb.h"

```

## Objects offered

- srr_pb objects and fty_srr_dto -> DTO for srr including serialisation for UI, comparaison and SrrRequestProcessor to help the client to implement SRR.
- message_bus_config_pb -> Configuration for message bus.

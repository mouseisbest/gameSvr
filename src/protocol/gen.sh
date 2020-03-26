#!/bin/bash
protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./def.proto
protoc -I ./ --cpp_out=. ./def.proto
protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./res.proto
protoc -I ./ --cpp_out=. ./def.proto
protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./cs.proto
protoc -I ./ --cpp_out=. ./cs.proto

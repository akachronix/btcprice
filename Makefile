source_files := $(shell find src/ -name *.cpp)
object_files := $(patsubst src/%.cpp, build/%.o, $(source_files))
target := build/btcprice

compiler_flags := -std=c++11 -Wall -pedantic -O2 -DCURL_STATICLIB -Ijsoncpp/include -Ljsoncpp/build/debug/lib

.PHONY: build clean jsoncpp
build: $(target) $(object_files)

clean:
	rm -rf build/

jsoncpp:
	cd jsoncpp/ && \
	mkdir -p build/debug && \
	cd build/debug && \
	cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../.. && \
	make

$(target): $(object_files) jsoncpp
	g++ $(compiler_flags) -o $(target) $(object_files) -ljsoncpp -lcurl

$(object_files): build/%.o : src/%.cpp jsoncpp
	mkdir -p $(dir $@) && \
	g++ $(compiler_flags) -c $(patsubst build/%.o, src/%.cpp, $@) && \
	mv *.o build/
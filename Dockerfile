FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y git cmake clang llvm zlib1g-dev

COPY . /app

RUN git submodule update --init --recursive
RUN cmake -S . -B build
RUN cmake --build build -j 12
RUN ./build/hydro ./input.hy ./out
RUN ctest --test-dir ./build --output-on-failure

CMD ["./out"]
FROM ubuntu:24.04 AS builder
RUN apt-get update && apt-get install -y g++ cmake ninja-build && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_HTTP_SERVER=ON \
 && cmake --build build --parallel \
 && ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime
COPY --from=builder /src/build/taskflow_server /usr/local/bin/taskflow_server
ENV PORT=8080
EXPOSE 8080
CMD ["taskflow_server"]

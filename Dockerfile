FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ cmake make git pkg-config \
    libfreetype-dev libharfbuzz-dev libudev-dev \
    libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --branch 3.1.0 --depth 1 https://github.com/SFML/SFML.git /sfml

RUN cmake -S /sfml -B /sfml/build \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DBUILD_SHARED_LIBS=ON \
      -DSFML_BUILD_AUDIO=OFF \
      -DSFML_BUILD_NETWORK=OFF \
    && cmake --build /sfml/build -j$(nproc) \
    && cmake --install /sfml/build \
    && ldconfig

WORKDIR /app
COPY . .

RUN g++ -std=c++17 orbitSim.cpp orbitHandler.cpp -o orbit \
    -L/usr/local/lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -Wl,-rpath,/usr/local/lib

CMD ["./orbit"]
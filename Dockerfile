FROM alpine:3.21.2 AS builder

# Установка необходимых пакетов
RUN apk add --no-cache \
    g++ \
    cmake \
    make \
    git \
    wget \
    build-base \
    ninja \
    zip \
    unzip \
    curl \
    pkgconf \
    libc6-compat

# Установка vcpkg и nlohmann/json
WORKDIR /opt
RUN git clone https://github.com/microsoft/vcpkg.git && \
    cd vcpkg && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg install nlohmann-json

# Установка рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY . /app

# Компиляция
RUN g++ -o bank bank.cpp -I/opt/vcpkg/installed/x64-linux/include

# Минимальный образ для запуска
FROM alpine:3.21.2

# Установка зависимостей для выполнения бинарного файла
RUN apk add --no-cache libstdc++ bash

# Копирование собранного бинарного файла из этапа сборки
COPY --from=builder /app/bank /app/bank

# Установка рабочей директории
WORKDIR /app

# Команда по умолчанию
CMD ["/app/bank"]

# Используем версию убунту 14.04
FROM ubuntu:14.04

WORKDIR /pa1

# Обновляем список пакетов и устанавливаем clang3.5
RUN apt-get update && \
    apt-get install -y clang-3.5 \
    -y make \
    -y gcc

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.5 100

COPY . .

CMD ["bash"]
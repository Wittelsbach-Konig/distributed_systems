# PA 1

## Makefile inctructions 
* `make` - compile & link
* `make clean` - rm object files, logs and executable files

## Run instruction

1. build docker image
    ```bash
    docker build -t pa1 .
    ```
2. run docker-container in interactive mode
    ```bash
    docker run -it --name pa1-container pa1
    ```
3. inside container
    ```bash
    ./pa1
    ```
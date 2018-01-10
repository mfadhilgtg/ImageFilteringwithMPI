# Image Filtering with MPI

My class assignment in parallel computing course. The task was to compare computational time of image filtering between serial and parallel

## Getting Started

### Prerequisites

Install gcc and MPI first.

For ubuntu,
```
$ sudo apt-get install libcr-dev mpich2 mpich2-doc
```
### Input Image
The image must be in .pgm and square(You can modify for non-square image).
The default image name opened is original.pgm

### Compiling
In the terminal:
  ```bash
  $ mpicc Paralel_ImagePro.c -o Paralel_ImagePro
  $ mpirun -np 4 ./Paralel_ImagePro
  ```
  -np X for X different processes

## Authors

* **Muhammad Fadhil Ginting** - [mfadhilgtg](https://github.com/mfadhilgtg)




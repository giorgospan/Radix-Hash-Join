# Software Development for Database Systems

## About

The continuous advance of technology used in the hardware domain has lead to the mass production of multi-core CPUs as well as to the decrease of RAM cost in terms of $/GB. In this project we demonstrate an efficient implementation of join operation in relational databases leveraging CPU parallelism and the large amount of available RAM in servers nowadays.

Our project, written in C language, was constructed in three parts which were joined together in a single one in such a way, so that it complies with the instructions we were given. It features a hash-based radix partition join inspired by the join algorithms introduced in this [paper](https://15721.courses.cs.cmu.edu/spring2016/papers/balkesen-icde2013.pdf).

Additionally, it is worth mentioning that the whole project is based on the [SIGMOD Programming Contest 2018](http://sigmod18contest.db.in.tum.de/task.shtml). Thus, we follow the task specifications of the contest and we also utilize the testing workloads provided to the contestants.


## Implementation

* #### Radix Hash Join

  The main idea of Radix Hash Join algorithm is to partition the input data of the two join relations in a number of buckets, so that the largest bucket can fit into the CPU cache. More precisely, the RHS algorithm consists of the following three phases:

   * **Partition**

     We partition the data of each relation in a number of buckets using the same hash function (HASH_FUN_1) for both relations. In our implementation HASH_FUN_1 uses that n least-significant bits of the record to determine its bucket. Moreover, histogram and prefix sum tables need to be calculated for each one of the two relations.

   * **Build**

     An index is created for each of the partitions (i.e: buckets) of the smallest relation. Each index resembles a hash table using two arrays (chain array and bucket array). Those arrays are used to store indices of the corresponding bucket according to the hash value of a new hash function (HASH_FUN_2).

  * **Probe**

    Partitions of the non-indexed, i.e: big, relation are scanned and the respective index is probed for matching tuples.

    ![image not found](./img/radix_hash_join.png)

    *Image above illustrates the three phases of Radix Hash Join Algorithm*


* #### Multithreading

  We managed to speed up our serial implementation by applying multithreading  on various parts of our code, such as filter execution, histogram creation, indexing on the buckets, probing e.t.c. For this purpose, POSIX Threads were used.

  ![image not found](./img/plot1.png)

  The above graph shows the correlation between execution time and number of threads. We used the *public* dataset which can be downloaded from [here](http://sigmod18contest.db.in.tum.de/public.tar.gz).

  Our machine's specifications are:
   * CPU: Ryzen 2400G 3.6 GHz , 4 cores , 8 threads
   * RAM: 16GB DDR4 dual-channel

## Usage

  * ``cd final``
  * ``./compile.sh && ./runTestHarness.sh``

## Unit Testing

  For unit testing we use the [CUnit](http://cunit.sourceforge.net/index.html) testing framework. Tests are added to different suites, each one being responsible for testing a specific category of functions. In order to run the tests CUnit must be [installed](http://archive15.fossology.org/projects/fossology/wiki/Installing_CUnit) on your system.  

#### Running the tests
  * ``cd final``
  * ``make unittest && ./runUnitTesting.sh``

## Memory checking

You may run a memory check using valgrind by uncommenting the line you want in *run.sh* script.


## Authors

  * Panagiotopoulos Georgios <sdi1400136@di.uoa.gr>
  * Papastamou Ioannis <sdi1400252@di.uoa.gr>

## References

  * Cagri Balkesen, Jens Teubner, Gustavo Alonso, and M. Tamer Özsu
  [Main-Memory Hash Joins on Multi-Core CPUs: Tuning to the Underlying Hardware](https://15721.courses.cs.cmu.edu/spring2016/papers/balkesen-icde2013.pdf)

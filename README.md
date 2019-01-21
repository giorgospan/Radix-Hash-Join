# Software Development for Database Systems

## About

This is an implementation of a hash-based radix partition join. Multithreading is used in order to achieve parallelism.The project is based on [SIGMOD Programming Contest 2018](http://sigmod18contest.db.in.tum.de/index.shtml)


## How to run

  * Open your command line
  * Move to SecondPart folder
  * Type ``./compile.sh && ./runTestHarness.sh``
  
## Unit Testing
  
  * Open your command line
  * Move to SecondPart folder
  * Type ``make unittest && ./runUnitTesting.sh`` 
  
## Memory check

You may run a memory check using valgrind by uncommenting the line you want in *run.sh* script.
 
 
## Authors

  * Panagiotopoulos Georgios <sdi1400136@di.uoa.gr>
  
  * Papastamou Ioannis <sdi1400252@di.uoa.gr>

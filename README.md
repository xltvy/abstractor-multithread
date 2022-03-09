# abstractor-multithread

Bogazici University

Computer Engineering Department

Fall 2021

CMPE 322 - Operating Systems

Project 2

Altay Acar

***

A multithreaded scientific search engine that queries the paper abstracts and summarizes the most relevant ones according to the Jaccard similarity score.

The program takes a query as an input, scans its file collection, finds the most relevant abstracts by measuring the similarities between abstracts and the given query, then retrieves convenient sentences from the selected abstracts.

The program creates T number of threads and makes them work till the end of the program.

Each thread logs its own name and the name of the file it is processing to the output file. Threads are named alphabetically and starts from A and continues with B, C, and so on.

The jobs are evenly spread to the threads. When all the abstracts are processed, all of the threads terminate gracefully. To address the synchronization issues, mutex locks and unlocks are used.

In this program Jaccard Similarity metric is used to determine the similarity between two texts. This metric finds how close the two texts are to each other in terms of the number of common words that exist over total words. An example:

1. Get two texts to compare:
Text1: the war and the dance
Text2: when the world was at war before we just kept dancing

2. Tokenize texts using space and find the unique words: WordSet1 = {the, war, and, dance}
WordSet2 = {when, the, world, was, at, war, before, we, just, kept, dancing}

3. Find the intersecting words of two sets:
Intersection WS1 WS2 = {the, war}

4. Find the union of two sets:
Union WS1 WS2 = {the, war, and, dance, when, world, was, at, before, we, just, kept, dancing}

5. Calculate the Jaccard Similarity using the mathematical formula given below:
  > J(A,B)= ∥A∩B∥ / ∥A∪B∥

Input:
- The abstractor.out program takes two command line arguments:
  > 1. input file name.txt: Path of the input file
  > 
  > 2. output file name.txt: Path of the output file

- The input file name.txt file contains several parameters, which will be read line-by-line:
  > 1. The first line contains values of T, A, and N, which are separated by spaces.
  > 
  >   • T: Number of threads (other than the main thread) that will be created for the session
  >   
  >   • A: Number of abstracts that will be scanned by the threads
  >   
  >   • N: Number of abstracts that will be returned and summarized
  >   
  >   2. The second line contains the words to query.
  >   
  >   3. From lines 3 to A+2, the names of the abstract files will be listed.

- An input file consists of several lines of sentences that are pre-processed and separated by dots. An example abstract, which consists of two sentences, all in lowercase, and separated from punctuation, is provided below:
  > the covid19 pandemic caused by sarscov2 remains a significant issue for global health eco- nomics and society . a wealth of data has been generated since its emergence in december 2019 and it is vital for clinicians to keep up with this data from across virus the world at a time of uncertainty and constantly evolving guidelines and clinical practice .

Output:
- The Abstractor outputs the thread operation logs, scores, and resulting sentences to the given text file.
- Each thread outputs its name and operation as ”Thread X is calculating abstract Y.txt”.
- ### is inserted after each separate operation, i.e., after finishing threads jobs, listing result 1, result 2, and so on.

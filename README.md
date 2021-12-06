#88 Key Guessing Strategies for Linear Key-Schedule Algorithms in Rectangle Attacks
===
Experiment of Computing Success Probability with Various Parameters
---
* `success_probability.m`: Source code for computing success probability.

Success Probability for 32-round SKINNY-128-384 with s=10

s' | Data | Memory | Time | Success Probability | Advantage h
1 | 2^{125.2} | 2^{125.2} | 2^{363.84} | 99.9% | 20.18
2 | 2^{125.2} | 2^{125.2} | 2^{357.37} | 99.7% | 31.35



Experiment of Rectangle Attack on 17-Round SKINNY-64/128
---
* `Experiment_17r_skinny_64_128/17r_attack.cpp`: Source code for the experiment using OpenMP.

Experiment of Rectangle Attack on 10-Round Simon-32/64
---
* `Rebuttal_Supplement_Simon32.pdf`: discribe an experiment on 10-round Simon-32/64.
* `10r_simon32_rectangle.cpp`: Source code for the experiment.


Source Code
---
This repository contains all the tools implemented for the automated search of boomerangs on skinny. It also contains some files to check the result claimed in the article.

The different folders are:
* `skinny-boomerang-search`: Thanks to the automated boomerang search models given by Delaune, Derbez and Wavrille, we tweak the models by adding some new constaints about the key-recovery attack. We modify the `solutiontotikz` tool to automically draw a picture of our searched attack.
* `ArticleTrails`: all the trails found on the different versions and number of rounds of Skinny.
* `newlibs`: the libraries used by the different tools. Remark that you may want to change some of the jars (in particular gurobi.jar)


There are more details about each folder in the `README.md` file of the folder.

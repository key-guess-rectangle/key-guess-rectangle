#88 Key Guessing Strategies for Linear Key-Schedule Algorithms in Rectangle Attacks
===
Experiment of Computing Success Probability with Various Parameters
---
* `success_probability.m`: Source code for computing success probability.

Success Probability for 32-round SKINNY-128-384 with s=10. s` is a threshold value, that only candidate keys with counter > s` are test in Exhaustive search. An s` will correspond to an h as shown in the following table.

s` | Data | Memory | Time | Success Probability | Advantage h
----|-----|------|-------|--------|---------
1 | 2^{125.2} | 2^{125.2} | 2^{363.84} | 99.9% | 20.18
2 | 2^{125.2} | 2^{125.2} | 2^{357.37} | 99.7% | 31.35
3 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 98.9% | 42.94
4 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 97.0% | 54.85
5 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 93.2% | 67.02
6 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 86.9% | 79.42
7 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 77.9% | 92.01
8 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 66.7% | 104.76
9 | 2^{125.2} | 2^{125.2} | 2^{357.31} | 54.2% | 117.67

On response to Review #39B
---

> All the attacks are applied in related-key setting except for a case where an attack is described against 10-round Serpent under single key setting. The authors indicate the example attack is only to prove the efficiency of the author’s model. I think it would be good if the author explains the rationale for selecting the single-key setting of the attack against a different cipher, i.e., Serpent, instead of the other ciphers discussed in the paper.

According to the estimation ``2^{-n} \approx p^2q^2’’ in Eq. (22), we have T_2 \approx 2^{m_b+2*r_f+m’_f-2h_f} in Eq.(27), and T_3=2^{k-h} > 2^{k-(m_b+m_f-x)}> 2^{k-(m_b+m_f)}. In single-key attack on10-r Serpent-128-256, we know m_b=r_b,m_f=r_f, and m’_f=h_f (i.e., guessing 4-bit key for one active Sbox, we get two 4-bit filter). Hence, T_2 \approx 2^{m_b+2*m_f-m’_f }. If we let m’_f=m_f, then T_2 \approx 2^{m_b+m_f}. Considering T_3 and T_2:
1)	When m_b+m_f < k/2, T_3 is the dominating one. Hence, in this case, the effective of the tradeoff is marginal. Unfortunately, the case happens to the 10-round attack, where mb+mf=20+76=96<128. So the improvement is marginal (only a factor of about 2^{10}).
2)	When m_b+m_f > k/2, we may choose m’_f to reduce T_2, and choose h, so that T_3\approx T_2. 
Hence, if we extend more rounds before and after the distinguisher to let m_b+m_f increase, we may have a better tradeoff. However, for Serpent, one additional round leads to significantly increase of m_b+m_f (close to the full key) due to the fast diffusion of Serpent’s round function as well as the boomerang distinguisher with many active input/output Sboxes. Hence, we fail to find a 11-round attack. Future work is find better distinguishers, which are friendly to our tradeoff model. 



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

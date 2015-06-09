# random-prediction
Prediction of the outputs of poor random generators.
In the hopes that this might save someone's time while learning on the subject.
I tried to comment the parts that were not completely explicit for me from the links.
I did not bother to write classes as this was just a quick POC.

###Sample output
```
Predicting output for glibc's TYPE_1 random (default):
Success rate: 7493 / 10000 (0.7493)

Predicting output for glibc's TYPE_1 random (default) & 0x0F:
Success rate: 7504 / 10000 (0.7504)

Predicting output for linear congruential generator:
Success rate: 10000 / 10000 (1)
```

###How to compile
```
g++ predict.cpp --std=c++11 -o predict
```


## What this predicts
- glibc's random() TYPE_1 (additive feedback generator)
  - [info about the maths behind](http://www.mathstat.dal.ca/~selinger/random/)
  - [info about the probability bias](http://stackoverflow.com/a/14679656/395386)
  - [info about glibc's random TYPEs](http://stackoverflow.com/a/25819262/395386)

- glibc's random() TYPE_1 (additive feedback generator) last 4 least significant bits
  - higher bits do not influence the future lower bits, so nothing new here

- linear congruential generator
  - [info about the maths behind](http://www.pcg-random.org/predictability.html)


## Ideas
- would be interesting to take a look at predicting Mersenne Twister output
  - [looks like interesting info](https://jazzy.id.au/2010/09/22/cracking_random_number_generators_part_3.html)
  - feel free to make a PR if you are interested

- predict the output of a LCG that only outputs high bits

- see if glibc's TYPE_1 random can be predicted with 100% accuracy (finding the seed/the states exact values?)

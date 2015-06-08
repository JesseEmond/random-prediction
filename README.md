# random-prediction
Prediction of the outputs of poor random generators.
In the hopes that this might save someone's time while learning on the subject.
I tried to comment the parts that were not completely explicit for me from the links.
I did not bother to write classes as this was just a quick POC.

Predicts:
- glibc's random() TYPE_1 (additive feedback generator)
  - [info about the maths behind](http://www.mathstat.dal.ca/~selinger/random/)
  - [info about the probability bias](http://stackoverflow.com/a/14679656/395386)
  - [info about glibc's random TYPEs](http://stackoverflow.com/a/25819262/395386)

- glibc's random() TYPE_1 (additive feedback generator) last 4 least significant bits
  - higher bits do not influence the future lower bits, so nothing new here

- linear congruential generator
  - [info about the maths behind](http://www.pcg-random.org/predictability.html)


Todo:
- would be interesting to take a look at predicting Mersenne Twister output
  - [looks like interesting info](https://jazzy.id.au/2010/09/22/cracking_random_number_generators_part_3.html)
  - feel free to make a PR if you are interested

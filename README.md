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

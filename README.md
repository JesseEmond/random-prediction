# random-prediction
Prediction of the outputs of poor random generators.

Predicts:
- glibc's random() TYPE_1 (additive feedback generator)
  - [info about the maths behind](http://www.mathstat.dal.ca/~selinger/random/)
  - [info about the probability bias](http://stackoverflow.com/a/14679656/395386)
  - [info about glibc's random TYPEs](http://stackoverflow.com/a/25819262/395386)

- glibc's random() TYPE_1 (additive feedback generator) last 4 least significant bits
  - higher bits do not influence the future lower bits, so nothing new here

#!/bin/sh 
read -p  'Which partition do you want to switch to?': partvar                                                                                                                     
for i in $(squeue -u zbaldwin -h -t PD -o %i)
do
scontrol update jobid=$i partition=$partvar
done

#!/bin/sh

#  ===========================================================================
# |                                                                           |
# |             COMMAND FILE FOR SUBMITTING SGE JOBS                          |
# |                                                                           |
# |                                                                           |
# | SGE keyword statements begin with #$                                      |
# |                                                                           |
# | Comments begin with #                                                     |
# | Any line whose first non-blank character is a pound sign (#)              |
# | and is not a SGE keyword statement is regarded as a comment.              |
#  ===========================================================================

# Request Bourne shell as shell for job
#$ -S /bin/sh

# Execute the job from the current working directory.
#$ -cwd

# Defines  or  redefines  the  path used for the standard error stream of the job.
#$ -e ./results

# The path used for the standard output stream of the job.
#$ -o ./results

# Do not change.
#$ -pe ompi 1

# Do not change.
#$ -q 4c_serial.q

rm -f *.gcda
make clean
make OPTIMAL=1 PROF_GEN=1
make test
#make performance_test FROM=1000 TO=1007
make performance_test FROM=2000 TO=2003
#make performance_test FROM=3000 TO=3001
#make performance_test FROM=4000 TO=4000
make clean
make OPTIMAL=1

# stats
This is a simple bash program which tells you the average and median of each column or row in a file full of numbers that looks similar to test_file.

#From terminal in a linux machine:
First make stats executable with

`chmod+x stats`

To get a list of the average of each row in test_file:

`stats -r test_file`

To get a list of the average of each column in test_file:

`stats -c test_file`

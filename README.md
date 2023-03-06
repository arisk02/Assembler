# Assembler
This assembler takes in MIPS assembly code and translates it into binary sorted in 2 output files - instructions binary file and static memory binary file.

readbytes.cpp is a program that prints all the binary in the output files into terminal, alongside their hexadecimal and decimal equivalent. This was used for debugging.

The program takes in at least 3 command line arguments. The last argument is the name of the instructions binary output file, second to last is the name of the static
memory binary output file, and all previous arguments are MIPS assembly files to be translated.

This is a part of the final project for Computer Organization course at the University of Richmond, taught by Dr. Prateek Bhakta. 
Code was authored by Aris Karamustafic and Joseph Lee.

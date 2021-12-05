## jmerge -- Merge data from Two Text Files

jmerge will merge data from two Delimited Text Files creating one
Merged File.  The merge will occur based upon the first column of each
File.  Each record in Output will be a concatenation of the
corresponding data from both Files based upon Column 1.

This is a hack and rather difficult to use, some day I hope to
rewrite it to make it simpler.  But for now I put up with the
aggravation with using it.

To build, execute build.sh to generate a Makefile from
Makefile.template on most BSD/Linux systems and IBM AIX.
For MS-DOS, it should be rather easy to create a Makefile.

This requires [j_lib2](https://github.com/jmcunx/j_lib2) to build.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, so I came up with my own method which
is a real hack.  But works for me.

## jmerge -- Merge data from Two Text Files

jmerge will merge data from two Delimited Text Files creating one
Merged File.
The merge will occur based upon the first column of each File.
Each record in Output will be a concatenation of the
corresponding data from both Files based upon Column 1.

This is a hack and rather difficult to use, some day I hope to
rewrite it to make it simpler.
But for now I put up with the aggravation with using it.

This requires [j\_lib2](https://github.com/jmcunx/j_lib2) to build.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, but this seems to be good enough for me.

**To compile:**
* Set "DESTDIR" for where you want this to install.  Examples:
  * setenv DESTDIR /usr/local
  * export DESTDIR=/usr/local
* this assumes [j\_lib2](https://github.com/jmcunx/j_lib2)
  is also installed under "DESTDIR".
* Edit Makefile, uncomment areas associated to the OS
  you want to compile on.
  OS Sections:
  * Linux 64 bit (default)
  * Linux 32 bit
  * BSD 64 bit
  * BSD 32 bit
  * AIX

_To uninstall_, execute
"make uninstall"
from the source directory

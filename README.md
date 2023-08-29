## jmerge -- Merge data from Two Text Files

**Note:** Starting October 12 2023, GitHub is forcing me to use
my Cell Phone for 2FA to allow access to my repositories.  I do
not want Microsoft to have access to my phone.  So updates to
this project will no longer occur on GitHub.  I will make a note
of where source can be retrieved from on my Gemini Capsule once
everything is moved off of GitHub.
Capsule: gemini://gem.sdf.org/jmccue/ or mirror gemini://sdf.org/jmccue/

If 
[join(1)](https://man.netbsd.org/join.1)
is available on your system, you should use
[join(1)](https://man.netbsd.org/join.1)
instead of jmerge.

jmerge will Merge Data Records from Two or more Delimited
Text Files creating One Merged File.

The merge will occur based upon the First Column of each File.
Each record on Output will be a concatenation of the
corresponding data from both Files based upon Column 1
with a tag indicating if a match was found.

For example if you have the Two Files shown below.
This command will produce the results as shown:

* jmerge -d '|' -i key.txt -K stat.txt dat.txt

The Main (key) File (key.txt).  Records unique by Column 1:

| Col 1 | Col 2 |
| :--- | :--- |
| 001| A |
| 002| B |
| 003| C |
| 005| E |

The Data File (dat.txt), Multiple Records per Column 1:

| Col 1 | Col 2 | Col 3 |
| :--- | :--- | :--- |
| 001 | AA | B12 |
| 001 | AB | B345 |
| 003 | CA | B5 |
| 003 | CB | B67 |
| 004 | DA | B67 |
| 005 | EA | B89 |
| 005 | EB | B100 |
| 005 | EC | B104 |

This is the Output File Created, notice the Tag:

| Col 1 with Tag | Col 2 | Col 3 | Col 4 |
| :--- | :--- | :--- | :--- |
| MATCHED:  001 | A | 001 | AA | B12 |
| MATCHED:  001 | A | 001 | AB | B345 |
| MATCHED:  003 | C | 003 | CA | B5 |
| MATCHED:  003 | C | 003 | CB | B67 |
| NO MATCH: 004 | DA | B67 | | |
| MATCHED:  005 | E | 005|EA | B89
| MATCHED:  005 | E | 005|EB | B100
| MATCHED:  005 | E | 005|EC | B104

And this is the Output Match Stats File you can
request to be created:

| Count of Matches | Key Value |
| :--- | :--- |
| 2 | 001 |
| 0 | 002 |
| 2 | 003 |
| 3 | 005 |

## Notes:

[j\_lib2](https://github.com/jmcunx/j_lib2) is an **optional** dependency.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, but this seems to be good enough for me.

**To compile:**
* If "DESTDIR" is not set, will install under /usr/local
* Execute ./build.sh to create a Makefile
* Works on Linux, BSD and AIX

_To uninstall_, execute
"make uninstall"
from the source directory

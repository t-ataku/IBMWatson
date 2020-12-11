# IBMWatson
Several study of IBM Watson
------------------------------------------
# NAME Interpret.sh

# SYNOPSIS
Interpret.sh [options]

# DESCRIPTION
Interprete written document from some language to another.
You need create WATSON "Language Translator" service, re-write [URLBASE] environment value in the script and keep [credential] of api-key in [pwd] file before using it.

It needs several options in execution.
-i: Language for input file
-o: Language you need to output
-f: Output filename
-r: Input(Read) filename
-l: List supported language.  Use "language" field in it for -i or -o.

# EXAMPLE
$ Interpret.sh -i en -o ja -r bcg_en.txt -f bcg_ja.json
------------------------------------------
# NAME Speak.sh

# SYNOPSIS
Speak.sh [-v]|[-l lang -o outputfile]

# DESCRIPTION
Read JSON document from "Language Translator" and synthesis it to audio file.
You need create WATSON "Text to Speach" service, re-write [URLBASE] environment value in the script and keep [credential] of api-key in [sppwd] file before using it.

Options are
-v: List languages and voice information supported
-l: Language to select.  It must be one of "en", "ja", "ko" and "zh"
-o: Audio filename to output

# EXAMPLE

$ Speak.sh -v
$ cat bgc_ja.json | Speak.sh -l ja -o bcg_ja.wav 

------------------------------------------
# NAME  wavinfo.c
WATSON creates WAV file with undefined data size (0xffffffff) in the RIFF (WAV) file.
Apple Quick Time player doesn't accept such file so that you must define those fields before playing.
You need C Compiler which is in Xcode to use it.

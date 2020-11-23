#!/bin/sh -x

# -i : 

URLBASE='https://api.eu-gb.language-translator.watson.cloud.ibm.com/instances/4a0c71d6-2c45-4435-966c-f55fc040522a'
URLLIST='v3/languages?version=2018-05-01'
URLDO='v3/translate?version=2018-05-01'

function getlanguages {
    curl -u "apikey:$(cat pwd)" -X GET $URLBASE/$URLLIST | less
}

while getopts "i:o:f:r:l" opt $@; do
    echo "\$opt=" $opt; echo "OPTARG=" $OPTARG
    echo "OPTIND=" $OPTIND
    case $opt in
	"i")
	    inlang=$OPTARG;
	    ;;
	"o")
	    outlang=$OPTARG;
	    ;;
	"f")
	    ofile=$OPTARG;
	    ;;
	"r")
	    ifile=$OPTARG;
	    ;;
	"l")
	    getlanguages;
	    exit 0;
	    ;;
	"d")
	    dbg=1;
	    ;;
	*)
	    echo "Undefined option was specified: [$OPTIND]" >&2
	    ;;
    esac
done
echo "OPTIND=" $OPTIND

if [ ! -r $ifile ]; then
    echo File cannot read: $ifile >&2
    exit 1
fi

cat > $$.txt <<EOF
{
  "text": [
EOF
awk '/["]/{ print "SYMBOL never be used" > "/dev/stderr"} $0 !~ /^[ 	]*$/ { printf("\"%s\",\n", $0);}' $ifile >> $$.txt
cat >> $$.txt <<EOF
""
], 
  "model_id":"$inlang-$outlang"
}
EOF

curl -X POST -o $ofile -u "apikey:$(cat pwd)" --header "Content-Type: application/json" --data @$$.txt $URLBASE/$URLDO

if [ ! "$dbg" ]; then
    /bin/rm -f $$.txt
fi

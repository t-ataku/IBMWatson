#!/bin/sh
#
# -l(ang) en-US : en-US_LisaVoice
#         ja-JP : ja-JP_EmiVoice
# -o(utput) filename.ext
# JSON fmt
# {
#   "Voice": $VOICENAME
#   "text": $SPEECHTEXT
# }

URLBASE=http://localhost:8443/
URLBASE=https://api.eu-gb.text-to-speech.watson.cloud.ibm.com/instances/ac07d247-9ff6-4714-a718-ca1d650c9aed
URLDO=v1/synthesize
#https://gateway-lon.watsonplatform.net/text-to-speech/api/v1/synthesize
URLLIST=v1/voices

function getvoicename {
    case $1 in
	en)
		echo "en-US_LisaVoice";
		;;
	ja)
		echo "ja-JP_EmiVoice";
		;;
	ko)
		echo "ko-KR_YoungmiVoice";
		;;
	zh)
		echo "zh-CN_LiNaVoice";
		;;
	*)
	    exit 1;;
    esac
    return 0
}

while getopts "l:o:v" opt $@; do
    echo "\$opt=" $opt; echo "OPTARG=" $OPTARG
    echo "OPTIND=" $OPTIND
    case $opt in
	"l")
	    voice=$(getvoicename $OPTARG);
	    ;;
	"o")
	    ofile=$OPTARG;
	    ;;
        "v")
            curl -u "apikey:$(cat sppwd)" ${URLBASE}/${URLLIST};
            exit 0;
            ;;
	*)
	    echo "Undefined option was specified: [$OPTIND]" >&2
	    ;;
    esac
done
echo "OPTIND=" $OPTIND

if [ -z "$voice" -o -z "$ofile" ]; then
	echo not enough parameter >&2
	exit 1
fi

#curl -v -X POST -u "apikey:xxxxxxxx" --header "Content-Type: application/json" --header "Accept: audio/wav" --data "{\"text\":\"Hello, I'm Lisa.\"}" --output $1_0.wav $URLBASE

#echo "{\"text\":\"Hello, I'm Lisa.\"}" > data1.dat
#curl -X POST -u "apikey:xxxxxxxx" --header "Content-Type: application/json" --header "Accept: audio/wav" --data @data1.dat --output $1_1.wav $URLBASE

awk '
BEGIN {
	print "{"
}

/"translation"/ {
	gsub(/^[^:]+: "/, "");
	gsub(/"[ 	]*$/, "");
	msg=msg $0
}

END {
	printf("\"text\": \"%s\"\n", msg);
	print "}"
}
' > data.dat
curl -v -X POST -u "apikey:$(cat sppwd)" --header "Content-Type: application/json" --header "Accept: audio/wav" --data @data.dat --output $ofile ${URLBASE}/${URLDO}'?'voice=$voice

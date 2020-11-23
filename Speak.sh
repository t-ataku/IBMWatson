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
URLBASE=https://gateway-lon.watsonplatform.net/text-to-speech/api/v1/synthesize

function getvoicename {
    case "$1" in
	"en-US")
	    echo "en-US_LisaVoice";;
	"ja-JP")
	    echo "ja-JP_EmiVoice";;
	*)
	    echo "en-US_LisaVoice";;
    esac
    return 0
}

while getopts "l:o:" opt $@; do
    echo "\$opt=" $opt; echo "OPTARG=" $OPTARG
    echo "OPTIND=" $OPTIND
    case $opt in
	"l")
	    voice=$(getvoicename $OPTARG);
	    ;;
	"o")
	    ofile=$OPTARG;
	    ;;
	*)
	    echo "Undefined option was specified: [$OPTIND]" >&2
	    ;;
    esac
done
echo "OPTIND=" $OPTIND


#curl -v -X POST -u "apikey:xxxxxxxx" --header "Content-Type: application/json" --header "Accept: audio/wav" --data "{\"text\":\"Hello, I'm Lisa.\"}" --output $1_0.wav $URLBASE

#echo "{\"text\":\"Hello, I'm Lisa.\"}" > data1.dat
#curl -X POST -u "apikey:xxxxxxxx" --header "Content-Type: application/json" --header "Accept: audio/wav" --data @data1.dat --output $1_1.wav $URLBASE

cat > data.dat <<EOF
{
  "Voice": "en-US_LisaVoice",
  "Text": "The following commands use the POST /v1/synthesize method to synthesize US English input to audio files in two different formats. Both requests use the default US English voice, en-US_MichaelVoice."
}
EOF
curl -v -X POST -u "apikey:xxxxxxxx" --header "Content-Type: application/json" --header "Accept: audio/wav" --data @data.dat --output $1_2.wav $URLBASE

#!/usr/bin/bash
#
# makedoc.sh
#
# Packages required: doxygen, ruby-ronn
# Files required: doxygen.conf, MAN.md

MANUAL='libconfini'
TITLE='Library Functions Manual'
AUTHOR='Stefano Gioffré'

rm -r "docs/html"
# Doxygen is not peaceful with the sequence '\"'. Replace it with '\&quot;'...
mv 'MAN.md' '__MAN__.md'
cp '__MAN__.md' 'MAN.md'
sed -i 's|\\"|\\\&quot;|g' 'MAN.md'
sed -i 's|\\"|\\\&quot;|g' 'MAN.md'
doxygen doxygen.conf
# Restore the sequence '\"'
mv '__MAN__.md' 'MAN.md'
# For ronn we need to read starting from line 4...
tail -n '+4' 'MAN.md' > "docs/${MANUAL}.md"
cd 'docs'
# Replace unrecognized markdown/doxygen sequences for ronn
sed -i 's|`#|`|g' "${MANUAL}.md"	# '`#' -> '`'
sed -i 's| -- |\x20\xE2\x80\x93\x20|g' "${MANUAL}.md"	# ' -- ' -> ' – '
sed -i 's|\&quot;|"|g' "${MANUAL}.md"	# '`#' -> '`'
# Create main man page with ronn
ronn "${MANUAL}.md" --manual="${TITLE}" --organization="${AUTHOR}"
# Codepoints to be replaced with HTML entities in ronn's HTML ouput
sed -i 's|\xC3\xA9|\&eacute;|g' "${MANUAL}.html"	# 'é'
sed -i 's|\xC3\x89|\&Eacute;|g' "${MANUAL}.html"	# 'É'
sed -i 's|\xE2\x80\x93|\&ndash;|g' "${MANUAL}.html"	# '–'
sed -i 's|\xE2\x87\x92|\&rArr;|g' "${MANUAL}.html"	# '⇒'
sed -i 's|\xC4\x80|\&Amacr;|g' "${MANUAL}.html"		# 'Ā'
sed -i 's|\xC4\x81|\&amacr;|g' "${MANUAL}.html"		# 'ā'
# Remove ronn's input
rm "${MANUAL}.md"
cd 'man'
mv 'man3' 'man_in'
mkdir 'man3'
mv 'man_in/'{'confini.h.3','IniDispatch.3','IniFormat.3','IniStatistics.3'} 'man3/'
rm -r 'man_in'
# Replace ".TH \"${MANUAL^^}\" \"\"" with ".TH \"${MANUAL^^}\" \"3\"" in ronn's man output (we want to populate man section #3, "C library function")
awk "NR==1,/\.TH \"${MANUAL^^}\" \"\"/{sub(/\.TH \"${MANUAL^^}\" \"\"/, \".TH \\\"${MANUAL^^}\\\" \\\"3\\\"\")} 1" ../"${MANUAL}" > "man3/${MANUAL}.3"
# '<code>' tags left by ronn will be replaced with '`' in man output
sed -i 's|<code>|`|g' "man3/${MANUAL}.3"
sed -i 's|</code>|`|g' "man3/${MANUAL}.3"
# Other substitutions in Doxygen's man output.
sed -i 's| -- |\x20\xE2\x80\x93\x20|g' "man3/"{IniFormat.3,IniStatistics.3,IniDispatch.3,confini.h.3}	# ' -- ' -> ' – '
rm ../"${MANUAL}"


on replaceText(find, replace, someText)
	set prevTIDs to text item delimiters of AppleScript
	set text item delimiters of AppleScript to find
	set someText to text items of someText
	set text item delimiters of AppleScript to replace
	set someText to "" & someText
	set text item delimiters of AppleScript to prevTIDs
	return someText
end replaceText

set fd to (open for access (path to resource "install.sh"))
set sh to (read fd for (get eof fd))
set legesmotus6 to (path to resource "legesmotus.6")
set lmserver6 to (path to resource "lmserver.6")
set sh to replaceText("$legesmotus6", quoted form of (POSIX path of legesmotus6), sh)
set sh to replaceText("$lmserver6", quoted form of (POSIX path of lmserver6), sh)
do shell script sh with administrator privileges
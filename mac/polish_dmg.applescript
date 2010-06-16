on run argv
	set lmdmg to item 1 of argv
	
	tell application "Finder"
		make new alias file to folder "Applications" of startup disk at disk lmdmg
		tell disk lmdmg
			open
			tell container window
				set current view to icon view
				set toolbar visible to false
				set statusbar visible to false
				set bounds to {100, 100, 100 + 640, 100 + 400}
			end tell
			set opts to icon view options of container window
			set bg to file ".lmbg.png"
			tell opts
				set icon size to 96
				set text size to 12
				set arrangement to not arranged
				set background picture to bg
			end tell
			set position of file "README.TXT" to {96, 280}
			set label index of file "README.TXT" to 2
			set position of folder "Other Stuff" to {544, 280}
			set label index of folder "Other Stuff" to 2
			set position of file "Leges Motus.app" to {224, 230}
			set label index of file "Leges Motus.app" to 4
			set position of file "Applications" to {416, 230}
			set label index of file "Applications" to 4
			delay 1
			close
		end tell
		update disk lmdmg
	end tell
end run
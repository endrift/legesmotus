BEGIN {
	#FS="~"
	#OFS="\t"
}

NR == 1 {
	print "name", $0
	next
}
NR == 2 {
	print "width", $1
	print "height", $2
	print ""
	FS="~"
	OFS="\t"
	next
}

/^[^#]/ && /[~]/ && $2 != 1 {
	printf "Warning: Non-standard object ID %d on line %d\n", $2, NR > "/dev/stderr"
}

# Obstacle
/^[^#]/ && /[~]/ && $1 == 1 {
	print "SPRITE", $3, "O", $4, $5
	next
}

# Gate
/^[^#]/ && /[~]/ && $1 == 2 && $4 == "A" {
	print "GATE", $3, "BLUE"
	next
}
/^[^#]/ && /[~]/ && $1 == 2 && $4 == "B" {
	print "GATE", $3, "RED"
	next
}

# Spawn point
/^[^#]/ && /[~]/ && $1 == 3 && $4 == "A" {
	print "SPAWN", $3, "BLUE"
	next
}
/^[^#]/ && /[~]/ && $1 == 3 && $4 == "B" {
	print "SPAWN", $3, "RED"
	next
}

# Decoration
/^[^#]/ && /[~]/ && $1 == 4 {
	print "SPRITE", $3, "B", $4
	next
}

# Background
/^[^#]/ && /[~]/ && $1 == 5 {
	print "SPRITE", $3, "TB", $4, $5, $6, $7
	next
}

# Unknown Object Type
/^[^#]/ && /[~]/ {
	printf "Warning: Unknown object type %d on line %d\n", $1, NR > "/dev/stderr"
	print "# " $0
	next
}

{
	print
}

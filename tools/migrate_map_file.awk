BEGIN {
	FS="~"
	OFS="\t"
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
/^[^#]/ && /[~]/ && $1 == 2 {
	print "GATE", $3, $4
	next
}

# Spawn point
/^[^#]/ && /[~]/ && $1 == 3 {
	print "SPAWN", $3, $4
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

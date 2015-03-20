{
	"targets" : [{
		"target_name" : "ofe",
		"sources" : [ "ofe.cc" ],
		"include_dirs": [
			'<!(node -e "require(\'nan\')")'
		]
	}]
}

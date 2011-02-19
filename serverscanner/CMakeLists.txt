add_executable(lmscan
	ServerScannerNetwork.cpp
	ServerScanner.cpp
	ServerList.cpp
	OutputGenerator.cpp
	JsonGenerator.cpp
	ReadableGenerator.cpp
	main.cpp
)

target_link_libraries(lmscan lmcommon)

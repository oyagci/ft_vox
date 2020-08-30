project "zlib"
    language "C"
    kind "StaticLib"

    files
    {
        "adler32.c",
        "compress.c",
        "crc32.c",
        "deflate.c",
        "gzclose.c",
        "gzlib.c",
        "gzread.c",
        "gzwrite.c",
        "infback.c",
        "inflate.c",
        "inftrees.c",
        "inffast.c",
        "trees.c",
        "uncompr.c",
        "zutil.c",
        "win32/zlib.def",
    }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
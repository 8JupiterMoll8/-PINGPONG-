Import("env")

env.Replace(
    UPLOADER="tycmd.exe",
    UPLOADCMD="$UPLOADER upload $UPLOADERFLAGS $SOURCE"
)


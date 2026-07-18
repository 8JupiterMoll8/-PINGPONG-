Import("env")

env.Replace(
    UPLOADER="$PROJECT_DIR/tycmd",
    UPLOADCMD="$UPLOADER upload $UPLOADERFLAGS $SOURCE"
)


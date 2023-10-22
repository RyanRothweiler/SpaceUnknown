extern "C" {

	EMSCRIPTEN_KEEPALIVE void FileSystemCreated() {

		printf("File system created \n");
		FileSystemReady = true;
	}

}


extern "C" {

	EMSCRIPTEN_KEEPALIVE void FileSystemCreated() {

		printf("File system created \n");
		FileSystemReady = true;
	}

	EMSCRIPTEN_KEEPALIVE void FileSystemDidSync() {

		GlobalEngineState->FileSynced = true;
	}

}


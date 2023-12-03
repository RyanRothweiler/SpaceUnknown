extern "C" {

	EMSCRIPTEN_KEEPALIVE void FileSystemCreated() {

		printf("File system created \n");
		FileSystemReady = true;
	}

	EMSCRIPTEN_KEEPALIVE void FileSystemDidSync() {

		GlobalEngineState->FileSystemSyncing = false;
		GlobalEngineState->FileSynced = true;
		GlobalEngineState->Saving = false;
	}

}


#include <node.h>
#include <v8.h>
#include <v8-profiler.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace v8;

class FileOutputStream: public OutputStream {
	public:
		FileOutputStream(FILE* stream): stream_(stream) { }
		virtual int GetChunkSize() {
			return 65536;
		}
		virtual void EndOfStream() { }
		virtual WriteResult WriteAsciiChunk(char* data, int size) {
			const size_t len = static_cast<size_t>(size);
			size_t off = 0;
			while (off < len && !feof(stream_) && !ferror(stream_))
				off += fwrite(data + off, 1, len - off, stream_);
			return off == len ? kContinue : kAbort;
		}

	private:
		FILE* stream_;
};

static void OnFatalError(const char* location, const char* message) {
	if (location) {
		fprintf(stderr, "FATAL ERROR: %s %s\n", location, message);
	} else {
		fprintf(stderr, "FATAL ERROR: %s\n", message);
	}
	fprintf(stderr, "Generating HeapDump\n");
	timeval tv;
	if (gettimeofday(&tv, NULL)) abort();

	char filename[256];
	snprintf(filename,
			sizeof(filename),
			"heapdump-%u.%u.heapsnapshot",
			static_cast<unsigned int>(tv.tv_sec),
			static_cast<unsigned int>(tv.tv_usec));
	FILE* fp = fopen(filename, "w");
	if (fp == NULL) abort();

	const HeapSnapshot* snap = HeapProfiler::TakeSnapshot(String::Empty());
	FileOutputStream stream(fp);
	snap->Serialize(&stream, HeapSnapshot::kJSON);
	fclose(fp);
	exit(1);
}

Handle<Value> Method(const Arguments& args) {
	HandleScope scope;
	V8::SetFatalErrorHandler(OnFatalError);
	return scope.Close(String::New("done"));
}

void Init(Handle<Object> target) {
	V8::SetFatalErrorHandler(OnFatalError);
	target->Set(String::NewSymbol("call"),
			FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(ofe, Init)

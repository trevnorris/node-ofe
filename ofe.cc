#include <nan.h>
#include <v8-profiler.h>
#include <stdlib.h>
#if defined(_WIN32)
  #include <time.h>
  #define snprintf _snprintf
#else
  #include <sys/time.h>
#endif

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
  if (location)
    fprintf(stderr, "FATAL ERROR: %s %s\n", location, message);
  else
    fprintf(stderr, "FATAL ERROR: %s\n", message);

  fprintf(stderr, "Generating HeapDump\n");

  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  char filename[256];
  strftime(filename, sizeof(filename),"%Y%m%dT%H%M%S.heapsnapshot", timeinfo);

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) abort();

  const HeapSnapshot* snap =
      Isolate::GetCurrent()->GetHeapProfiler()->TakeHeapSnapshot();

  FileOutputStream stream(fp);
  snap->Serialize(&stream, HeapSnapshot::kJSON);
  fclose(fp);
  exit(1);
}

NAN_METHOD(Call) {
  Nan::HandleScope scope;
  Isolate::GetCurrent()->SetFatalErrorHandler(OnFatalError);
  info.GetReturnValue().Set(Nan::New("done").ToLocalChecked());
}

NAN_METHOD(Trigger) {
  Nan::HandleScope scope;
  OnFatalError("here", "and there");
}

NODE_MODULE_INIT(/* exports, module, context */) {
  exports->Set(
      context,
      Nan::New("call").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Call)->GetFunction(context)
        .ToLocalChecked()).FromJust();
  exports->Set(
      context,
      Nan::New("trigger").ToLocalChecked(),
      Nan::New<FunctionTemplate>(Trigger)->GetFunction(context)
        .ToLocalChecked()).FromJust();
}

#ifdef _WIN32
  #include <windows.h>
  #define PID DWORD
#else
  #include <sys/types.h>
  #define PID pid_t
#endif

namespace IPC{
  class Message{
  public:
    Message(char* data, size_t len);
    ~Message();

    void setPID(PID pid);
    void setSubject(char* subject);
    size_t getLen();
    char* getData();
    void* getCPointer();

  private:
    void* ptr;
  };
}

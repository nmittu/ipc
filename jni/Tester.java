import com.mittudev.ipc.Message;

class Tester{
  public static void main(String[] args) {
    Message msg = new Message("Testing the Java JNI API".getBytes());
    msg.setSubject("Test");
    msg.destroy();
  }
}

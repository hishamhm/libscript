
public class javatestjni {
   private String[] words = { "Earth", "Wind", "Fire", "Water" };
   private native void myNativeMethod();
   public static void main(String[] args) {
      new javatestjni().myNativeMethod();
   }
   static {
      System.loadLibrary("javatestjni");
   }
}


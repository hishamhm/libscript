
public class javatestjni {
   private String[] words = { "Earth", "Wind", "Fire", "Water" };
   private Double[] numbers = { 1.0, 2.0, 1.2, 2.12, 12.212, 2121.2212 };
   private native void myNativeMethod();
   private native String concatArray(Object[] nums);
   public static void main(String[] args) {
      javatestjni j = new javatestjni();
      j.myNativeMethod();
      System.out.println("The sum is: " + j.concatArray(j.words));
      System.out.println("The sum is: " + j.concatArray(j.numbers));
   }
   static {
      System.loadLibrary("javatestjni");
   }
}



public class javapoly{
   private native int poly();
   private native int poly(double d);
   private native double poly(int i);
   static private native double poly(int i, double d);
   public static void main(String[] args) {
   }
   static {
      System.loadLibrary("javapoly");
   }
}

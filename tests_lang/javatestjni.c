
#include <jni.h>
#include <stdio.h>
#include "javatestjni.h"

JNIEXPORT void JNICALL Java_javatestjni_myNativeMethod(JNIEnv *J, jobject this) {
   jclass klass = (*J)->GetObjectClass(J, this);
   jfieldID wordsID = (*J)->GetFieldID(J, klass, "words", "[Ljava/lang/String;");
   jarray words = (*J)->GetObjectField(J, this, wordsID);
   jstring words1 = (*J)->GetObjectArrayElement(J, words, 1);
   const char* words1str = (*J)->GetStringUTFChars(J, words1, NULL);
   printf("%s\n", words1str);
   (*J)->ReleaseStringUTFChars(J, words1, words1str);
}

JNIEXPORT jstring JNICALL Java_javatestjni_concatArray(JNIEnv *J, jobject this, jobjectArray t) {
   static jmethodID concat = NULL, toString = NULL;
   int i;
   if (!concat) {
      jclass cls = (*J)->FindClass(J, "java/lang/String");
      concat = (*J)->GetMethodID(J, cls, "concat",
         "(Ljava/lang/String;)Ljava/lang/String;");
      cls = (*J)->FindClass(J, "java/lang/Object");
      toString = (*J)->GetMethodID(J, cls, "toString",
         "()Ljava/lang/String;");
   }
   jstring s = (*J)->NewString(J, NULL, 0);
   s = (*J)->NewGlobalRef(J, (jobject) s);
   int len = (*J)->GetArrayLength(J, t);
   for (i = 0; i < len; i++) {
      (*J)->PushLocalFrame(J, 10);
      jobject r = (*J)->GetObjectArrayElement(J, t, i);
      jstring str = (*J)->CallObjectMethod(J, r, toString);
      str = (*J)->CallObjectMethod(J, s, concat, str);
      (*J)->DeleteGlobalRef(J, s);
      s = (*J)->NewGlobalRef(J, str);
      (*J)->PopLocalFrame(J, NULL);
   }
   jobject ret = (*J)->NewLocalRef(J, s);
   (*J)->DeleteGlobalRef(J, s);
   return ret;
}


#include <jni.h>
#include <stdio.h>
#include "javatestjni.h"

JNIEXPORT void JNICALL Java_javatestjni_myNativeMethod(JNIEnv *env, jobject this) {
   jclass klass = (*env)->GetObjectClass(env, this);
   jfieldID wordsID = (*env)->GetFieldID(env, klass, "words", "[Ljava/lang/String;");
   jarray words = (*env)->GetObjectField(env, this, wordsID);
   jstring words1 = (*env)->GetObjectArrayElement(env, words, 1);
   const char* words1str = (*env)->GetStringUTFChars(env, words1, NULL);
   printf("%s\n", words1str);
   (*env)->ReleaseStringUTFChars(env, words1, words1str);
}

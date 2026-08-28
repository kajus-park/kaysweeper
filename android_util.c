#ifndef KAY_ANDROID_HELPER
#define KAY_ANDROID_HELPER

#include <android_native_app_glue.h>
#include <jni.h>
#include <stdio.h>
#include <unistd.h>
extern struct android_app *gapp;

void vibrate(void) {
  JNIEnv *env = NULL;
  (*gapp->activity->vm)->AttachCurrentThread(gapp->activity->vm, &env, NULL);

  jobject activity = gapp->activity->clazz;
  jclass activity_class = (*env)->GetObjectClass(env, activity);

  jmethodID getSystemService =
      (*env)->GetMethodID(env, activity_class, "getSystemService",
                          "(Ljava/lang/String;)Ljava/lang/Object;");

  jstring service_name = (*env)->NewStringUTF(env, "vibrator");

  jobject vibrator =
      (*env)->CallObjectMethod(env, activity, getSystemService, service_name);

  jclass vibrator_class = (*env)->GetObjectClass(env, vibrator);

  // Check that the device actually has a vibrator.
  jmethodID hasVibrator =
      (*env)->GetMethodID(env, vibrator_class, "hasVibrator", "()Z");

  jboolean has_vibrator = (*env)->CallBooleanMethod(env, vibrator, hasVibrator);

  printf("has vibrator: %d\n", has_vibrator);

  if (has_vibrator) {
    jclass effect_class = (*env)->FindClass(env, "android/os/VibrationEffect");

    jmethodID createOneShot = (*env)->GetStaticMethodID(
        env, effect_class, "createOneShot", "(JI)Landroid/os/VibrationEffect;");

    jobject effect = (*env)->CallStaticObjectMethod(
        env, effect_class, createOneShot, (jlong)50, 255);

    jmethodID vibrate = (*env)->GetMethodID(env, vibrator_class, "vibrate",
                                            "(Landroid/os/VibrationEffect;)V");

    (*env)->CallVoidMethod(env, vibrator, vibrate, effect);

    (*env)->DeleteLocalRef(env, effect);
    (*env)->DeleteLocalRef(env, effect_class);
  }

  (*env)->DeleteLocalRef(env, vibrator_class);
  (*env)->DeleteLocalRef(env, vibrator);
  (*env)->DeleteLocalRef(env, service_name);
  (*env)->DeleteLocalRef(env, activity_class);

  (*gapp->activity->vm)->DetachCurrentThread(gapp->activity->vm);
}
#endif // KAY_ANDROID_HELPER

#include <jni.h>
#include <stdint.h>
#include <string.h>
#include <chromaprint.h>

static ChromaprintContext *GetChromaprintContext(JNIEnv *env, jobject obj) {
    jclass c = (*env)->GetObjectClass(env, obj);
    jfieldID f = (*env)->GetFieldID(env, c, "ctx", "J");
    return (ChromaprintContext *) (uintptr_t) (*env)->GetLongField(env, obj, f);
}

static void SetChromaprintContext(JNIEnv *env, jobject obj, ChromaprintContext *ctx) {
    jclass c = (*env)->GetObjectClass(env, obj);
    jfieldID f = (*env)->GetFieldID(env, c, "ctx", "J");
    (*env)->SetLongField(env, obj, f, (jlong) (uintptr_t) ctx);
}

JNIEXPORT void JNICALL Java_org_acoustid_chromaprint_Chromaprint_create(JNIEnv *env, jobject obj) {
    ChromaprintContext *ctx;

    ctx = chromaprint_new(CHROMAPRINT_ALGORITHM_DEFAULT);

    SetChromaprintContext(env, obj, ctx);
}

JNIEXPORT void JNICALL Java_org_acoustid_chromaprint_Chromaprint_destroy(JNIEnv *env, jobject obj) {
    ChromaprintContext *ctx;

    ctx = GetChromaprintContext(env, obj);
    chromaprint_free(ctx);

    SetChromaprintContext(env, obj, NULL);
}

JNIEXPORT void JNICALL
Java_org_acoustid_chromaprint_Chromaprint_start(JNIEnv *env, jobject obj, jint sample_rate,
                                                jint channels) {
    ChromaprintContext *ctx;

    ctx = GetChromaprintContext(env, obj);
    chromaprint_start(ctx, sample_rate, channels);
}

JNIEXPORT void JNICALL
Java_org_acoustid_chromaprint_Chromaprint_feed(JNIEnv *env, jobject obj, jshortArray data) {
    ChromaprintContext *ctx;

    ctx = GetChromaprintContext(env, obj);
    jshort *pcm = (*env)->GetShortArrayElements(env, data, 0);
    chromaprint_feed(ctx, pcm, (*env)->GetArrayLength(env, data));
    (*env)->ReleaseShortArrayElements(env, data, pcm, 0);
}

JNIEXPORT void JNICALL
Java_org_acoustid_chromaprint_Chromaprint_finish(JNIEnv *env, jobject obj, jobject buffer) {
    ChromaprintContext *ctx;

    ctx = GetChromaprintContext(env, obj);
    chromaprint_finish(ctx);
}

JNIEXPORT jstring JNICALL
Java_org_acoustid_chromaprint_Chromaprint_getFingerprint(JNIEnv *env, jobject obj) {
    ChromaprintContext *ctx;
    char *fingerprint;
    jstring result;

    ctx = GetChromaprintContext(env, obj);
    chromaprint_get_fingerprint(ctx, &fingerprint);

    result = (*env)->NewStringUTF(env, fingerprint);

    chromaprint_dealloc(fingerprint);

    return result;
}

JNIEXPORT jintArray JNICALL
Java_org_acoustid_chromaprint_Chromaprint_getRawFingerprint(JNIEnv *env, jobject obj) {
    ChromaprintContext *ctx;
    int32_t *fingerprint;
    int i, length;
    jintArray result;

    ctx = GetChromaprintContext(env, obj);
    chromaprint_get_raw_fingerprint(ctx, (void **) &fingerprint, &length);

    result = (*env)->NewIntArray(env, length);
    (*env)->SetIntArrayRegion(env, result, 0, length, fingerprint);
    chromaprint_dealloc(fingerprint);

    return result;
}
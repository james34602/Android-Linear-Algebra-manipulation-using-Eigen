#include <iostream>
#include <math.h>
#include <jni.h>

#include <Eigen/Dense>

using namespace Eigen;

extern "C"
{
JNIEXPORT jdoubleArray JNICALL Java_james_androideigen_EigenLib_adding (JNIEnv *env, jobject obj, jdoubleArray fltarray1, jdoubleArray fltarray2,jint row,jint col)
{

	jdoubleArray result;
	jsize alen1 = env->GetArrayLength(fltarray1); //�������
	jsize alen2 = env->GetArrayLength(fltarray2); //�������
	//�P�_���׬O�_�@�P
	if(alen1!=alen2)
	{
		return NULL;
	}
	//�PŪ���׬O�_���T
	if(alen1!=row*col)
	{
		return NULL;
	}
	result = env->NewDoubleArray(alen1);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}
	jdouble* out;
	jdouble* flt1 = env->GetDoubleArrayElements( fltarray1,0);
	jdouble* flt2 = env->GetDoubleArrayElements( fltarray2,0);

	//�ʺA�x�}�A�B��ɽT�w MatrixXd
	Map<MatrixXd> dymMat1(flt1,row,col);
	Map<MatrixXd> dymMat2(flt2,row,col);

	MatrixXd xdAdd=dymMat1+dymMat2;
	out=xdAdd.data();

	int len=(int)(row*col);
	env->SetDoubleArrayRegion(result, 0, len, out);
	env->ReleaseDoubleArrayElements(fltarray1, flt1, 0);
	env->ReleaseDoubleArrayElements(fltarray2, flt2, 0);
	return result;
}
JNIEXPORT jdoubleArray JNICALL Java_james_androideigen_EigenLib_subtract (JNIEnv *env, jobject obj, jdoubleArray fltarray1, jdoubleArray fltarray2,jint row,jint col)
{

	jdoubleArray result;
	jsize alen1 = env->GetArrayLength(fltarray1); //�������
	jsize alen2 = env->GetArrayLength(fltarray2); //�������
	//�P�_���׬O�_�@�P
	if(alen1!=alen2)
	{
		return NULL;
	}
	//�PŪ���׬O�_���T
	if(alen1!=row*col)
	{
		return NULL;
	}
	result = env->NewDoubleArray(alen1);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}
	jdouble* out;
	jdouble* flt1 = env->GetDoubleArrayElements( fltarray1,0);
	jdouble* flt2 = env->GetDoubleArrayElements( fltarray2,0);

	//�ʺA�x�}�A�B��ɽT�w MatrixXd
	Map<MatrixXd> dymMat1(flt1,row,col);
	Map<MatrixXd> dymMat2(flt2,row,col);

	MatrixXd xd=dymMat1-dymMat2;
	out=xd.data();

	int len=(int)(row*col);
	env->SetDoubleArrayRegion(result, 0, len, out);
	env->ReleaseDoubleArrayElements(fltarray1, flt1, 0);
	env->ReleaseDoubleArrayElements(fltarray2, flt2, 0);
	return result;
}
JNIEXPORT jdoubleArray JNICALL Java_james_androideigen_EigenLib_multiply (JNIEnv *env, jobject obj, jdoubleArray fltarray1, jdoubleArray fltarray2,jint row,jint col)
{

	jdoubleArray result;
	jsize alen1 = env->GetArrayLength(fltarray1); //�������
	jsize alen2 = env->GetArrayLength(fltarray2); //�������
	//�P�_���׬O�_�@�P
	if(alen1!=alen2)
	{
		return NULL;
	}
	//�PŪ���׬O�_���T
	if(alen1!=row*col)
	{
		return NULL;
	}
	result = env->NewDoubleArray(alen1);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}
	jdouble* out;
	jdouble* flt1 = env->GetDoubleArrayElements( fltarray1,0);
	jdouble* flt2 = env->GetDoubleArrayElements( fltarray2,0);

	//�ʺA�x�}�A�B��ɽT�w MatrixXd
	Map<MatrixXd> dymMat1(flt1,row,col);
	Map<MatrixXd> dymMat2(flt2,row,col);

	MatrixXd xd=dymMat2*dymMat1;
	out=xd.data();

	int len=(int)(row*col);
	env->SetDoubleArrayRegion(result, 0, len, out);
	env->ReleaseDoubleArrayElements(fltarray1, flt1, 0);
	env->ReleaseDoubleArrayElements(fltarray2, flt2, 0);
	return result;
}
JNIEXPORT jdoubleArray JNICALL Java_james_androideigen_EigenLib_division (JNIEnv *env, jobject obj, jdoubleArray fltarray1, jdoubleArray fltarray2,jint row,jint col)
{

	jdoubleArray result;
	jsize alen1 = env->GetArrayLength(fltarray1); //�������
	jsize alen2 = env->GetArrayLength(fltarray2); //�������
	//�P�_���׬O�_�@�P
	if(alen1!=alen2)
	{
		return NULL;
	}
	//�PŪ���׬O�_���T
	if(alen1!=row*col)
	{
		return NULL;
	}
	result = env->NewDoubleArray(alen1);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}
	jdouble* out;
	jdouble* flt1 = env->GetDoubleArrayElements( fltarray1,0);
	jdouble* flt2 = env->GetDoubleArrayElements( fltarray2,0);

	//�ʺA�x�}�A�B��ɽT�w MatrixXd
	Map<MatrixXd> dymMat1(flt1,row,col);
	Map<MatrixXd> dymMat2(flt2,row,col);

	MatrixXd xd=dymMat1.inverse()*dymMat2;
	out=xd.data();

	int len=(int)(row*col);
	env->SetDoubleArrayRegion(result, 0, len, out);
	env->ReleaseDoubleArrayElements(fltarray1, flt1, 0);
	env->ReleaseDoubleArrayElements(fltarray2, flt2, 0);
	return result;
}
JNIEXPORT jdoubleArray JNICALL Java_james_androideigen_EigenLib_transpose (JNIEnv *env, jobject obj, jdoubleArray fltarray1,jint row,jint col)
{

	jdoubleArray result;
	jsize alen1 = env->GetArrayLength(fltarray1); //�������
	result = env->NewDoubleArray(alen1);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}
	jdouble* out;
	jdouble* flt1 = env->GetDoubleArrayElements( fltarray1,0);

	//�ʺA�x�}�A�B��ɽT�w MatrixXd
	Map<MatrixXd> dymMat1(flt1,row,col);

	MatrixXd xd=dymMat1.transpose();
	out=xd.data();

	int len=(int)(row*col);
	env->SetDoubleArrayRegion(result, 0, len, out);
	env->ReleaseDoubleArrayElements(fltarray1, flt1, 0);
	return result;
}
}

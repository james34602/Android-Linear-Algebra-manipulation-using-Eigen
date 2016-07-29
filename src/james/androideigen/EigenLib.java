package james.androideigen;

public class EigenLib {

	static {
		System.loadLibrary("eigentest");
	}
	public static native double[] adding(double[] array1, double[] array2,int row,int col);
	public static native double[] subtract(double[] array1, double[] array2,int row,int col);
	public static native double[] multiply(double[] array1, double[] array2,int row,int col);
	public static native double[] division(double[] array1, double[] array2,int row,int col);
	public static native double[] transpose(double[] array1,int row,int col);
}
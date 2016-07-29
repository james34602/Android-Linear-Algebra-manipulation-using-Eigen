package james.androideigen;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;

public class MainActivity extends Activity {

	private TextView textView1;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		super.setContentView(R.layout.activity_main);
		textView1=(TextView)findViewById(R.id.textView1);
		TextView tv = textView1;
		tv.setMovementMethod(new ScrollingMovementMethod());	
		double[] f1={1,324,65,76,87,98,45,56};
		double[] f2={23,45,6,7,34,56,76,87};
		double[] f3={1,4,6,7,8,9,4,6,4,2,4,6,2,8,5,9};
		double[] f4={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
		double[] fAdd=EigenLib.adding(f1,f2,2,4);
		double[] fSub=EigenLib.subtract(f1,f2,2,4);
		double[] fMul=EigenLib.multiply(f3,f4,4,4);
		double[] fDiv=EigenLib.division(f3,f4,4,4);
		double[] fTranspM1=EigenLib.transpose(f1,4,2);
		double[] fTranspM2=EigenLib.transpose(f2,4,2);
		StringBuffer sb=new StringBuffer();
		sb.append("�Ĥ@�կx�}��:\n");
		int len=f1.length;
		for(int i=0;i<len;i++){
			sb.append(f1[i]);
			if(i!=len-1){
				sb.append("�B");
			}
		}
		sb.append("\n�ĤG�կx�}��:\n");
		len=f2.length;
		for(int i=0;i<len;i++){
			sb.append(f2[i]);
			if(i!=len-1){
				sb.append("�B");
			}
		}
		sb.append("\n�ĤT�կx�}��A(4x4 Matrix):\n");
		len=f3.length;
		for(int i=0;i<len;i++){
			sb.append(f3[i]);
			if(i!=len-1){
				sb.append("�B");
			}
		}
		sb.append("\n�ĥ|�կx�}��B(4x4 Matrix):\n");
		len=f4.length;
		for(int i=0;i<len;i++){
			sb.append(f4[i]);
			if(i!=len-1){
				sb.append("�B");
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("�x�}�@�[�x�}�G���G:");
		if(fAdd==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fAdd.length;
			for(int i=0;i<len;i++){
				sb.append(fAdd[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("�x�}�@��x�}�G���G:");
		if(fSub==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fSub.length;
			for(int i=0;i<len;i++){
				sb.append(fSub[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("�x�}�|���x�}�T���G(B*A):");
		if(fMul==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fMul.length;
			for(int i=0;i<len;i++){
				sb.append(fMul[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("�x�}�|���x�}�T���G(B/A):");
		sb.append("\nMatrix division is computed from transpose matrix A and multiplying B\n");
		if(fDiv==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fDiv.length;
			for(int i=0;i<len;i++){
				sb.append(fDiv[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("��m�x�}1���G:");
		if(fTranspM1==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fTranspM1.length;
			for(int i=0;i<len;i++){
				sb.append(fTranspM1[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		sb.append("\n");
		sb.append("\n");
		sb.append("��m�x�}2���G:");
		if(fTranspM2==null){
			sb.append("�ƾڷ��X��!");
		}else{
			len=fTranspM2.length;
			for(int i=0;i<len;i++){
				sb.append(fTranspM2[i]);
				if(i!=len-1){
					sb.append("�B");
				}
			}
		}
		textView1.setText(new String(sb));
	}

}

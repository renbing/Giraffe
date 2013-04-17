package com.woyouquan.Giraffe;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.InputType;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.Gravity;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;

@SuppressLint("HandlerLeak")
public class GiraffeActivity extends Activity {
    /** Called when the activity is first created. */
	
	static final String packageName = "com.woyouquan.Giraffe";
	private GLSurfaceView mGLView;
	private TextView mFps;
	private TextView mConsole;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        //android.os.Debug.waitForDebugger();
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
           
    	ApplicationInfo appInfo = null;
    	PackageManager packMgmr = this.getApplication().getPackageManager();
    	try
    	{
    		appInfo = packMgmr.getApplicationInfo(packageName, 0);
    	}
    	catch(NameNotFoundException e)
    	{
    		e.printStackTrace();
    		throw new RuntimeException("Unable to load asset");
    	}
    	
    	String apkPath = appInfo.sourceDir;
    	nativeInit(apkPath);
    	
    	setContentView(R.layout.main);
    	
        mGLView = new EAGLView(this, handler);        
        //mGLView.setOnTouchListener(new TouchListener());
        
        FrameLayout layout = (FrameLayout) findViewById(R.id.mainLayout);
        layout.addView(mGLView, 0);
        
        mFps = (TextView)findViewById(R.id.fps);
        mFps.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if( mConsole.getVisibility() == View.INVISIBLE )
				{
					mConsole.setVisibility(View.VISIBLE);
				}
				else
				{
					mConsole.setVisibility(View.INVISIBLE);
				}
			}
		});
        
        mConsole = (TextView)findViewById(R.id.console);
        mConsole.setMovementMethod(new ScrollingMovementMethod());
        mConsole.setVisibility(View.INVISIBLE);
        
        EditText input = new EditText(this);
        input.setSingleLine(true);
        input.setHint("hello");
        input.setHintTextColor(Color.RED);
        //input.setBackgroundColor(Color.TRANSPARENT);
        //input.setBackgroundDrawable(null);
        input.setTextColor(Color.RED);
        input.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);
        input.setImeOptions(EditorInfo.IME_ACTION_DONE);
        input.setOnTouchListener(new OnTouchListener() {
        	public boolean onTouch(View v, MotionEvent event) {
        		((EditText)v).setText(String.valueOf(v.hasFocus()));
    			InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
    			imm.showSoftInput(v, 0);
        		return false;
        	}
        });
        input.setOnEditorActionListener(new OnEditorActionListener() {
        	public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        		if( actionId == EditorInfo.IME_ACTION_DONE
        				|| event.getAction() == KeyEvent.ACTION_DOWN 
        				|| event.getKeyCode() == KeyEvent.KEYCODE_ENTER){
        			v.setText("end");
        			v.setInputType(InputType.TYPE_NULL);
        			InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        			imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        			return true;
        		}
        		return false;
        	}
        });
        //stringFromJNI();
    }
    
    @Override
    public void onDestroy()
    {
    	super.onDestroy();
    	//mGLView.onPause();
    	
    	//nativeDone();
    	android.os.Process.killProcess(android.os.Process.myPid());
    }
    
	@Override
	protected void onPause() {
		super.onPause();
		//mGLView.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		//mGLView.onResume();
	}
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if( keyCode == KeyEvent.KEYCODE_BACK )
		{
			onDestroy();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
	public static final int CanvasAlert = 0x101;
	public static final int FpsUpdate = 0x102;
	public static final int ConsoleUpdate = 0x103;
	
	Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			Bundle b = msg.getData();
			switch(msg.what) {
				case GiraffeActivity.CanvasAlert:
					String alertContent = b.getString("content");
					alert(alertContent);
					break;
				case GiraffeActivity.FpsUpdate:
					int fps = b.getInt("fps");
					mFps.setText("fps:" + String.valueOf(fps));
					break;
				case GiraffeActivity.ConsoleUpdate:
					String traceContent = b.getString("content");
					mConsole.append(traceContent + "\n");
					break;
				default:
					break;
			}
			super.handleMessage(msg);
		}
	};
	
	public void alert(String content) {
        AlertDialog dialog = new AlertDialog.Builder(this).setTitle("alert")
        		.setMessage(content)
        		.setPositiveButton("确认", null)
        		.create();
        dialog.show();
	}
	
    private native String stringFromJNI();
    private native void nativeInit(String apkPath);
    private native void nativeDone();
    
    static {
        System.loadLibrary("gljni");
    }
}

class EAGLView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnScaleGestureListener {
	
	private Handler uiHandler;
	private GestureDetector mGesture;
	private ScaleGestureDetector mScaleGesture;
	private float mScale = 1.0f;
	
	private long time = System.currentTimeMillis();
	private int fpCount = 0;
	private int fps = 0;
	
	private static final int SWIPE_MIN_DISTANCE = 200; 
	private static final int SWIPE_MIN_VELOCITY = 400;
		
    private native void nativeInit();
    private native void nativeResize(int w, int h);
    private native void nativeRender();
    
	private static native void nativeDown(float x, float y);
	private static native void nativeMove(float x, float y);
	private static native void nativeUp(float x, float y);
	
	private static native void nativeTouch(float x, float y);
	private static native void nativeTap(float x, float y);
	private static native void nativeDrag(int state, float dx, float dy);
	private static native void nativeSwipe(int direction);
	private static native void nativePinch(float scale);
	private static native void nativeLongPress();
	
	
    public EAGLView(Context context, Handler handler) {
        super(context);
        uiHandler = handler;
        mGesture = new GestureDetector(context, this);
        mScaleGesture = new ScaleGestureDetector(context, this);
        
        setEGLContextFactory(new ContextFactory());
        setRenderer(this);
    }
    
    public boolean onTouchEvent(final MotionEvent event) {
    	final EAGLView glView = this;
    	queueEvent(new Runnable() {
    		public void run() {
    			glView.handleTouchEvent(event);
    		}
    	});

		return true;
    }

	public void onDrawFrame(GL10 gl) {
		nativeRender();
		
    	fpCount++;
    	if( (System.currentTimeMillis() - time) > 500 )
    	{
    		fps = (int) (fpCount * 1000 / (System.currentTimeMillis() - time));
        	//android.util.Log.v("fps", "fps:" + String.valueOf(fps));
    		time = System.currentTimeMillis();
    		fpCount = 0;
    		
            Message msg = new Message();
            msg.what = GiraffeActivity.FpsUpdate;
            Bundle data = new Bundle();
            data.putInt("fps", fps);
            msg.setData(data);
            uiHandler.sendMessage(msg);
    	}
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
        nativeResize(width, height);		
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		nativeInit();
	}
	
    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            Log.w("createContext", "creating OpenGL ES 2.0 context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
            EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }
    
    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            Log.e("checkEglError", String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }
	
    public void handleTouchEvent(MotionEvent event) {
    	//Log.i("gesture", String.valueOf(event.getPointerCount()));
    	if( event.getPointerCount() == 2 ) {
    		mScaleGesture.onTouchEvent(event);
    	}else {
    		mGesture.onTouchEvent(event);
    	}
    	/*
		float x = event.getX();
		float y = event.getY();

		int action = event.getAction();
		switch(action)
		{
			case MotionEvent.ACTION_DOWN:
				nativeDown(x, y);
				break;
			case MotionEvent.ACTION_MOVE:
				nativeMove(x, y);
				break;
			case MotionEvent.ACTION_UP:
				nativeUp(x, y);
				break;
			default:
				break;
		}
		*/
    }
    
    @Override
    public boolean onDown(MotionEvent e) {
    	nativeTouch(e.getX(), e.getY());
    	return true;
    }
    
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
    	if(Math.abs(velocityX) > SWIPE_MIN_VELOCITY) {
    		if( velocityX < 0 ) {
    			// swipe left
    			nativeSwipe(7);
    		}else{
    			// swipe right
    			nativeSwipe(8);
    		}
    	}else if(Math.abs(velocityX) > SWIPE_MIN_VELOCITY) { 
    		if( velocityY < 0 ){
    			// swipe up
    			nativeSwipe(9);
    		}else {
    			// swipe down
    			nativeSwipe(10);
    		}
    	}
    	return true;
    }
    
    @Override
    public void onLongPress(MotionEvent e) {
    	nativeLongPress();
    }
    
	@Override
	public void onShowPress(MotionEvent e) {
		
	}
    
	@Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
		//Log.i("gesture", String.format("Drag: %f,%f", distanceX, distanceY));
    	nativeDrag(1, -distanceX, -distanceY);
    	return true;
    }
    
	@Override
    public boolean onSingleTapUp(MotionEvent e) {
    	nativeTap(e.getX(), e.getY());
    	return true;
    }
	
	@Override
	public boolean onScale(ScaleGestureDetector detector) {
		nativePinch(detector.getScaleFactor() / mScale);
		mScale = detector.getScaleFactor();
		return false;
	}
	
	@Override
	public boolean onScaleBegin(ScaleGestureDetector detector) {
		mScale = 1.0f;
		return true;
	}
	
	@Override
	public void onScaleEnd(ScaleGestureDetector detector) {
		mScale = 1.0f;
	}

    // NDK C++调用Java函数
    public void alert(String content) {
        Message msg = new Message();
        msg.what = GiraffeActivity.CanvasAlert;
        Bundle data = new Bundle();
        data.putString("content", content);
        msg.setData(data);
        uiHandler.sendMessage(msg);
    }
    
    public void trace(String content) {
        Message msg = new Message();
        msg.what = GiraffeActivity.ConsoleUpdate;
        Bundle data = new Bundle();
        data.putString("content", content);
        msg.setData(data);
        uiHandler.sendMessage(msg);
    }
    
    public JavaAudioPlayer newAudioPlayer() {
    	return new JavaAudioPlayer(this.getContext());
    }
    
    public JavaBitmap newBitmap(int w, int h) {
    	return new JavaBitmap(this.getContext(), w, h);
    }
    
    public void createTextBitmap(String text, int w, int h, int pw, int ph, String textAlign, String fontName, 
    								float fontSize, String fontSizeUnit, int textColor, boolean bold, boolean italic, byte[] pixels) {
       	
    	JavaBitmap jb = new JavaBitmap(this.getContext(), pw, ph);
    	jb.fillText(text, 0, 0, w, h, textAlign, fontName, fontSize, fontSizeUnit, textColor, bold, italic);
    	jb.getPixels(pixels);
    }
}
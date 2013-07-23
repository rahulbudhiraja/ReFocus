package com.tesseract.refocus;

/*
 * Copyright (C) 2011 HTC Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore.Images.Media;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;

import com.htc.view.DisplaySetting;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;

// S3D example of live camera preview (touch screen to toggle mode) and overlay text indicating 3D or 2D mode
public class CustomClickActivity extends Activity implements SurfaceHolder.Callback {

	private final static String TAG = "imagetest";
	/*
	 * CAMERA_STEREOSCOPIC Used to open S3D camera on devices running Android older than
	 * ICS, the id value is 2. For Android ICS devices, the id value is 100
	 * See how to check the device's Android version below in getS3DCamera()
	 */
	private final static int CAMERA_STEREOSCOPIC = 2;
	private final static int CAMERA_STEREOSCOPIC_ICS = 100;
	private boolean is3Denabled = true;
	private SurfaceHolder holder;
	private SurfaceView preview;
	private Camera camera;
	private TextView text;
	private int width, height;
	Mat img, gray;
	ImageButton ClickButton;
	static int imageNumber=0;
	// create a File object for the parent directory
	File saveDirectory = new File(Environment.getExternalStorageDirectory()+"/SimpleImage/");
	private Timer myTimer;

	Bitmap TestBitmap;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    System.loadLibrary("disp_img");
                    //mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
		setContentView(R.layout.video);
		preview = (SurfaceView) findViewById(R.id.surface);
		text = (TextView) findViewById(R.id.text);
		ClickButton=(ImageButton)findViewById(R.id.imageButton1);

		
		myTimer = new Timer();
	    myTimer.schedule(new TimerTask() {      
	      @Override
	      public void run() {
	        TimerMethod();
	      }
	      
	    }, 0, 5000);
	    

		saveDirectory.delete();
		
		saveDirectory.mkdirs();
		
		imageNumber++;
		
		holder = preview.getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	}

private void TimerMethod()
{
  //This method is called directly by the timer
  //and runs in the same thread as the timer.

	
	
  //We call the method that will work with the UI
  //through the runOnUiThread method.
  this.runOnUiThread(Timer_Tick);
  
  
}


private Runnable Timer_Tick = new Runnable() {
  public void run() {
  
  //This method runs in the same thread as the UI.             
  
  //Do something to the UI thread here
	  
	        
	      }
  
};




	public void surfaceCreated(SurfaceHolder holder) {
		camera = getS3DCamera();
		if (is3Denabled) {
			text.setText("S3D");
		} else {
			camera = get2DCamera();
			text.setText("2D");
		}
		text.setVisibility(View.VISIBLE);
	}

	public Camera get2DCamera() {
		Camera camera = null;
		try {
			camera = Camera.open();
			camera.setPreviewDisplay(holder);
		} catch (IOException ioe) {
			if (camera != null) {
				camera.release();
			}
			camera = null;
		} catch (RuntimeException rte) {
			if (camera != null) {
				camera.release();
			}
			camera = null;
		}
		return camera;
	}

	public Camera getS3DCamera() {
		Camera camera = null;
		int cameraID = Build.VERSION.SDK_INT < Build.VERSION_CODES.ICE_CREAM_SANDWICH ?
				CAMERA_STEREOSCOPIC : CAMERA_STEREOSCOPIC_ICS;
		try {
			camera = Camera.open(cameraID);
			camera.setPreviewDisplay(holder);
			is3Denabled = true;
		} catch (IOException ioe) {
			if (camera != null) {
				camera.release();
			}
			camera = null;
		} catch (NoSuchMethodError nsme) {
			is3Denabled = false;
			text.setVisibility(View.VISIBLE);
			Log.w(TAG, Log.getStackTraceString(nsme));
		} catch (UnsatisfiedLinkError usle) {
			is3Denabled = false;
			text.setVisibility(View.VISIBLE);
			Log.w(TAG, Log.getStackTraceString(usle));
		} catch (RuntimeException re) {
			is3Denabled = false;
			text.setVisibility(View.VISIBLE);
			Log.w(TAG, Log.getStackTraceString(re));
			if (camera != null) {
				camera.release();
			}
			camera = null;
		}
		return camera;
	}

	public void surfaceDestroyed(SurfaceHolder surfaceholder) {
		stopPreview();
		holder = surfaceholder;
		enableS3D(false, surfaceholder.getSurface()); // to make sure it's off
	}

	private void stopPreview() {
		if (camera != null) {
			camera.stopPreview();
			camera.release();
			camera = null;
		}
	}

	private Size getOptimalPreviewSize(List<Size> sizes, int w, int h) {
		final double ASPECT_TOLERANCE = 0.05;
		double targetRatio = (double) w / h;
		if (sizes == null)
			return null;

		Size optimalSize = null;
		double minDiff = Double.MAX_VALUE;

		int targetHeight = h;

		for (Size size : sizes) {
			Log.i(TAG, String.format("width=%d height=%d", size.width, size.height));
			double ratio = (double) size.width / size.height;
			if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
				continue;
			if (Math.abs(size.height - targetHeight) < minDiff) {
				optimalSize = size;
				minDiff = Math.abs(size.height - targetHeight);
			}
		}
		if (optimalSize == null) {
			minDiff = Double.MAX_VALUE;
			for (Size size : sizes) {
				if (Math.abs(size.height - targetHeight) < minDiff) {
					optimalSize = size;
					minDiff = Math.abs(size.height - targetHeight);
				}
			}
		}
		return optimalSize;
	}

	public void surfaceChanged(SurfaceHolder surfaceholder, int format, int w, int h) {
		holder = surfaceholder;
		width = w;
		height = h;
		startPreview(width, height);
		
		 Log.d(TAG, "Number of cameras"+Camera.getNumberOfCameras());
	}

	private void startPreview(int w, int h) {
		if (camera != null) {
			Camera.Parameters parameters = camera.getParameters();
			List<Size> sizes = parameters.getSupportedPreviewSizes();
			Log.d(TAG,"Sizes"+sizes);
			
			Size optimalSize = getOptimalPreviewSize(sizes, w, h);
			parameters.setPreviewSize(optimalSize.width, optimalSize.height);
			//parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_MACRO);
			Log.i(TAG, "optimalSize.width=" + optimalSize.width
					+ " optimalSize.height=" + optimalSize.height);
			Log.d(TAG,"Formats"+parameters.getSupportedPictureFormats());
			
			Log.d(TAG,"supported focus modes "+parameters.getSupportedFocusModes());
			

			camera.setParameters(parameters);
			parameters.set("rawsave-mode", "1");
			parameters.set("rawfname", Environment.getExternalStorageDirectory().getPath()+"test.raw");
			camera.startPreview();
			
			   Log.d(TAG,"Get number of cameras" + parameters.getSupportedPictureFormats());
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
		//	toggle();
			//Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE); 
            //startActivityForResult(cameraIntent, 1337);
//			int bufferSize = width * height * 3;
//			byte[] mPreviewBuffer = null;
//
//			// New preview buffer.
//			mPreviewBuffer = new byte[bufferSize + 4096];
//
//			// with buffer requires addbuffer.
//			camera.addCallbackBuffer(mPreviewBuffer);
//			camera.setPreviewCallbackWithBuffer(mCameraCallback);
//			break;
		default:
			break;
		}
		return true;
	}
	
	/**
	 * Converts YUV420 NV21 to RGB8888
	 * 
	 * @param data byte array on YUV420 NV21 format.
	 * @param width pixels width
	 * @param height pixels height
	 * @return a RGB8888 pixels int array. Where each int is a pixels ARGB. 
	 */
	public static int[] convertYUV420_NV21toRGB8888(byte [] data, int width, int height) {
	    int size = width*height;
	    int offset = size;
	    int[] pixels = new int[size];
	    int u, v, y1, y2, y3, y4;

	    // i percorre os Y and the final pixels
	    // k percorre os pixles U e V
	    for(int i=0, k=0; i < size; i+=2, k+=2) {
	        y1 = data[i  ]&0xff;
	        y2 = data[i+1]&0xff;
	        y3 = data[width+i  ]&0xff;
	        y4 = data[width+i+1]&0xff;

	        u = data[offset+k  ]&0xff;
	        v = data[offset+k+1]&0xff;
	        u = u-128;
	        v = v-128;

	        pixels[i  ] = convertYUVtoRGB(y1, u, v);
	        pixels[i+1] = convertYUVtoRGB(y2, u, v);
	        pixels[width+i  ] = convertYUVtoRGB(y3, u, v);
	        pixels[width+i+1] = convertYUVtoRGB(y4, u, v);

	        if (i!=0 && (i+2)%width==0)
	            i+=width;
	    }

	    return pixels;
	}

	private static int convertYUVtoRGB(int y, int u, int v) {
	    int r,g,b;

	    r = y + (int)1.402f*v;
	    g = y - (int)(0.344f*u +0.714f*v);
	    b = y + (int)1.772f*u;
	    r = r>255? 255 : r<0 ? 0 : r;
	    g = g>255? 255 : g<0 ? 0 : g;
	    b = b>255? 255 : b<0 ? 0 : b;
	    return 0xff000000 | (b<<16) | (g<<8) | r;
	}

	private final Camera.PreviewCallback mCameraCallback = new Camera.PreviewCallback() {
	public void onPreviewFrame(byte[] data, Camera c) {
	
	// INSERT YOUR CAMERA CAPTURING Fxns HERE
		
		
		Log.d(TAG, "ON Preview frame");
		img = new Mat(height, width, CvType.CV_8UC1);
		gray = new Mat(height, width, CvType.CV_8UC3);
		
		camera.stopPreview(); // similar to a normal camera capture app ..
		
		Mat rgba=new Mat(height,width,CvType.CV_8UC4);
		
//		 Mat colorImg=new Mat(height,width,CvType.cv_8)
		
		img.put(0, 0, data);		
	
		Imgproc.cvtColor(img, gray, Imgproc.COLOR_YUV420sp2RGB);
		Imgproc.cvtColor(img, rgba, Imgproc.COLOR_YUV420sp2RGBA);
		
		String pixvalue = String.valueOf(gray.get(300, 400)[0]);
		String pixval1 = String.valueOf(gray.get(300, 400+width/2)[0]); 
		
		Log.d(TAG, pixvalue);
		Log.d(TAG, pixval1);
		
		/// Saving the BitMap ..
		
		Camera.Parameters parameters = camera.getParameters();
        Size size = parameters.getPreviewSize();
        
        
        YuvImage image = new YuvImage(data,ImageFormat.NV21,c.getParameters().getPreviewSize().width,c.getParameters().getPreviewSize().height,null);
        Rect leftrectangle = new Rect();
        leftrectangle.bottom = size.height;
        leftrectangle.top = 0;
        leftrectangle.left = 0;
        leftrectangle.right = size.width/2;
        
        Rect rightrectangle=new Rect();
        rightrectangle.top=0;
        rightrectangle.left=size.width/2;
        rightrectangle.bottom=size.height;
        rightrectangle.right=size.width;
        
        Rect fullrectangle=new Rect();
		  fullrectangle.bottom = size.height;
		  fullrectangle.top = 0;
		  fullrectangle.left = 0;
		  fullrectangle.right = size.width;
		  
        OutputStream outputStream,outputStream2,outputStream3,outStream;
        
		try {
			outputStream = new FileOutputStream ( Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_left"+imageNumber+".jpg");
		    ByteArrayOutputStream out2 = new ByteArrayOutputStream();
	        image.compressToJpeg(leftrectangle, 100, outputStream);
	      	
	    	int[] mIntArray ;
	    	
	    	mIntArray=convertYUV420_NV21toRGB8888(data,size.width,size.height);
	    	
	    	//Initialize the bitmap, with the replaced color  
	    	Bitmap bmp = Bitmap.createBitmap(mIntArray, size.width, size.height, Bitmap.Config.ARGB_8888);  
	    	bmp.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
			outputStream2 = new FileOutputStream ( Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_right"+imageNumber+".jpg");
			
			image.compressToJpeg(rightrectangle, 100, outputStream2);
			
			outputStream3 = new FileOutputStream ( Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_full"+imageNumber+".jpg");
			
			image.compressToJpeg(fullrectangle, 100, outputStream3);
			
			Intent returnIntent = new Intent();
			returnIntent.putExtra("left_URI",Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_left"+imageNumber+".jpg");
			returnIntent.putExtra("right_URI",Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_right"+imageNumber+".jpg");
			returnIntent.putExtra("full_URI",Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_full"+imageNumber+".jpg");
			setResult(RESULT_OK, returnIntent);
			
			
	            
	        // ..... 
	        			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 

	 	Log.d("The Width is "+size.width,"The Height is "+size.height);
     finish();
	 	
	}
	};

/*	public void toggle() {
		is3Denabled = !is3Denabled;
		stopPreview();
		if (is3Denabled) {
			camera = getS3DCamera();
		}
		if (!is3Denabled) {
			camera = get2DCamera();
		}
		startPreview(width, height);
		if (is3Denabled) {
			text.setText("S3D");
		} else {
			text.setText("2D");
		}
	}*/

	private void enableS3D(boolean enable, Surface surface) {
		Log.i(TAG, "enableS3D(" + enable + ")");
		int mode = DisplaySetting.STEREOSCOPIC_3D_FORMAT_SIDE_BY_SIDE;
		if (!enable) {
			mode = DisplaySetting.STEREOSCOPIC_3D_FORMAT_OFF;
		} else {
			is3Denabled = true;
		}
		boolean formatResult = true;
		try {
			formatResult = DisplaySetting
					.setStereoscopic3DFormat(surface, mode);
		} catch (NoClassDefFoundError e) {
			android.util.Log.i(TAG,
					"class not found - S3D display not available");
			is3Denabled = false; 
		}
		Log.i(TAG, "return value:" + formatResult);
		if (!formatResult) {
			android.util.Log.i(TAG, "S3D format not supported");
			is3Denabled = false;
		}
	}
	public void takePicture(View v)
	{
		AutoFocusCallBackImpl autoFocusCallBack = new AutoFocusCallBackImpl();
	      camera.autoFocus(autoFocusCallBack);
	  	
//		camera.takePicture(shutterCallback, rawCallback, jpegCallback);
	}

	
	private class AutoFocusCallBackImpl implements Camera.AutoFocusCallback {
	    @Override
	    public void onAutoFocus(boolean success, Camera camera) {
	        boolean bIsAutoFocused = success; //update the flag used in onKeyDown()
	        Log.i(TAG, "Inside autofocus callback. autofocused="+success);
	        int bufferSize = width * height * 3;
			byte[] mPreviewBuffer = null;

			// New preview buffer.
			mPreviewBuffer = new byte[bufferSize + 4096];

			// with buffer requires addbuffer.
			camera.addCallbackBuffer(mPreviewBuffer);
			camera.setPreviewCallbackWithBuffer(mCameraCallback);
			
//			Toast.makeText(getBaseContext(), "Image Number :"+ imageNumber, Toast.LENGTH_SHORT).show();
			
			//camera.takePicture(shutterCallback, rawCallback, jpegCallback);
			
	        //play the autofocus sound
	       
	    }
	}
	
	 ShutterCallback shutterCallback = new ShutterCallback() {
		    public void onShutter() {
		      // Log.d(TAG, "onShutter'd");
		    }
		  };

		  PictureCallback rawCallback = new PictureCallback() {
		    public void onPictureTaken(byte[] data, Camera camera) {
		    	
		    	
		    	
		    	
		       Log.d(TAG, "onPictureTaken - raw");
		     ///  Log.d(TAG,"raw size"+data.length);
		       finish();
		    }
		  };

		  PictureCallback jpegCallback = new PictureCallback() {
		    public void onPictureTaken(byte[] data, Camera camera) {
		      FileOutputStream outStream = null;
		      try {
		        // Write to SD Card
		        String fileName = String.format( Environment.getExternalStorageDirectory().getPath()+"/test2.jpg", System.currentTimeMillis());
		        outStream = new FileOutputStream(fileName);
		        outStream.write(data);
		        outStream.close();
		        Log.d(TAG, "onPictureTaken - wrote bytes: " + data.length);


		      } catch (FileNotFoundException e) {
		        e.printStackTrace();
		      } catch (IOException e) {
		        e.printStackTrace();
		      } finally {
		      }
		      Log.d(TAG, "onPictureTaken - jpeg");
		    }
		  };
	public native void getDisparity(long matAddrRgba, long matAddrfinalImage);
	
}

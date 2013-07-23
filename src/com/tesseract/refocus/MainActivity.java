package com.tesseract.refocus;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;



import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.Intent;
import android.graphics.AvoidXfermode.Mode;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Paint.Style;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.v4.app.FragmentActivity;
import android.util.Base64;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.graphics.PorterDuff;

import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import android.view.MotionEvent;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

import org.opencv.core.Mat;
import org.opencv.highgui.*;

import com.facebook.FacebookAuthorizationException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.FacebookRequestError;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.model.GraphObject;
import com.facebook.model.GraphUser;

import com.facebook.widget.LoginButton;
import com.facebook.widget.ProfilePictureView;

public class MainActivity extends FragmentActivity {

		ImageButton shutterButton;
		Bitmap mImageBitmap;
		ImageView mImageView;
		private Mat mRgba;
		private Mat finalImage;
		String TAG="SimpleImageCapture";
		private File imgFile;
		private Bitmap myBitmap;
		boolean isImageClicked=false;
		static int loading_progress=0;
		ProgressBar mProgress;
		private Handler mHandler = new Handler();
		ProgressDialog progress;
		float converted_xcoord,converted_ycoord;
		File leftimgFile;
	
		Bitmap imageViewBitmap;
			
		List<String> fileList = new ArrayList<String>();
		
		private Button postPhotoButton;
		
		//// Facebook Variables .
		private static final List<String> PERMISSIONS = Arrays.asList("publish_actions");
		private final String PENDING_ACTION_BUNDLE_KEY = "com.facebook.samples.hellofacebook:PendingAction";

		private PendingAction pendingAction = PendingAction.NONE;
	    private ViewGroup controlsContainer;
	    private GraphUser user;
	    private LoginButton loginButton;
	    private ProfilePictureView profilePictureView;
	   

	    private enum PendingAction {
	        NONE,
	        POST_PHOTO,
	        POST_STATUS_UPDATE
	    }
	    private UiLifecycleHelper uiHelper;

	    private Session.StatusCallback callback = new Session.StatusCallback() {
	        @Override
	        public void call(Session session, SessionState state, Exception exception) {
	           ;// onSessionStateChange(session, state, exception);
	        }
	    };

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		setContentView(R.layout.activity_main);
		
		shutterButton=(ImageButton) findViewById(R.id.shutterButton);
		mImageView=(ImageView) findViewById(R.id.imageView1);
		
	
		
		shutterButton.setOnClickListener((android.view.View.OnClickListener) new MyOnClickListener());
		mImageView.setOnTouchListener(new TouchListener());
		
		profilePictureView = (ProfilePictureView) findViewById(R.id.profilePicture);
		
		progress= new ProgressDialog(this);
		
	
        File sdCard = Environment.getExternalStorageDirectory();
        File dir = new File (sdCard.getAbsolutePath()+"/Tesseract/Refocus");
        dir.mkdirs();
       
        System.loadLibrary("disp_img");
        
        // Facebook Stuff ..
        
        postPhotoButton=(Button) findViewById(R.id.postpic);
		 postPhotoButton.setOnClickListener(new View.OnClickListener() {
	            public void onClick(View view) {
	                onClickPostPhoto();
	            }
	        });
        
        uiHelper = new UiLifecycleHelper(this, callback);
        uiHelper.onCreate(savedInstanceState);

        
        loginButton = (LoginButton) findViewById(R.id.login_button);
        loginButton.setUserInfoChangedCallback(new LoginButton.UserInfoChangedCallback() {
            @Override
            public void onUserInfoFetched(GraphUser user) {
               MainActivity.this.user = user;
                updateUI();
                // It's possible that we were waiting for this.user to be populated in order to post a
                // status update.
                handlePendingAction();
            }
        });
        
    	
	  
        
	}
	
	

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	
	
	 @Override
	    public void onPause() {
	        super.onPause();
	        uiHelper.onPause();
	    }

	    @Override
	    public void onDestroy() {
	        super.onDestroy();
	        uiHelper.onDestroy();
	    }

	    private void onSessionStateChange(Session session, SessionState state, Exception exception) {
	        if (pendingAction != PendingAction.NONE &&
	                (exception instanceof FacebookOperationCanceledException ||
	                exception instanceof FacebookAuthorizationException)) {
	                new AlertDialog.Builder(MainActivity.this)
	                    .setTitle(R.string.cancelled)
	                    .setMessage(R.string.permission_not_granted)
	                    .setPositiveButton(R.string.ok, null)
	                    .show();
	            pendingAction = PendingAction.NONE;
	        } else if (state == SessionState.OPENED_TOKEN_UPDATED) {
	            handlePendingAction();
	        }
	        updateUI();
	    }
	
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
	    super.onActivityResult(requestCode, resultCode, data);
	    uiHelper.onActivityResult(requestCode, resultCode, data);
	    
	    Log.d("Request code:"+requestCode,"tea");
	    
	    switch(requestCode){
	    case 1337:
	        if(resultCode==RESULT_OK)
	        {
	        	loading_progress=0;
	        	// @Jay : Change this to part to full_URI
		    	imgFile = new  File((String) data.getExtras().get("full_URI"));
		    	leftimgFile = new File((String) data.getExtras().get("left_URI"));
		    	Log.d("full_URI","url="+imgFile);
		    	
		    	isImageClicked=true;
		    	
		    	if(imgFile.exists())
		    	{
			    	myBitmap = BitmapFactory.decodeFile(leftimgFile.getAbsolutePath());
			    	imageViewBitmap = Bitmap.createScaledBitmap(myBitmap, mImageView.getWidth(),mImageView.getHeight(), true);
				    mImageView.setImageBitmap(imageViewBitmap);
				
			    }
	        }
	    }
	}
	
	  
		class MyOnClickListener implements View.OnClickListener{

			public void onClick(View v) {
				
				    Intent camera_intent=new Intent(getBaseContext(),CustomClickActivity.class);
				    startActivityForResult(camera_intent,1337);
			
			}
		     };
	

    

	     
	     class TouchListener implements View.OnTouchListener{

	 		@Override
	 		public boolean onTouch(View v, MotionEvent event) {

	 			if(event.getAction() == MotionEvent.ACTION_DOWN&&isImageClicked) 
	 			{
	 				
	 				// Image has been clicked already,so u should not be able to touch the ImageView again ..
	 				isImageClicked=false;
	 				
	 				Log.d(TAG,"X ="+(event.getRawX()-mImageView.getLeft())+"  Y= "+(event.getRawY()-mImageView.getTop())); // For landscape orientation,i.e max val of x is 800 and y max value is 480 ..

	 				
	 				
	 				// Pass these to the JNI function
	 				converted_xcoord=(event.getRawX()-mImageView.getLeft());
	 				converted_ycoord=(event.getRawY()-mImageView.getTop());
	 				
	 				
	 				converted_xcoord=(converted_xcoord/mImageView.getWidth())*640;
	 				converted_ycoord=(converted_ycoord/mImageView.getHeight())*720;
	 				// 
	 				
	 				
	 				Log.d(TAG, "converted");
	 				mRgba = new Mat();
			    	finalImage = new Mat();
				    	
			    	Log.d(TAG,"Initialized Mat");
			    	Log.d(TAG,"progress"+loading_progress);
			    	
			    	mRgba = Highgui.imread(imgFile.getAbsolutePath());
			    
			    	Log.d(TAG, "Image loaded");
			    	
			    	//mRgba = Highgui.imread(filename);
			    	
			    	
				    new ComputeDisparity().execute("");
				   
	 			}
	 			else 
	 			{
	 				
	 				Toast.makeText(getBaseContext(), "Please click an image", Toast.LENGTH_SHORT).show();
	 			
	 			}
	 			return false;
	 		}

	 	  }

    public native void getDisparity(long matAddrRgba, long matAddrfinalImage, int ji1, int ji2);

    private class ComputeDisparity extends AsyncTask<String, Void, String> {
    	
        @Override
        protected String doInBackground(String... params) {
        	// getDisparity(mRgba.getNativeObjAddr(), finalImage.getNativeObjAddr(), (int)converted_xcoord, (int)converted_ycoord);
// Commenting out for now ..
              return "";
        }      

        @Override
        protected void onPostExecute(String result) {
           
             progress.dismiss();
             
             Log.d(TAG,"blah");
             
             String colVal = String.valueOf(finalImage.cols());
	    	 Log.d("Cols", colVal);
	    	 Highgui.imwrite(imgFile.getAbsolutePath(), finalImage);
	    	 myBitmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath());
		     mImageView.setImageBitmap(myBitmap);
		    
		       Log.d("done","done");
             
             // txt.setText(result);
             
              //might want to change "executed" for the returned string passed into onPostExecute() but that is upto you
        }

        @Override
        protected void onPreExecute() {
        	   progress.setTitle("Processing Image");
               progress.setMessage("Please wait while we process your image ...");
               progress.show();
        }

        @Override
        protected void onProgressUpdate(Void... values) {
        }
        
  }   
    
////  Facebook Functions ...
    
    private void updateUI() {
        Session session = Session.getActiveSession();
        
        boolean enableButtons = (session != null && session.isOpened());

        postPhotoButton.setEnabled(enableButtons);

        if (enableButtons && user != null) {
            profilePictureView.setProfileId(user.getId());

        } else {
            profilePictureView.setProfileId(null);

        }
    }

    @SuppressWarnings("incomplete-switch")
    private void handlePendingAction() {
        PendingAction previouslyPendingAction = pendingAction;
        // These actions may re-set pendingAction if they are still pending, but we assume they
        // will succeed.
        pendingAction = PendingAction.NONE;

        switch (previouslyPendingAction) {
            case POST_PHOTO:
                postPhoto();
                break;
            
        }
    }
    
    
    private boolean hasPublishPermission() {
        Session session = Session.getActiveSession();
        return session != null && session.getPermissions().contains("publish_actions");
    }

    private void performPublish(PendingAction action) {
        Session session = Session.getActiveSession();
        if (session != null) {
            pendingAction = action;
            if (hasPublishPermission()) {
                // We can do the action right away.
                handlePendingAction();
            } else {
                // We need to get new permissions, then complete the action when we get called back.
                session.requestNewPublishPermissions(new Session.NewPermissionsRequest(this, PERMISSIONS));
            }
        }
    }
    
    private void onClickPostPhoto() {
        performPublish(PendingAction.POST_PHOTO);
    }

    private void postPhoto() {
        if (hasPublishPermission()&&isImageClicked) {
          //  Bitmap image = BitmapFactory.decodeResource(this.getResources(), R.drawable.icon);
            Request request = Request.newUploadPhotoRequest(Session.getActiveSession(), ((BitmapDrawable)mImageView.getDrawable()).getBitmap(), new Request.Callback() {
                @Override
                public void onCompleted(Response response) {
                	
                    showPublishResult(getString(R.string.photo_post), response.getGraphObject(), response.getError());
                }
            });
            request.executeAsync();
        } else {
            pendingAction = PendingAction.POST_PHOTO;
        }
    }
    
    private interface GraphObjectWithId extends GraphObject {
        String getId();
    }
    
    private void showPublishResult(String message, GraphObject result, FacebookRequestError error) {
        String title = null;
        String alertMessage = null;
        if (error == null) {
            title = getString(R.string.success);
            String id = result.cast(GraphObjectWithId.class).getId();
            alertMessage = getString(R.string.successfully_posted_post, message, id);
        } else {
            title = getString(R.string.error);
            alertMessage = error.getErrorMessage();
        }

        new AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(alertMessage)
                .setPositiveButton(R.string.ok, null)
                .show();
    }


    
    
    
    public void printHashKey() {

        try {
            PackageInfo info = getPackageManager().getPackageInfo("com.facebook.samples.hellofacebook",
                    PackageManager.GET_SIGNATURES);
            for (Signature signature : info.signatures) {
                MessageDigest md = MessageDigest.getInstance("SHA");
                md.update(signature.toByteArray());
                Log.d("TEMPTAGHASH KEY:",
                        Base64.encodeToString(md.digest(), Base64.DEFAULT));
            }
        } catch (NameNotFoundException e) {

        } catch (NoSuchAlgorithmException e) {

        }

    }
    
    
    
    
    
    
}



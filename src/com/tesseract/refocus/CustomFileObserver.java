package com.tesseract.refocus;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.CompressFormat;
import android.os.Environment;
import android.os.FileObserver;
import android.util.Log;

public class CustomFileObserver 
{

	Context activityContext;

	CustomFileObserver(Context context)
	{
		activityContext=context;
		initializeFileObserver();
	}

	public void initializeFileObserver()
	{
Log.d("com.tesseract.refocus","Initializing");

		FileObserver observer = new FileObserver(android.os.Environment.getExternalStorageDirectory().toString() + "/DCIM/100MEDIA/") { // set up a file observer to watch this directory on sd card
			@SuppressWarnings("deprecation")
			@Override
			public void onEvent(int event, String file) {
				Log.d("fired","fired"+" Event:"+event);

				PackageManager pm = activityContext.getPackageManager();

				if(event == FileObserver.CLOSE_WRITE && !file.equals(".probe")){ // check if its a "create" and not equal to .probe because thats created every time camera is launched
					{
						Log.d("fds", "File created [" + android.os.Environment.getExternalStorageDirectory().toString() + "/DCIM/100MEDIA/" + file + "]");
		//				i++;
			//			Log.d("i value","i ="+i);
						try
						{

							//int t=splitmpointojpg(Environment.getExternalStorageDirectory().toString() + "/DCIM/100MEDIA/" + file);

							String jpsfilename=Environment.getExternalStorageDirectory().toString() + "/DCIM/100MEDIA/" + file;
							String jpgfilename=jpsfilename.substring(0,jpsfilename.length()-1)+"g";

							Log.d("File name","name "+jpsfilename);

							File from      = new File("",jpsfilename);
							File to        = new File("",jpgfilename);
							from.renameTo(to);

							Log.i("From path is", from.toString());
							Log.i("To path is", to.toString());

							BitmapFactory.Options option = new BitmapFactory.Options();
							option.inSampleSize =2;
							option.inScaled = true;

							Bitmap fullsize_bitmap=BitmapFactory.decodeFile(jpgfilename,option);

							Bitmap left_img=Bitmap.createBitmap(fullsize_bitmap, 0,0,fullsize_bitmap.getWidth()/2,fullsize_bitmap.getHeight());

							Bitmap right_img=Bitmap.createBitmap(fullsize_bitmap,fullsize_bitmap.getWidth()/2,0,fullsize_bitmap.getWidth()/2,fullsize_bitmap.getHeight());

							OutputStream outputStream,outputStream2;

							try {
								outputStream = new FileOutputStream ( Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_left.jpg");
								left_img.compress(CompressFormat.JPEG, 100, outputStream);

								outputStream2 = new FileOutputStream ( Environment.getExternalStorageDirectory().getPath()+"/SimpleImageCapture/img_right.jpg");
								right_img.compress(CompressFormat.JPEG, 100, outputStream2);


							} catch (FileNotFoundException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} 

							Log.d("yp", "blah");


							Intent it = pm.getLaunchIntentForPackage("com.tesseract.refocus");




							if (null != it)
								activityContext.startActivity(it);

						}

						catch (ActivityNotFoundException e)
						{//oops, no such application
						}


					}// fileSaved = "New photo Saved: " + file;
				}
			}
		};

		observer.startWatching(); // start the observer
	}

}

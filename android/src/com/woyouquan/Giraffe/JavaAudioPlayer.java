package com.woyouquan.Giraffe;

import java.io.IOException;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.SoundPool;
import android.media.SoundPool.OnLoadCompleteListener;

public class JavaAudioPlayer implements OnPreparedListener, OnCompletionListener, OnLoadCompleteListener{
	
	private Context mContext;
	private MediaPlayer mPlayer;
	private boolean mPlaying;
	private boolean mPlayerPrepared;
	private boolean mAutoplay;
	private boolean mLoop;
	private float mVolume;
	private String mSource;
	
	private SoundPool mSoundPool;
	private int mSoundID;
	private int mSoundStreamID;
	
	public JavaAudioPlayer(Context context) {
		mContext = context;
		mPlayer = new MediaPlayer();
    	mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
    	
		mSoundPool = new SoundPool(16, AudioManager.STREAM_MUSIC, 0);
		mSoundPool.setOnLoadCompleteListener(this);
    	
    	mPlayerPrepared = false;
    	mPlaying = false;
    	mAutoplay = true;
    	mLoop = false; 
    	mVolume = 1.0f;
    	
    	mSoundID = 0;
    	mSoundStreamID = 0;
	}

    public void setSource(String path) {
    	
    	if( path.startsWith("assets/")) {
    		path = path.substring(7);
    	}
    	
    	mSource = path;
    	mPlayer.reset();

		mSoundPool.stop(mSoundStreamID);
		mSoundStreamID = 0;
		mSoundPool.unload(mSoundID);
		mSoundID = 0;
    	
    	try {
    		if( path.startsWith("http://") ){
    			mPlayer.setDataSource(mSource);
    		}else{
    			AssetFileDescriptor fd = mContext.getResources().getAssets().openFd(mSource);
    			mPlayer.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
    		}
			
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalStateException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
    	finally
    	{
    		mPlayer.setOnPreparedListener(this);
    		mPlayer.prepareAsync();
    	}
    }
    
    public void setLoop(boolean loop) {
    	mLoop = loop;
		if( mSoundStreamID > 0 )
		{
	    	int loopMode = mLoop ? -1 : 0;
	    	mSoundPool.setLoop(mSoundStreamID, loopMode);
		}
		else if( mPlayerPrepared )
		{
			mPlayer.pause();
			mPlayer.setLooping(mLoop);
		}
    }
    
    public void setAutoplay(boolean autoplay) {
    	mAutoplay = autoplay;
    }
    
    public float getVolume(){
    	AudioManager mgr = (AudioManager)mContext.getSystemService(Context.AUDIO_SERVICE);
        
    	float streamVolumeCurrent = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);   
        float streamVolumeMax = mgr.getStreamMaxVolume(AudioManager.STREAM_MUSIC);      
        float volume = streamVolumeCurrent/streamVolumeMax;
        
        return volume;
    }
    
	public void onPrepared(MediaPlayer mp) {
		
		if( mp.getDuration() < 5000 )
		{
			// 短音
			try {
				mSoundID = mSoundPool.load(mContext.getResources().getAssets().openFd(mSource), 1);
			} catch (IOException e) {
				e.printStackTrace();
			}
			mp.reset();
			return;
		}
		else
		{
			mPlayerPrepared = true;
			if( mAutoplay || mPlaying )
			{
				mp.start();
				mPlaying = true;
			}
		}
	}
	
	public void onCompletion(MediaPlayer mp) {
	}
	
	public void play() {
		mPlaying = true;
		if( mSoundID > 0 )
		{
			int loopMode = mLoop ? -1 : 0;
			mSoundStreamID = mSoundPool.play(mSoundID, mVolume, mVolume, 1, loopMode, 1f);
		}
		else if( mPlayerPrepared )
		{
			mPlayer.start();
		}
	}
	
	public void pause() {
		mPlaying = false;
		if( mSoundStreamID > 0 )
		{
			mSoundPool.pause(mSoundStreamID);
		}
		else if( mPlayerPrepared )
		{
			mPlayer.pause();
		}
	}
	
	public void setVolume(float volume) {
		mVolume = volume;
		if( mSoundStreamID > 0 )
		{
			mSoundPool.setVolume(mSoundStreamID, volume, volume);
		}
		else if( mPlayerPrepared )
		{
			mPlayer.setVolume(volume, volume);
		}
	}
	
	public void release() {
		mPlayer.release();
		mSoundPool.stop(mSoundStreamID);
		mSoundPool.unload(mSoundID);
		mSoundPool.release();
	}

	public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
		setLoop(mLoop);
		setVolume(mVolume);
		
		if( mPlaying || mAutoplay )
		{
			play();
		}
	}
}

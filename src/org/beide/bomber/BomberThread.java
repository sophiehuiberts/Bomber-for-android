/*
 * Copyright (C) Kees Huiberts <itissohardtothinkofagoodemail@gmail.com> 2012
 * 
 * Licensed under the GPL v3
 * 
 * Distributed without any warranty, including those about merchantability
 * or fitness for a particular purpose.
 * If you did not receive the license along with this file, you can find it
 * at <http://www.gnu.org/licenses/>.
 */

package org.beide.bomber;

import android.content.Context;
import android.content.res.Resources;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.preference.PreferenceManager;
import android.view.View;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

public class BomberThread extends Thread implements View.OnTouchListener, View.OnKeyListener {
	
	public String TAG = "Bomber";
	
	Context context;
	Resources res;
	SharedPreferences prefs;
	
	int[][] levels = {
		{0,0,1,2,3,4,5,4,3,2,1,0,0},
		{0,5,5,5,5,5,5,5,5,5,5,0,0},
		{0,9,8,7,6,5,4,3,2,1,0,0,0},
		{0,9,9,9,9,9,9,9,0,9,9,0,0}
	};
	
	int UNITS_HORIZONTAL = 12;
	int UNITS_VERTICAL = 18;
	
	// Size of bomb
	int BOMB_RADIUS = 12;
	
	int PLANE_START_HEIGHT = 17;
	
	SurfaceHolder holder;
	
	Paint textpaint, paint;
	Paint roundrectpaint, bigtextpaint;
	RectF rect;
	
	Bitmap plane, tower, bomb, background;
	
	boolean running = true;
	boolean paused;
	
	// Dummy values
	int canvaswidth = 500;
	int canvasheight = 500;
	
	// Size of one unit
	int unitheight;
	int unitwidth;
	
	long previoustick;
	
	int score;
	int level;
	int lives, maxlives;
	float bombX, bombY, bombgravity, bombspeed;
	float planeX, planeY, velocity, planegravity, planestart, planespeed;
	int[] towers;
	
	public BomberThread(SurfaceHolder hold, Context c) {
		holder = hold;
		context = c;
		
		res = c.getResources();
		
		textpaint = new Paint();
		textpaint.setColor(Color.BLACK);
		
		paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		
		// Colors are from KDE Bomber
		roundrectpaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		roundrectpaint.setARGB(188, 202, 222, 155);
		
		bigtextpaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		bigtextpaint.setTextSize((float) 30);
		
		rect = new RectF(unitwidth, unitheight, canvaswidth - unitwidth, canvasheight - unitheight);
		
		prefs = PreferenceManager.getDefaultSharedPreferences(c);
	}
	
	public void setRunning(boolean run) {
		running = run;
		previoustick = System.nanoTime();
	}
	
	public void setPaused(boolean pause) {
		paused = pause;
		
		if(!pause && paused) {
			getSettings();
			previoustick = System.nanoTime();
		}
	}
	
	public void getSettings() {
		bombspeed = Float.valueOf(prefs.getString("bombspeed", "0.45"));
		planespeed = Float.valueOf(prefs.getString("planespeed", "0.45"));
		maxlives = Integer.parseInt(prefs.getString("lives", "3"));
	}
	
	
	public void surfaceSize(int width, int height) {
		synchronized(holder) {
			getSettings();
			
			canvasheight = height;
			canvaswidth = width;
			unitwidth = width / UNITS_HORIZONTAL;
			unitheight = height / UNITS_VERTICAL;
			Log.v(TAG, "Canvas is " + canvaswidth + "x" + canvasheight);
			Log.v(TAG, "Units are " + unitwidth + "x" + unitheight);
			planestart = PLANE_START_HEIGHT * unitheight;
			
			bombgravity = canvasheight * bombspeed;
			velocity = canvaswidth * planespeed;
			planegravity = unitheight;
			
			plane = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(res, R.drawable.plane),
																				(int) (unitwidth), (int) (unitheight), true);
			tower = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(res, R.drawable.tower),
																				unitwidth, unitheight, true);
			bomb = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(res, R.drawable.bomb),
																			 BOMB_RADIUS * 2, BOMB_RADIUS * 2, true);
			
			rect = new RectF(unitwidth, unitheight, canvaswidth - unitwidth, canvasheight - unitheight);
			
			background = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(res, R.drawable.landscape),
																						 width, height, true);
			
		}
	}
	
	public void restart() {
		synchronized(holder) {
			score = 0;
			initlevel(0);
		}
	}
	
	public void initlevel(int lvl) {
		Log.v(TAG, "initing level " + lvl);
		synchronized(holder) {
			
			if(lvl == 0) {
				lives = maxlives;
			}
			
			level = lvl;
			bombY = 0;
			planeY = planestart;
			planeX = 0;
			towers = (int[]) levels[lvl].clone();
			bombgravity = canvasheight * bombspeed;
			velocity = canvaswidth * planespeed;
			planegravity = unitheight;
		}
	}
	
	public void draw(Canvas canvas) {
		canvas.drawBitmap(background, (float) 0, (float) 0, paint);
		
		// Draw the towers
		for(int i = 0; i < towers.length; i++) {
			for(int j = 0; j < towers[i]; j++) {
				canvas.drawBitmap(tower, (float) i * unitwidth,
													(float) canvasheight - (j + 1) * unitheight, paint);
			}
		}
		
		// Draw the plane
		canvas.drawBitmap(plane, planeX - unitwidth,
											canvasheight - planeY, paint);
		
		if(bombY > 0) {
			// Draw the bomb
			canvas.drawBitmap(bomb, bombX - BOMB_RADIUS, canvasheight - bombY - BOMB_RADIUS, paint);
		}
		
		canvas.drawText(res.getString(R.string.level) + level, (float) 0, unitheight, textpaint);
		canvas.drawText(res.getString(R.string.score) + score, (float) 0, unitheight * 2, textpaint);
		canvas.drawText(res.getString(R.string.lives) + lives, (float) 0, unitheight * 3, textpaint);
	}
	
	public void update() {
		
		synchronized(holder) {
			long tick = System.nanoTime();
			// Elapsed time is in seconds
			double elapsedtime = (tick - previoustick)/1000000;
			elapsedtime /= 1000;
			previoustick = tick;
			
			if(bombY != 0) {
				bombY -= bombgravity * elapsedtime;
				if(towers[(int) bombX / unitwidth] * unitheight >= bombY || bombY >= canvasheight) {
					towers[(int) bombX / unitwidth]--;
					
					score++;
					bombY = 0;
				}
			}
			
			planeX += velocity * elapsedtime;
			if(planeX >= canvaswidth) {
				planeX = 0;
				planeY -= planegravity;
			}
			
			// Oops, we hit a tower
			if(towers[(int) planeX / unitwidth] * unitheight >= planeY) {
				planeY = planestart;
				planeX = 0;
				lives--;
			}
			
			if(lives < 0) {
				
				return;
			}
			
			// If there are towers, return
			for(int i = 0; i < towers.length; i++) {
				if(towers[i] > 0) {
					return;
				}
			}
			
			// There are no towers, so we will level up
			initlevel(level + 1);
		}
	}
	
	public void gameover(Canvas canvas) {
		Log.i(TAG, "Game over! Score: " + score);
		
		setPaused(true);
		
		int highscore = prefs.getInt("highscore", 0);
		
		if(score > highscore) {
			SharedPreferences.Editor edit = prefs.edit();
			edit.putInt("highscore", score);
			edit.commit();
		}
		
		canvas.drawRoundRect(rect, (float) unitheight, (float) unitheight, roundrectpaint);
		
		canvas.drawText(res.getString(R.string.gameover), (float) 100, 100, bigtextpaint);
		canvas.drawText(res.getString(R.string.highscore) + highscore, (float) 100, 150, textpaint);
		canvas.drawText(res.getString(R.string.score) + score, (float) 100, 170, textpaint);
		
		score = 0;
	}
	
	/**
	 * Called when BomberView is touched. Dispatches click() on touch down.
	 */
	public boolean onTouch(View v, MotionEvent event) {
		if(event.getAction() == MotionEvent.ACTION_DOWN) {
			click();
		}
		
		return true;
	}
	
	/**
	 * Called when a key is pressed on BomberView. Dispatches click() on button down.
	 */
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		if(event.getAction() == KeyEvent.ACTION_DOWN && event.getAction() == 0) {
			click();
		}
		
		return true;
	}
	
	/**
	 * Called by onTouch and onKey. Continues on gameover and drops bombs.
	 */
	public void click() {
		if(lives < 0) {
			initlevel(0);
			setPaused(false);
			return;
		}
		
		// If the bomb is available, drop it
		if(bombY == 0) {
			bombY = planeY - unitheight;
			bombX = planeX - (planeX % unitwidth) + unitwidth / 2;
		}
	}
	
	public void run() {
		restart();
		while(running) {
			Canvas c = null;
			try {
				c = holder.lockCanvas(null);
				if(!paused) {
					synchronized(holder) {
						if(lives < 0) {
							draw(c);
							gameover(c);
						} else {
							update();
							draw(c);
						}
					}
				} else {
					if(lives < 0) {
						draw(c);
						gameover(c);
					}
				}
			} catch(Exception e) {
				e.printStackTrace();
			} finally {
				if(c != null) {
					holder.unlockCanvasAndPost(c);
				} else {
					Log.e(TAG, "Canvas is null");
				}
			}
		}
	}
}
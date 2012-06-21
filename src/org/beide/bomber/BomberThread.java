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

import java.util.Random;

public class BomberThread extends Thread implements View.OnTouchListener, View.OnKeyListener {
	
	public String TAG = "Bomber";
	
	Context context;
	Resources res;
	SharedPreferences prefs;
	
	Random random;
	
	final static int UNITS_HORIZONTAL = 12;
	final static int UNITS_VERTICAL = 18;
	
	// Size of bomb
	final static int BOMB_RADIUS = 12;
	
	final static int PLANE_START_HEIGHT = 17;
	
	final static int STATE_NEWGAME = 0;
	final static int STATE_PLAYING = 1;
	final static int STATE_GAMEOVER = 2;
	final static int STATE_PAUSED = 3;
	final static int STATE_LEVELUP = 4;
	
	SurfaceHolder holder;
	
	Paint smalltextpaint, medtextpaint, bigtextpaint;
	Paint roundrectpaint, paint;
	RectF rect;
	
	Bitmap plane, tower, bomb, background;
	
	boolean running = true;
	int state = STATE_NEWGAME;
	
	// Dummy values
	int canvaswidth = 500;
	int canvasheight = 500;
	
	// Size of one unit
	int unitheight;
	int unitwidth;
	
	long previoustick;
	
	int score, highscore;
	int level;
	int lives, maxlives;
	float bombX, bombY, bombgravity, bombspeed;
	float planestart, planestartspeed;
	float planeX, planeY, velocity, planegravity, planespeed, planeaccelleration;
	int[] towers;
	
	public BomberThread(SurfaceHolder hold, Context c) {
		holder = hold;
		context = c;
		
		random = new Random();
		
		res = c.getResources();
		
		smalltextpaint = new Paint();
		smalltextpaint.setColor(Color.BLACK);
		smalltextpaint.setTextSize((float) 16);
		
		medtextpaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		medtextpaint.setColor(Color.BLACK);
		medtextpaint.setTextSize((float) 22);
		
		bigtextpaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		bigtextpaint.setColor(Color.BLACK);
		bigtextpaint.setTextSize((float) 38);
		
		paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		
		// Colors are from KDE Bomber
		roundrectpaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		roundrectpaint.setARGB(188, 202, 222, 155);
		
		// Changes in this line should also be done in its other occurence
		rect = new RectF(unitwidth, unitheight, canvaswidth - unitwidth, canvasheight - unitheight);
		
		prefs = PreferenceManager.getDefaultSharedPreferences(c);
	}
	
	/**
	 * Only has effect when the game is paused or played.
	 */
	public void setRunning(boolean run) {
		running = run;
		previoustick = System.nanoTime();
	}
	
	/**
	 * Only has effect when the game is paused or played.
	 */
	public void setPaused(boolean pause) {
		
		if(!pause && state == STATE_PAUSED) {
			previoustick = System.nanoTime();
			state = STATE_PLAYING;
		}
		
		if(pause && state == STATE_PLAYING) {
			state = STATE_PAUSED;
		}
	}
	
	/**
	 * Refreshes settings.
	 */
	public void getSettings() {
		bombspeed = Float.valueOf(prefs.getString("bombspeed", "0.45"));
		planestartspeed = Float.valueOf(prefs.getString("planespeed", "0.45"));
		planeaccelleration = Float.valueOf(prefs.getString("planeaccelleration", "0.0028"));
		maxlives = Integer.parseInt(prefs.getString("lives", "3"));
	}
	
	/**
	 * Scales the units appropriately for the screen-size.
	 */
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
			
			// Changes in this line should also be done in its other occurence
			rect = new RectF(unitwidth, unitheight, canvaswidth - unitwidth, canvasheight - unitheight);
			
			background = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(res, R.drawable.landscape),
																						 width, height, true);
			
		}
	}
	
	/**
	 * Restarts the game.
	 */
	public void restart() {
		synchronized(holder) {
			score = 0;
			initlevel(0);
		}
	}
	
	/**
	 * Initializes the level given as parameter.
	 */
	public void initlevel(int lvl) {
		Log.v(TAG, "initing level " + lvl);
		synchronized(holder) {
			
			if(lvl == 0) {
				lives = maxlives;
				score = 0;
				planespeed = planestartspeed;
			} else {
				planespeed += planeaccelleration;
			}
			
			level = lvl;
			bombY = 0;
			planeY = planestart;
			planeX = 0;
			bombgravity = canvasheight * bombspeed;
			velocity = canvaswidth * planespeed;
			planegravity = unitheight;
			
			towers = levelGen(lvl);
		}
	}
	
	/**
	 * Level generator.
	 */
	public int[] levelGen(int lvl) {
		// Container
		int[] generated = new int[UNITS_HORIZONTAL];
		
		// Baseline
		int min = level;
		int max = level + 3;
		
		// Sanitize
		if(min < 3) {
			min = 3;
		}
		if(max < 5) {
			max = 5;
		}
		
		// These 2 rules are not in the original game
		if(max >= PLANE_START_HEIGHT) {
			max = PLANE_START_HEIGHT - 1;
		}
		if(min > max) {
			min = max;
		}
		
		// Keep the first and last columns open
		for(int i = 1; i < UNITS_HORIZONTAL - 1; i++) {
			
			// The + 1 and - 0.5 are to make the min and max equally probable to the other heights
			generated[i] = (int) (random.nextFloat() * (max - min + 1) + min - 0.5);
		}
		
		return generated;
	}
	
	/**
	 * Draws the background, the plane, the bomb and the buildings.
	 */
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
		
		canvas.drawText(res.getString(R.string.level) + (level + 1), (float) 0, unitheight, smalltextpaint);
		canvas.drawText(res.getString(R.string.score) + score, (float) 0, unitheight * 2, smalltextpaint);
		canvas.drawText(res.getString(R.string.lives) + lives, (float) 0, unitheight * 3, smalltextpaint);
	}
	
	/**
	 * Does 1 tick of the game. All the primary game logic.
	 */
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
					
					// If something is destroyed, get points
					if(towers[(int) bombX / unitwidth] > 0) {
						score++;
						towers[(int) bombX / unitwidth]--;
					}
					
					bombY = 0;
				}
			}
			
			planeX += velocity * elapsedtime;
			if(planeX >= canvaswidth) {
				planeX = planeX % canvaswidth;
				planeY -= planegravity;
			}
			
			// Oops, we hit a tower
			if(towers[(int) planeX / unitwidth] * unitheight >= planeY) {
				// Lose 1 live
				lives--;
				
				// Destroy the block we hit
				towers[(int) planeX / unitwidth]--;
				
				// Reset plane to start-position
				planeY = planestart;
				planeX = 0;
			}
			
			if(lives < 0) {
				Log.i(TAG, "Game over! Score: " + score);
				
				state = STATE_GAMEOVER;
				
				highscore = prefs.getInt("highscore", 0);
				
				if(score > highscore) {
					SharedPreferences.Editor edit = prefs.edit();
					edit.putInt("highscore", score);
					edit.commit();
				}
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
			state = STATE_LEVELUP;
		}
	}
	
	/**
	 * Draws the orange rectangle with the "Game Over" text.
	 * Use this after draw.)
	 */
	public void drawgameover(Canvas canvas) {
		
		canvas.drawRoundRect(rect, (float) unitheight, (float) unitheight, roundrectpaint);
		
		canvas.drawText(res.getString(R.string.gameover), (float) 100, 100, bigtextpaint);
		canvas.drawText(res.getString(R.string.highscore) + highscore, (float) 100, 150, medtextpaint);
		canvas.drawText(res.getString(R.string.score) + score, (float) 100, 170, medtextpaint);
	}
	
	/**
	 * Draws the orange rectangle with the "New Game" text.
	 * Use this after draw.)
	 */
	public void drawnewgame(Canvas canvas) {
		
		canvas.drawRoundRect(rect, (float) unitheight, (float) unitheight, roundrectpaint);
		
		canvas.drawText(res.getString(R.string.welcome), (float) 100, 100, bigtextpaint);
		canvas.drawText(res.getString(R.string.clicktostart), (float) 100, 200, bigtextpaint);
	}
	
	/**
	 * Draws the orange rectangle with the "Level up" text.
	 * Use this after draw.)
	 */
	public void drawlevelup(Canvas canvas) {
		
		canvas.drawRoundRect(rect, (float) unitheight, (float) unitheight, roundrectpaint);
		
		canvas.drawText(res.getString(R.string.clearedlevel)  + " " + level, (float) 100, 100, medtextpaint);
		canvas.drawText(res.getString(R.string.ontolevel) + " " + (level + 1), (float) 100, 150, medtextpaint);
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
		}
		
		if(state != STATE_PLAYING) {
			state = STATE_PLAYING;
			previoustick = System.nanoTime();
			return;
		}
		
		// If the bomb is available, drop it
		if(bombY == 0) {
			bombY = planeY - unitheight;
			bombX = planeX;
			if(bombX >= canvaswidth - BOMB_RADIUS)
				bombX = canvaswidth - BOMB_RADIUS - 1;
			if(bombX <= BOMB_RADIUS)
				bombX = BOMB_RADIUS + 1;
		}
	}
	
	/**
	 * Main loop. Gets the canvas and manages the states.
	 */
	public void run() {
		restart();
		while(running) {
			Canvas c = null;
			try {
				synchronized(holder) {
						c = holder.lockCanvas(null);
						switch(state) {
							case STATE_PLAYING:
								if(lives < 0) {
									draw(c);
									drawgameover(c);
									state = STATE_GAMEOVER;
								} else {
									update();
									draw(c);
								}
								break;
							case STATE_GAMEOVER:
								draw(c);
								drawgameover(c);
								break;
							case STATE_LEVELUP:
								draw(c);
								drawlevelup(c);
								break;
							case STATE_NEWGAME:
								draw(c);
								drawnewgame(c);
								break;
							case STATE_PAUSED:
								draw(c);
								break;
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
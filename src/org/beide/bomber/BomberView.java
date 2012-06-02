package org.beide.bomber;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

public class BomberView extends SurfaceView implements SurfaceHolder.Callback {
	
	public String TAG = "Bomber";
	
	public class BomberThread extends Thread implements View.OnTouchListener {
		
		int[][] levels = {
			{0,1,2,3,4,5,4,3,2,1},
			{5,5,5,5,5,5,5,4,5,5},
			{9,8,7,6,5,4,3,2,1,0}
		};
		
		int UNITS_HORIZONTAL = 10;
		int UNITS_VERTICAL = 18;
		
		// Size of bomb
		int BOMB_RADIUS = 5;
		
		int PLANE_START_HEIGHT = 17;
		
		SurfaceHolder holder;
		Context context;
		
		Paint towerpaint, planepaint, bombpaint, textpaint;
		
		boolean running = true;
		boolean paused;
		
		int canvaswidth;
		int canvasheight;
		
		// Size of one unit
		int unitheight;
		int unitwidth;
		
		int score;
		int level;
		float bombX, bombY, bombgravity;
		float planeX, planeY, velocity, planegravity, planestart;
		int[] towers;
		
		public BomberThread(SurfaceHolder hold, Context c) {
			holder = hold;
			context = c;
			
			towerpaint = new Paint();
			towerpaint.setColor(Color.RED);
			
			planepaint = new Paint();
			planepaint.setColor(Color.BLACK);
			
			bombpaint = new Paint();
			bombpaint.setColor(Color.BLACK);
			
			textpaint = new Paint();
			textpaint.setColor(Color.YELLOW);
		}
		
		public void setRunning(boolean run) {
			running = run;
		}
		
		public void setPaused(boolean pause) {
			paused = pause;
		}
		
		public void surfaceSize(int width, int height) {
			synchronized(holder) {
				canvasheight = height;
				canvaswidth = width;
				unitwidth = width / UNITS_HORIZONTAL;
				unitheight = height / UNITS_VERTICAL;
				planestart = PLANE_START_HEIGHT * unitheight;
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
				level = lvl;
				bombY = 0;
				planeY = planestart;
				planeX = 0;
				towers = (int[]) levels[lvl].clone();
				bombgravity = 1000 / canvaswidth;
				velocity = 1000 / canvaswidth;
				planegravity = unitheight;
			}
		}
		
		public void draw(Canvas canvas) {
			canvas.drawColor(Color.BLUE);
			
			// Draw the towers
			for(int i = 0; i < towers.length; i++) {
				canvas.drawRect(i * unitwidth, canvasheight - towers[i] * unitheight,
												(i + 1) * unitwidth, canvasheight, towerpaint);
			}
			
			// Draw the plane
			canvas.drawRect(planeX - unitwidth, canvasheight - planeY + unitheight - 1,
											planeX, canvasheight - planeY - 1, planepaint);
			
			if(bombY > 0) {
				// Draw the bomb
				canvas.drawRect(bombX - BOMB_RADIUS, canvasheight - bombY + BOMB_RADIUS,
												bombX + BOMB_RADIUS, canvasheight - bombY - BOMB_RADIUS, bombpaint);
			}
			
			canvas.drawText("Score: " + score, (float) 0, canvasheight, textpaint);
			canvas.drawText("Level: " + (level + 1), (float) 0, canvasheight - unitheight, textpaint);
		}
		
		public void update() {
			
			synchronized(holder) {
				if(bombY != 0) {
					bombY -= bombgravity;
					if(towers[(int) bombX / unitwidth] * unitheight >= bombY || bombY >= canvasheight) {
						towers[(int) bombX / unitwidth]--;
						
						score++;
						bombY = 0;
					}
				}
				
				planeX += velocity;
				if(planeX >= canvaswidth) {
					planeX = 0;
					planeY -= planegravity;
				}
				
				if(towers[(int) planeX / unitwidth] * unitheight >= planeY) {
					gameover();
				}
				
				for(int i = 0; i < towers.length; i++) {
					if(towers[i] > 0) {
						return;
					}
				}
				
				initlevel(level + 1);
			}
		}
		
		public void gameover() {
			Log.i(TAG, "Game over! Score: " + score);
			score = 0;
			initlevel(0);
		}
		
		public boolean onTouch(View v, MotionEvent event) {
			// If the bomb is available, drop it
			if(bombY == 0) {
				bombY = planeY - unitheight;
				bombX = planeX - (planeX % unitwidth) + unitwidth / 2;
			}
			
			return true;
		}
		
		public void run() {
			restart();
			while(running) {
				Canvas c = null;
				try {
					c = holder.lockCanvas(null);
					synchronized(holder) {
						if(!paused) update();
						draw(c);
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
	
	BomberThread thread;
	Context context;
	
	public BomberThread getThread() {
		return thread;
	}
	
	public BomberView(Context c) {
		super(c);
		
		context = c;
		
		SurfaceHolder holder = getHolder();
		holder.addCallback(this);
		
		thread = new BomberThread(holder, c);
		
		setFocusable(true);
	}
	
	public void surfaceCreated(SurfaceHolder holder) {
		holder.addCallback(this);
		setOnTouchListener(thread);
		thread.setRunning(true);
		thread.start();
	}
	
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		thread.surfaceSize(w, h);
	}
	
	
	public void surfaceDestroyed(SurfaceHolder holder) {
		thread.setRunning(false);
		
		// Bad things will happen without this
		boolean retry = true;
		while (retry) {
			try {
				thread.join();
				retry = false;
			} catch (InterruptedException e) {
			}
		}
	}
}
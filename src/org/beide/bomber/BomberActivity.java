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

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;

public class BomberActivity extends Activity {
	
	private static final int MENU_RESTART = 1;
	private static final int MENU_SETTINGS = 2;
	
	private BomberView view;
	private BomberThread thread;
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		view = new BomberView(getApplicationContext());
		thread = view.getThread();
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		setContentView(view);
	}
	
	public void onResume() {
		super.onResume();
		thread.setPaused(false);
	}
	
	public void onPause() {
		super.onPause();
		thread.setPaused(true);
	}
	
	public void onOptionsMenuClosed(Menu menu) {
		thread.setPaused(false);
	}
	
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		
		menu.add(0, MENU_RESTART, 1, R.string.menu_restart);
		menu.add(0, MENU_SETTINGS, 1, R.string.menu_settings);
		
		return true;
	}
	
	public boolean onMenuOpened(int featureId, Menu menu) {
		thread.setPaused(true);
		return true;
	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		switch(item.getItemId()) {
			case MENU_RESTART:
				thread.restart();
				thread.setPaused(false);
			case MENU_SETTINGS:
				startActivity(new Intent(this, BomberPreferences.class));
		}
		return true;
	}
}

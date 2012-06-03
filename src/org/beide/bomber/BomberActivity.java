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
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

public class BomberActivity extends Activity {
	
	private static final int MENU_RESTART = 1;
	
	private BomberView view;
	private BomberThread thread;
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		view = new BomberView(getApplicationContext());
		thread = view.getThread();
		
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
	
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		
		menu.add(0, MENU_RESTART, 1, R.string.menu_restart);
		
		return true;
	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		
		thread.restart();
		return true;
	}
}

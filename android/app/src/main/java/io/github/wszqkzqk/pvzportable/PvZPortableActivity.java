/*
 *
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of ResoddedFramework.
 *
 * ResoddedFramework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ResoddedFramework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ResoddedFramework. If not, see <https://www.gnu.org/licenses/>.
 */

package io.github.electr0gunner.resoddedframework;;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

import java.io.File;

public class PvZPortableActivity extends SDLActivity {
    private static final String TAG = "PvZPortable";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        File extDir = getExternalFilesDir(null);
        if (extDir != null && !extDir.exists()) extDir.mkdirs();
        Log.i(TAG, "Resource dir: " + (extDir != null ? extDir.getAbsolutePath() : "null"));

        if (!hasGameResources(extDir)) {
            super.onCreate(savedInstanceState);
            startActivity(new Intent(this, ResourceImportActivity.class));
            finish();
            return;
        }

        super.onCreate(savedInstanceState);
        hideSystemUI();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) hideSystemUI();
    }

    private void hideSystemUI() {
        Window window = getWindow();
        if (window == null || window.peekDecorView() == null) {
            return;
        }
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                window.setDecorFitsSystemWindows(false);
                WindowInsetsController controller = window.getInsetsController();
                if (controller != null) {
                    controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                    controller.setSystemBarsBehavior(
                        WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
                }
            } else {
                window.getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed to hide system UI", e);
        }
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
            "bass",
            "main"
        };
    }

    @Override
    public void setRequestedOrientation(int requestedOrientation) {
        if (requestedOrientation == android.content.pm.ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE) {
            requestedOrientation = android.content.pm.ActivityInfo.SCREEN_ORIENTATION_USER_LANDSCAPE;
        }
        super.setRequestedOrientation(requestedOrientation);
    }

    private static boolean hasGameResources(File dir) {
        if (dir == null || !dir.isDirectory()) return false;
        File pak = new File(dir, "main.pak");
        File props = new File(dir, "properties");
        return pak.exists() && props.isDirectory();
    }
}

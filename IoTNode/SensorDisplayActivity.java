package analog.com.IoTNode;

import analog.com.IoTNode.sensors.GenericSensor;
import analog.com.IoTNode.sensors.MainSensor;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Bundle;
import android.support.p003v7.app.AppCompatActivity;
import android.view.View;
import android.view.animation.DecelerateInterpolator;
import android.widget.AdapterView;
import android.widget.FrameLayout;
import android.widget.ListView;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;
import java.util.ArrayList;

public class SensorDisplayActivity extends AppCompatActivity {
    static final /* synthetic */ boolean $assertionsDisabled = (!SensorDisplayActivity.class.desiredAssertionStatus());
    private final CheckLastDataReceived mCheckLastDataReceived = new CheckLastDataReceived();
    /* access modifiers changed from: private */
    public Animator mCurrentAnimator;
    /* access modifiers changed from: private */
    public long mLastReceivedData;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals("ADI_BLE_CHARACTERISTIC_CHANGED")) {
                long unused = SensorDisplayActivity.this.mLastReceivedData = System.currentTimeMillis();
                SensorDisplayActivity.this.mSensorDisplayListAdapter.handlePacket(intent.getExtras().getByteArray("NEW_VALUE"));
                GraphView graph = (GraphView) SensorDisplayActivity.this.findViewById(C0009R.C0011id.sensorGraphViewBig);
                if (graph.getVisibility() == 0 && SensorDisplayActivity.this.zoomedSeries != null) {
                    Utilities.updateGraphAxis(graph, SensorDisplayActivity.this.zoomedSeries);
                }
            }
            if (action.equals("ADI_BLE_DISCONNECTED_EVENT")) {
                SensorDisplayActivity.this.finish();
            }
        }
    };
    /* access modifiers changed from: private */
    public SensorDisplayListAdapter mSensorDisplayListAdapter;
    /* access modifiers changed from: private */
    public ArrayList<LineGraphSeries<DataPoint>> zoomedSeries = null;

    /* access modifiers changed from: protected */
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView((int) C0009R.layout.sensor_display);
        IntentFilter filter = new IntentFilter();
        filter.addAction("ADI_BLE_CHARACTERISTIC_CHANGED");
        filter.addAction("ADI_BLE_DISCONNECTED_EVENT");
        registerReceiver(this.mReceiver, filter);
        ListView mListView = (ListView) findViewById(C0009R.C0011id.sensorList);
        if ($assertionsDisabled || mListView != null) {
            this.mSensorDisplayListAdapter = new SensorDisplayListAdapter(getApplicationContext());
            mListView.setAdapter(this.mSensorDisplayListAdapter);
            mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                    MainSensor mListEntry = (MainSensor) adapterView.getItemAtPosition(i);
                    if (!(mListEntry instanceof GenericSensor)) {
                        ArrayList unused = SensorDisplayActivity.this.zoomedSeries = mListEntry.getSeries();
                        SensorDisplayActivity.this.zoomImage(SensorDisplayActivity.this.zoomedSeries);
                    }
                }
            });
            this.mLastReceivedData = System.currentTimeMillis();
            new Thread(this.mCheckLastDataReceived).start();
            getWindow().addFlags(128);
            return;
        }
        throw new AssertionError();
    }

    /* access modifiers changed from: protected */
    public void onDestroy() {
        super.onDestroy();
        sendBroadcast(new Intent("ADI_BLE_GATT_DISCONNECT"));
        unregisterReceiver(this.mReceiver);
        this.mCheckLastDataReceived.stop();
    }

    public void onDisconnect(View v) {
        finish();
    }

    /* access modifiers changed from: private */
    public void zoomImage(ArrayList<LineGraphSeries<DataPoint>> Series) {
        float startScale;
        if (this.mCurrentAnimator != null) {
            this.mCurrentAnimator.cancel();
        }
        final GraphView BigGraph = (GraphView) findViewById(C0009R.C0011id.sensorGraphViewBig);
        if ($assertionsDisabled || BigGraph != null) {
            Utilities.setGraphVisuals(BigGraph);
            Utilities.addGraphSeries(BigGraph, Series);
            final Rect startBounds = new Rect();
            Rect finalBounds = new Rect();
            Point globalOffset = new Point();
            FrameLayout layout = (FrameLayout) findViewById(C0009R.C0011id.container);
            if (layout != null) {
                layout.getGlobalVisibleRect(finalBounds, globalOffset);
            }
            startBounds.offset(-globalOffset.x, -globalOffset.y);
            finalBounds.offset(-globalOffset.x, -globalOffset.y);
            if (((float) finalBounds.width()) / ((float) finalBounds.height()) > ((float) startBounds.width()) / ((float) startBounds.height())) {
                startScale = ((float) startBounds.height()) / ((float) finalBounds.height());
                float deltaWidth = ((startScale * ((float) finalBounds.width())) - ((float) startBounds.width())) / 2.0f;
                startBounds.left = (int) (((float) startBounds.left) - deltaWidth);
                startBounds.right = (int) (((float) startBounds.right) + deltaWidth);
            } else {
                startScale = ((float) startBounds.width()) / ((float) finalBounds.width());
                float deltaHeight = ((startScale * ((float) finalBounds.height())) - ((float) startBounds.height())) / 2.0f;
                startBounds.top = (int) (((float) startBounds.top) - deltaHeight);
                startBounds.bottom = (int) (((float) startBounds.bottom) + deltaHeight);
            }
            BigGraph.setVisibility(0);
            BigGraph.setBackgroundColor(-1);
            AnimatorSet set = new AnimatorSet();
            set.play(ObjectAnimator.ofFloat(BigGraph, View.X, new float[]{(float) startBounds.left, (float) finalBounds.left})).with(ObjectAnimator.ofFloat(BigGraph, View.Y, new float[]{(float) startBounds.top, (float) finalBounds.top})).with(ObjectAnimator.ofFloat(BigGraph, View.SCALE_X, new float[]{startScale, 1.0f})).with(ObjectAnimator.ofFloat(BigGraph, View.SCALE_Y, new float[]{startScale, 1.0f}));
            set.setDuration((long) getResources().getInteger(17694720));
            set.setInterpolator(new DecelerateInterpolator());
            set.addListener(new AnimatorListenerAdapter() {
                public void onAnimationEnd(Animator animation) {
                    Animator unused = SensorDisplayActivity.this.mCurrentAnimator = null;
                }

                public void onAnimationCancel(Animator animation) {
                    Animator unused = SensorDisplayActivity.this.mCurrentAnimator = null;
                }
            });
            set.start();
            this.mCurrentAnimator = set;
            final float startScaleFinal = startScale;
            BigGraph.setOnClickListener(new View.OnClickListener() {
                public void onClick(View view) {
                    AnimatorSet set = new AnimatorSet();
                    set.play(ObjectAnimator.ofFloat(BigGraph, View.X, new float[]{(float) startBounds.left})).with(ObjectAnimator.ofFloat(BigGraph, View.Y, new float[]{(float) startBounds.top})).with(ObjectAnimator.ofFloat(BigGraph, View.SCALE_X, new float[]{startScaleFinal})).with(ObjectAnimator.ofFloat(BigGraph, View.SCALE_Y, new float[]{startScaleFinal}));
                    set.setDuration((long) SensorDisplayActivity.this.getResources().getInteger(17694720));
                    set.setInterpolator(new DecelerateInterpolator());
                    set.addListener(new AnimatorListenerAdapter() {
                        public void onAnimationEnd(Animator animation) {
                            BigGraph.removeAllSeries();
                            BigGraph.setVisibility(4);
                            Animator unused = SensorDisplayActivity.this.mCurrentAnimator = null;
                        }

                        public void onAnimationCancel(Animator animation) {
                            BigGraph.removeAllSeries();
                            BigGraph.setVisibility(4);
                            Animator unused = SensorDisplayActivity.this.mCurrentAnimator = null;
                        }
                    });
                    set.start();
                    Animator unused = SensorDisplayActivity.this.mCurrentAnimator = set;
                }
            });
            return;
        }
        throw new AssertionError();
    }

    private class CheckLastDataReceived implements Runnable {
        private volatile boolean exit;

        private CheckLastDataReceived() {
            this.exit = false;
        }

        public void run() {
            while (!this.exit) {
                if (System.currentTimeMillis() - SensorDisplayActivity.this.mLastReceivedData > 900000) {
                    SensorDisplayActivity.this.mSensorDisplayListAdapter.notifyDataSetChanged();
                    long unused = SensorDisplayActivity.this.mLastReceivedData = System.currentTimeMillis();
                }
                try {
                    Thread.sleep(1000);
                } catch (Exception e) {
                    System.out.println("Couldn't sleep");
                }
            }
        }

        /* access modifiers changed from: package-private */
        public void stop() {
            this.exit = true;
        }
    }
}
